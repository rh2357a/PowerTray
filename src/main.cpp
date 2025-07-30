#include "main.h"

#include "resources.h"
#include "utils.h"
#include "api/power.h"
#include "api/windows.h"
#include "settings/app.h"

#include <windows.h>

#include <memory>
#include <format>
#include <vector>

constexpr int WM_SYSTEM_TRAY = WM_APP + 0;

enum app_menu : UINT
{
	MODE_BEGIN = 1,

	PSR = 5,
	AUTO_START,

	EXIT,

	PROFILE_EDIT,
	PROFILE_ITEM_BEGIN,
};

HWND handle;
HMENU main_menu = nullptr, profile_menu = nullptr;
std::vector<api::power::profile> recent_profiles;

void win_main(std::vector<std::string> args)
{
	if (std::find(args.begin(), args.end(), "--toggle-psr") != args.end())
		toggle_psr();

	WNDCLASS wc{};
	wc.lpfnWndProc = wnd_proc;
	wc.hInstance = ::GetModuleHandle(nullptr);

	auto class_name = std::format(L"{}Class", APP_NAME);
	wc.lpszClassName = class_name.c_str();

	::RegisterClass(&wc);

	handle = ::CreateWindowEx(0, wc.lpszClassName, APP_NAME, 0, 0, 0, 0, 0, 0, 0, 0, wc.hInstance);

	NOTIFYICONDATA nid{};
	nid.cbSize = sizeof(NOTIFYICONDATA);
	nid.hWnd = handle;
	nid.uID = 1;
	nid.uFlags = NIF_ICON | NIF_TIP | NIF_MESSAGE;
	nid.uCallbackMessage = WM_SYSTEM_TRAY;
	nid.hIcon = (HICON)::LoadImage(::GetModuleHandle(nullptr), MAKEINTRESOURCE(IDI_APP), IMAGE_ICON, 16, 16, LR_DEFAULTCOLOR);

	auto &tip = APP_DESCRIPTION;
	utils::strings::wstring_copy(nid.szTip, tip);

	::Shell_NotifyIcon(NIM_ADD, &nid);

	on_menu_create();

	MSG msg{};
	while (::GetMessage(&msg, nullptr, 0, 0))
	{
		::TranslateMessage(&msg);
		::DispatchMessage(&msg);
	}

	::Shell_NotifyIcon(NIM_DELETE, &nid);
}

LRESULT wnd_proc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	if (msg == WM_SYSTEM_TRAY && (lparam == WM_RBUTTONUP || lparam == WM_LBUTTONUP))
	{
		on_menu_update();
		on_menu_show();
	}
	else if (msg == WM_DESTROY)
	{
		on_app_exit();
	}

	return ::DefWindowProc(hwnd, msg, wparam, lparam);
}

void on_app_exit()
{
	if (main_menu)
		::DestroyMenu(main_menu);
	if (profile_menu)
		::DestroyMenu(profile_menu);

	::PostQuitMessage(0);
}

void on_menu_create()
{
	main_menu = ::CreatePopupMenu();
	profile_menu = ::CreatePopupMenu();

	for (size_t i = 0; i < api::power::mode::MODES.size(); i++)
	{
		const auto &mode = api::power::mode::MODES[i];
		::AppendMenu(main_menu, MF_STRING, app_menu::MODE_BEGIN + i, mode.name.c_str());
	}

	::AppendMenu(main_menu, MF_SEPARATOR, 0, nullptr);
	::AppendMenu(main_menu, MF_POPUP, (UINT_PTR)profile_menu, L"전원 프로필");
	::AppendMenu(profile_menu, MF_STRING, app_menu::PROFILE_EDIT, L"편집...");
	::AppendMenu(profile_menu, MF_SEPARATOR, 0, nullptr);

	::AppendMenu(main_menu, MF_SEPARATOR, 0, nullptr);
	::AppendMenu(main_menu, MF_STRING, app_menu::PSR, L"PSR 활성화");

	::AppendMenu(main_menu, MF_SEPARATOR, 0, nullptr);
	::AppendMenu(main_menu, MF_STRING, app_menu::AUTO_START, L"Windows 시작 시 자동 실행");

	::AppendMenu(main_menu, MF_SEPARATOR, 0, nullptr);
	::AppendMenu(main_menu, MF_STRING, app_menu::EXIT, L"끝내기");
}

void on_menu_update()
{
	auto &current_mode = api::power::get_power_mode();
	for (size_t i = 0; i < api::power::mode::MODES.size(); i++)
	{
		const auto &mode = api::power::mode::MODES[i];
		const auto current = current_mode.guid == mode.guid;
		::CheckMenuItem(main_menu, app_menu::MODE_BEGIN + i, current ? MF_CHECKED : MF_UNCHECKED);
	}

	::CheckMenuItem(main_menu, app_menu::PSR, settings::app::is_psr_enabled() ? MF_CHECKED : MF_UNCHECKED);
	::CheckMenuItem(main_menu, app_menu::AUTO_START, settings::app::is_auto_start() ? MF_CHECKED : MF_UNCHECKED);

	for (size_t i = 0; i < recent_profiles.size(); i++)
	{
		const auto &profile = recent_profiles[i];
		::DeleteMenu(profile_menu, app_menu::PROFILE_ITEM_BEGIN + i, MF_BYCOMMAND);
	}

	recent_profiles = api::power::get_power_profiles();

	for (size_t i = 0; i < recent_profiles.size(); i++)
	{
		const auto &profile = recent_profiles[i];
		auto name = profile.name.c_str();
		::AppendMenu(
			profile_menu,
			MF_STRING | (profile.enabled ? MF_CHECKED : MF_UNCHECKED),
			app_menu::PROFILE_ITEM_BEGIN + i,
			name);
	}
}

void on_menu_show()
{
	POINT pt;
	::GetCursorPos(&pt);

	::SetForegroundWindow(handle);

	UINT cmd = ::TrackPopupMenu(main_menu, TPM_BOTTOMALIGN | TPM_LEFTBUTTON | TPM_RETURNCMD, pt.x, pt.y, 0, handle, nullptr);

	if (cmd == app_menu::PSR)
	{
		toggle_psr();
		return;
	}
	else if (cmd == app_menu::AUTO_START)
	{
		auto enabled = !settings::app::is_auto_start();
		settings::app::set_auto_start(enabled);
		return;
	}
	else if (cmd == app_menu::EXIT)
	{
		on_app_exit();
	}
	else if (cmd == app_menu::PROFILE_EDIT)
	{
		api::windows::run_process("control powercfg.cpl");
	}
	else if (cmd >= app_menu::MODE_BEGIN && cmd < app_menu::MODE_BEGIN + static_cast<UINT>(api::power::mode::MODES.size()))
	{
		int index = cmd - app_menu::MODE_BEGIN;
		auto &mode = api::power::mode::MODES[index];
		api::power::apply_power_mode(mode);
		return;
	}
	else if (cmd >= app_menu::PROFILE_ITEM_BEGIN)
	{
		const auto &profile = recent_profiles[cmd - app_menu::PROFILE_ITEM_BEGIN];
		api::power::apply_power_profile(profile);
	}
}

void toggle_psr()
{
	if (api::windows::is_user_administrator())
	{
		auto enabled = !settings::app::is_psr_enabled();
		settings::app::set_psr_enabled(enabled);
	}
	else
	{
		api::windows::restart_as_administrator();
	}
}
