#include <string>
#include <iostream>

#include <windows.h>

#include "settings.h"

static constexpr const auto NAME = L"PowerTray";
static constexpr const auto REG_PATH = L"Software\\Microsoft\\Windows\\CurrentVersion\\Run";

bool IsStartupEnabled()
{
    HKEY hKey;
    LONG result = ::RegOpenKeyEx(HKEY_CURRENT_USER, REG_PATH, 0, KEY_READ, &hKey);

    if (result != ERROR_SUCCESS)
        return false;

    char value[260];
    DWORD bufferSize = sizeof(value);
    result = ::RegQueryValueEx(hKey, NAME, nullptr, nullptr, (LPBYTE)value, &bufferSize);

    ::RegCloseKey(hKey);

    return result == ERROR_SUCCESS;
}

void SetStartupEnabled(bool isEnabled)
{
    HKEY hKey;
    LONG result = RegOpenKeyEx(HKEY_CURRENT_USER, REG_PATH, 0, KEY_WRITE, &hKey);

    if (result != ERROR_SUCCESS)
        return;

    if (isEnabled)
    {
        wchar_t path[MAX_PATH];
        auto len = ::GetModuleFileName(nullptr, path, sizeof(path));
        ::RegSetValueEx(hKey, NAME, 0, REG_SZ, (const BYTE *)path, len * sizeof(wchar_t));
    }
    else
    {
        ::RegDeleteValue(hKey, NAME);
    }

    ::RegCloseKey(hKey);
}
