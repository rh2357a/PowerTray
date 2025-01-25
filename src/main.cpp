// 정렬 고정: 항상 먼저 정의되어 있어야 함.
#include <windows.h>

#include <shellapi.h>

#include "powerapi.h"
#include "settings.h"
#include "utils.h"

#define APP_NAME L"PowerTray v1.3.0.0"

#define MENU_POWER_MDOE_BEST_PERFORMANCE 1
#define MENU_POWER_MDOE_BETTER_PERFORMANCE 2
#define MENU_POWER_MDOE_BEST_BATTERY 3

#define MENU_STARTUP 4

#define MENU_EXIT 5

#define MENU_PROFILE_EDIT 6
#define MENU_PROFILE_ITEM 7

HMENU menu = nullptr;
HMENU profileMenu = nullptr;

std::vector<PowerProfileNode> recentProfiles;

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
    InitializePowerAPI();

    WNDCLASS wc = {};
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = GetModuleHandle(nullptr);
    wc.lpszClassName = TEXT("TrayAppClass");
    ::RegisterClass(&wc);

    HWND hwnd = ::CreateWindowEx(0, wc.lpszClassName, APP_NAME, 0, 0, 0, 0, 0, 0, 0, 0, wc.hInstance);

    NOTIFYICONDATA nid = {};
    nid.cbSize = sizeof(NOTIFYICONDATA);
    nid.hWnd = hwnd;
    nid.uID = 1;
    nid.uFlags = NIF_ICON | NIF_TIP | NIF_MESSAGE;
    nid.uCallbackMessage = WM_APP;
    nid.hIcon = ::LoadIcon(nullptr, IDI_APPLICATION);
    lstrcpyn(nid.szTip, APP_NAME, ARRAYSIZE(nid.szTip));

    ::Shell_NotifyIcon(NIM_ADD, &nid);

    MSG msg;
    while (::GetMessage(&msg, nullptr, 0, 0))
    {
        ::TranslateMessage(&msg);
        ::DispatchMessage(&msg);
    }

    ::Shell_NotifyIcon(NIM_DELETE, &nid);

    DestroyPowerAPI();

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

    ::AppendMenu(menu, MF_STRING, MENU_STARTUP, L"Windows 시작 시 자동 실행");
    ::AppendMenu(menu, MF_SEPARATOR, 0, nullptr);

    ::AppendMenu(menu, MF_STRING, MENU_EXIT, L"끝내기");
}

void OnUpdateTrayMenu()
{
    auto currentScheme = PowerGetEffectiveOverlayScheme();

    ::CheckMenuItem(
        menu,
        MENU_POWER_MDOE_BEST_PERFORMANCE,
        currentScheme == POWER_OVERLAY_SCHEME_BEST_PERFORMANCE ? MF_CHECKED : MF_UNCHECKED);

    ::CheckMenuItem(
        menu,
        MENU_POWER_MDOE_BETTER_PERFORMANCE,
        currentScheme == POWER_OVERLAY_SCHEME_BETTER_PERFORMANCE ? MF_CHECKED : MF_UNCHECKED);

    ::CheckMenuItem(
        menu,
        MENU_POWER_MDOE_BEST_BATTERY,
        currentScheme == POWER_OVERLAY_SCHEME_BETTER_BATTERY ? MF_CHECKED : MF_UNCHECKED);

    ::CheckMenuItem(menu, MENU_STARTUP, IsStartupEnabled() ? MF_CHECKED : MF_UNCHECKED);

    int i = 0;
    for (auto &scheme : recentProfiles)
    {
        ::DeleteMenu(profileMenu, MENU_PROFILE_ITEM + i, MF_BYCOMMAND);
        i++;
    }

    recentProfiles = GetPowerProfiles();
    auto currentProfileScheme = *GetCurrentPowerProfile();

    i = 0;
    for (auto &scheme : recentProfiles)
    {
        auto name = scheme.friendlyName.c_str();
        ::AppendMenu(
            profileMenu,
            MF_STRING | (currentProfileScheme == scheme.schemeGuid ? MF_CHECKED : MF_UNCHECKED),
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
        ::PowerSetActiveScheme(nullptr, &scheme.schemeGuid);
        return;
    }

    switch (cmd)
    {
    case MENU_POWER_MDOE_BEST_PERFORMANCE:
        PowerSetActiveOverlayScheme(POWER_OVERLAY_SCHEME_BEST_PERFORMANCE);
        break;

    case MENU_POWER_MDOE_BETTER_PERFORMANCE:
        PowerSetActiveOverlayScheme(POWER_OVERLAY_SCHEME_BETTER_PERFORMANCE);
        break;

    case MENU_POWER_MDOE_BEST_BATTERY:
        PowerSetActiveOverlayScheme(POWER_OVERLAY_SCHEME_BETTER_BATTERY);
        break;

    case MENU_STARTUP:
        SetStartupEnabled(!IsStartupEnabled());
        break;

    case MENU_EXIT:
        OnAppExit();
        break;

    case MENU_PROFILE_EDIT:
        STARTUPINFOA si = {sizeof(STARTUPINFOA)};
        PROCESS_INFORMATION pi;

        auto cmdline = "control powercfg.cpl";
        if (CreateProcessA(nullptr, (LPSTR)cmdline, nullptr, nullptr, false, 0, nullptr, nullptr, &si, &pi))
        {
            CloseHandle(pi.hProcess);
            CloseHandle(pi.hThread);
        }
        break;
    }
}

void OnAppExit()
{
    if (menu != nullptr)
        ::DestroyMenu(menu);
    ::PostQuitMessage(0);
}
