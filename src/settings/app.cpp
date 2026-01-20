#include "app.h"

#include "utils.h"
#include "resources.h"

#include <windows.h>

namespace settings::app {

constexpr const auto REG_PATH_STARTUP = L"Software\\Microsoft\\Windows\\CurrentVersion\\Run";

constexpr const auto REG_KEY_PSR_FEATURE = L"DalPSRFeatureEnable";
constexpr const auto REG_PATH_PSR_FEATURE = L"SYSTEM\\CurrentControlSet\\Control\\Class\\{4d36e968-e325-11ce-bfc1-08002be10318}\\0000";

constexpr const auto REG_KEY_MPO_FEATURE = L"OverlayTestMode";
constexpr const auto REG_PATH_MPO_FEATURE = L"SOFTWARE\\Microsoft\\Windows\\Dwm";

bool is_auto_start()
{
    HKEY key;

    LONG result = ::RegOpenKeyEx(HKEY_CURRENT_USER, REG_PATH_STARTUP, 0, KEY_READ, &key);
    if (result != ERROR_SUCCESS)
        return false;

    LPBYTE filename[MAX_PATH];
    DWORD len = sizeof(filename);
    auto app_name = utils::strings::to_wstring(APP_NAME);
    result = ::RegQueryValueEx(key, app_name.c_str(), nullptr, nullptr, (LPBYTE)filename, &len);

    ::RegCloseKey(key);

    return result == ERROR_SUCCESS;
}

void set_auto_start(bool enabled)
{
    HKEY key;

    LONG result = ::RegOpenKeyEx(HKEY_CURRENT_USER, REG_PATH_STARTUP, 0, KEY_WRITE, &key);
    if (result != ERROR_SUCCESS)
        return;

    auto app_name = utils::strings::to_wstring(APP_NAME);

    if (enabled)
    {
        wchar_t filename[MAX_PATH];
        auto len = ::GetModuleFileName(nullptr, filename, sizeof(filename));
        ::RegSetValueEx(key, app_name.c_str(), 0, REG_SZ, (const BYTE *)filename, len * sizeof(wchar_t));
    }
    else
    {
        ::RegDeleteValue(key, app_name.c_str());
    }

    ::RegCloseKey(key);
}

bool is_psr_enabled()
{
    HKEY key;

    LONG result = ::RegOpenKeyExW(HKEY_LOCAL_MACHINE, REG_PATH_PSR_FEATURE, 0, KEY_READ, &key);
    if (result != ERROR_SUCCESS)
        return false;

    DWORD value = 0;
    DWORD size = sizeof(value);
    DWORD type = 0;

    result = ::RegQueryValueExW(key, REG_KEY_PSR_FEATURE, nullptr, &type, reinterpret_cast<LPBYTE>(&value), &size);
    ::RegCloseKey(key);

    if (result != ERROR_SUCCESS || type != REG_DWORD)
        return false;

    return value != 0;
}

void set_psr_enabled(bool enabled)
{
    HKEY key;

    LONG result = ::RegOpenKeyEx(HKEY_LOCAL_MACHINE, REG_PATH_PSR_FEATURE, 0, KEY_WRITE, &key);
    if (result != ERROR_SUCCESS)
        return;

    auto value = enabled ? 1 : 0;
    ::RegSetValueEx(key, REG_KEY_PSR_FEATURE, 0, REG_DWORD, reinterpret_cast<const BYTE *>(&value), sizeof(DWORD));

    ::RegCloseKey(key);
}

bool is_mpo_enabled()
{
    HKEY key;

    LONG result = ::RegOpenKeyExW(HKEY_LOCAL_MACHINE, REG_PATH_MPO_FEATURE, 0, KEY_READ, &key);
    if (result != ERROR_SUCCESS)
        return false;

    DWORD value = 0;
    DWORD size = sizeof(value);
    DWORD type = 0;

    result = ::RegQueryValueExW(key, REG_KEY_MPO_FEATURE, nullptr, &type, reinterpret_cast<LPBYTE>(&value), &size);
    ::RegCloseKey(key);

    if (result != ERROR_SUCCESS || type != REG_DWORD)
        return true;

    return value != 5;
}

void set_mpo_enabled(bool enabled)
{
    HKEY key;

    LONG result = ::RegOpenKeyEx(HKEY_LOCAL_MACHINE, REG_PATH_MPO_FEATURE, 0, KEY_WRITE, &key);
    if (result != ERROR_SUCCESS)
        return;

    if (enabled)
    {
        ::RegDeleteValueW(key, REG_KEY_MPO_FEATURE);
    }
    else
    {
        DWORD value = 5;
        ::RegSetValueEx(key, REG_KEY_MPO_FEATURE, 0, REG_DWORD, reinterpret_cast<const BYTE *>(&value), sizeof(DWORD));
    }

    ::RegCloseKey(key);
}

} // namespace settings::app
