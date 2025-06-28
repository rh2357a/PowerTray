#include <windows.h>
#include <shellapi.h>

#include "resources.h"
#include "powerapi.h"
#include "app_settings.h"
#include "utils.h"

#define MENU_POWER_MDOE_BEST_PERFORMANCE   1
#define MENU_POWER_MDOE_BETTER_PERFORMANCE 2
#define MENU_POWER_MDOE_BEST_BATTERY       3

#define MENU_PSR_FEATURE 4
#define MENU_STARTUP     5

#define MENU_EXIT 6

#define MENU_PROFILE_EDIT 7
#define MENU_PROFILE_ITEM 8

HMENU menu = nullptr;
HMENU profileMenu = nullptr;

std::vector<power_profile_node> recentProfiles;

LRESULT CALLBACK WindowProc(HWND, UINT, WPARAM, LPARAM);
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int);
void OnCreateTrayMenu();
void OnUpdateTrayMenu();
void OnTrayMenuSelected(int);
void OnAppExit();

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_APP:
        if (lParam == WM_RBUTTONUP || lParam == WM_LBUTTONUP)
        {
            if (menu == nullptr)
            {
                menu = ::CreatePopupMenu();
                OnCreateTrayMenu();
            }

            OnUpdateTrayMenu();

            POINT pt;
            ::GetCursorPos(&pt);

            ::SetForegroundWindow(hwnd);
            int cmd = ::TrackPopupMenu(menu, TPM_BOTTOMALIGN | TPM_LEFTBUTTON | TPM_RETURNCMD, pt.x, pt.y, 0, hwnd, nullptr);
            OnTrayMenuSelected(cmd);
        }
        break;

    case WM_COMMAND:
        OnTrayMenuSelected(LOWORD(wParam));
        break;

    case WM_DESTROY:
        OnAppExit();
        break;
    }

    return ::DefWindowProc(hwnd, uMsg, wParam, lParam);
}

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
    WNDCLASS wc = {};
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = GetModuleHandle(nullptr);
    wc.lpszClassName = L"PowerTrayClass";
    ::RegisterClass(&wc);

    HWND hwnd = ::CreateWindowEx(0, wc.lpszClassName, APP_NAME, 0, 0, 0, 0, 0, 0, 0, 0, wc.hInstance);

    NOTIFYICONDATA nid = {};
    nid.cbSize = sizeof(NOTIFYICONDATA);
    nid.hWnd = hwnd;
    nid.uID = 1;
    nid.uFlags = NIF_ICON | NIF_TIP | NIF_MESSAGE;
    nid.uCallbackMessage = WM_APP;
    nid.hIcon = (HICON)::LoadImage(::GetModuleHandle(nullptr), MAKEINTRESOURCE(IDI_APP_ICON), IMAGE_ICON, 16, 16, LR_DEFAULTCOLOR);
    lstrcpyn(nid.szTip, APP_NAME, ARRAYSIZE(nid.szTip));

    ::Shell_NotifyIcon(NIM_ADD, &nid);

    MSG msg;
    while (::GetMessage(&msg, nullptr, 0, 0))
    {
        ::TranslateMessage(&msg);
        ::DispatchMessage(&msg);
    }

    ::Shell_NotifyIcon(NIM_DELETE, &nid);

    return 0;
}

void OnCreateTrayMenu()
{
    ::AppendMenu(menu, MF_STRING, MENU_POWER_MDOE_BEST_PERFORMANCE, L"최고의 성능");
    ::AppendMenu(menu, MF_STRING, MENU_POWER_MDOE_BETTER_PERFORMANCE, L"균형 잡힌");
    ::AppendMenu(menu, MF_STRING, MENU_POWER_MDOE_BEST_BATTERY, L"최고의 전원 효율성");
    ::AppendMenu(menu, MF_SEPARATOR, 0, nullptr);

    profileMenu = ::CreatePopupMenu();
    ::AppendMenu(menu, MF_POPUP, (UINT_PTR)profileMenu, L"전원 프로필");
    ::AppendMenu(profileMenu, MF_STRING, MENU_PROFILE_EDIT, L"편집...");
    ::AppendMenu(profileMenu, MF_SEPARATOR, 0, nullptr);
    ::AppendMenu(menu, MF_SEPARATOR, 0, nullptr);

    ::AppendMenu(menu, MF_STRING, MENU_PSR_FEATURE, L"PSR 활성화");
    ::AppendMenu(menu, MF_SEPARATOR, 0, nullptr);

    ::AppendMenu(menu, MF_STRING, MENU_STARTUP, L"Windows 시작 시 자동 실행");
    ::AppendMenu(menu, MF_SEPARATOR, 0, nullptr);

    ::AppendMenu(menu, MF_STRING, MENU_EXIT, L"끝내기");
}

void OnUpdateTrayMenu()
{
    auto currentScheme = power_api::get_overlay_scheme();

    ::CheckMenuItem(
        menu,
        MENU_POWER_MDOE_BEST_PERFORMANCE,
        currentScheme == power_api::SCHEME_BEST_PERFORMANCE ? MF_CHECKED : MF_UNCHECKED);

    ::CheckMenuItem(
        menu,
        MENU_POWER_MDOE_BETTER_PERFORMANCE,
        currentScheme == power_api::SCHEME_BETTER_PERFORMANCE ? MF_CHECKED : MF_UNCHECKED);

    ::CheckMenuItem(
        menu,
        MENU_POWER_MDOE_BEST_BATTERY,
        currentScheme == power_api::SCHEME_BETTER_BATTERY ? MF_CHECKED : MF_UNCHECKED);

    ::CheckMenuItem(menu, MENU_PSR_FEATURE, app_settings::is_psr_enabled() ? MF_CHECKED : MF_UNCHECKED);
    ::EnableMenuItem(menu, MENU_PSR_FEATURE, windows::is_administrator_enabled() ? MF_ENABLED : MF_DISABLED);

    ::CheckMenuItem(menu, MENU_STARTUP, app_settings::is_startup_enabled() ? MF_CHECKED : MF_UNCHECKED);

    int i = 0;
    for (auto &scheme : recentProfiles)
    {
        ::DeleteMenu(profileMenu, MENU_PROFILE_ITEM + i, MF_BYCOMMAND);
        i++;
    }

    recentProfiles = power_api::get_power_profiles();
    auto currentProfileScheme = power_api::get_power_profile();

    i = 0;
    for (auto &scheme : recentProfiles)
    {
        auto name = scheme.friendly_name.c_str();
        ::AppendMenu(
            profileMenu,
            MF_STRING | (currentProfileScheme == scheme.guid ? MF_CHECKED : MF_UNCHECKED),
            MENU_PROFILE_ITEM + i,
            name);
        i++;
    }
}

void OnTrayMenuSelected(int cmd)
{
    if (cmd >= MENU_PROFILE_ITEM)
    {
        int index = cmd - MENU_PROFILE_ITEM;
        auto scheme = recentProfiles[index];
        power_api::set_power_profile(scheme.guid);
        return;
    }

    switch (cmd)
    {
    case MENU_POWER_MDOE_BEST_PERFORMANCE:
        power_api::set_overlay_scheme(power_api::SCHEME_BEST_PERFORMANCE);
        break;

    case MENU_POWER_MDOE_BETTER_PERFORMANCE:
        power_api::set_overlay_scheme(power_api::SCHEME_BETTER_PERFORMANCE);
        break;

    case MENU_POWER_MDOE_BEST_BATTERY:
        power_api::set_overlay_scheme(power_api::SCHEME_BETTER_BATTERY);
        break;

    case MENU_PSR_FEATURE:
        app_settings::set_psr_enabled(!app_settings::is_psr_enabled());
        break;

    case MENU_STARTUP:
        app_settings::set_startup_enabled(!app_settings::is_startup_enabled());
        break;

    case MENU_EXIT:
        OnAppExit();
        break;

    case MENU_PROFILE_EDIT:
        windows::exec("control powercfg.cpl");
        break;
    }
}

void OnAppExit()
{
    if (menu != nullptr)
        ::DestroyMenu(menu);
    ::PostQuitMessage(0);
}
