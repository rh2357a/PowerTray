#include <string>
#include <iostream>

#include <windows.h>

#include "settings.h"

static constexpr const auto REG_KEY_APP_NAME = L"PowerTray";
static constexpr const auto REG_KEY_PSR_FEATURE = L"DalPSRFeatureEnable";

static constexpr const auto REG_PATH_STARTUP = L"Software\\Microsoft\\Windows\\CurrentVersion\\Run";
static constexpr const auto REG_PATH_PSR_FEATURE = L"SYSTEM\\CurrentControlSet\\Control\\Class\\{4d36e968-e325-11ce-bfc1-08002be10318}\\0000";

bool IsStartupEnabled()
{
    HKEY hKey;
    LONG result = ::RegOpenKeyEx(HKEY_CURRENT_USER, REG_PATH_STARTUP, 0, KEY_READ, &hKey);

    if (result != ERROR_SUCCESS)
        return false;

    char value[260];
    DWORD bufferSize = sizeof(value);
    result = ::RegQueryValueEx(hKey, REG_KEY_APP_NAME, nullptr, nullptr, (LPBYTE)value, &bufferSize);

    ::RegCloseKey(hKey);

    return result == ERROR_SUCCESS;
}

void SetStartupEnabled(bool isEnabled)
{
    HKEY hKey;
    LONG result = RegOpenKeyEx(HKEY_CURRENT_USER, REG_PATH_STARTUP, 0, KEY_WRITE, &hKey);

    if (result != ERROR_SUCCESS)
        return;

    if (isEnabled)
    {
        wchar_t path[MAX_PATH];
        auto len = ::GetModuleFileName(nullptr, path, sizeof(path));
        ::RegSetValueEx(hKey, REG_KEY_APP_NAME, 0, REG_SZ, (const BYTE *)path, len * sizeof(wchar_t));
    }
    else
    {
        ::RegDeleteValue(hKey, REG_KEY_APP_NAME);
    }

    ::RegCloseKey(hKey);
}

bool IsPsrFeatureEnabled()
{
    HKEY hKey;
    LONG result = ::RegOpenKeyExW(HKEY_LOCAL_MACHINE, REG_PATH_PSR_FEATURE, 0, KEY_READ, &hKey);
    if (result != ERROR_SUCCESS)
        return false;

    DWORD value = 0;
    DWORD size = sizeof(value);
    DWORD type = 0;

    result = ::RegQueryValueExW(hKey, REG_KEY_PSR_FEATURE, nullptr, &type, reinterpret_cast<LPBYTE>(&value), &size);
    ::RegCloseKey(hKey);

    if (result != ERROR_SUCCESS || type != REG_DWORD)
        return false;

    return value != 0;
}

void SetPsrFeatureEnabled(bool isEnabled)
{
    HKEY hKey;
    LONG result = RegOpenKeyEx(HKEY_LOCAL_MACHINE, REG_PATH_PSR_FEATURE, 0, KEY_WRITE, &hKey);

    if (result != ERROR_SUCCESS)
        return;

    auto value = isEnabled ? 1 : 0;
    ::RegSetValueEx(hKey, REG_KEY_PSR_FEATURE, 0, REG_DWORD, reinterpret_cast<const BYTE *>(&value), sizeof(DWORD));
    ::RegCloseKey(hKey);
}
