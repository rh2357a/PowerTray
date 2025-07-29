#include "app.h"

#include "resources.h"
#include <windows.h>

namespace settings::app {

constexpr const auto REG_KEY_APP_NAME = APP_NAME;
constexpr const auto REG_PATH_STARTUP = L"Software\\Microsoft\\Windows\\CurrentVersion\\Run";

bool is_auto_start()
{
	HKEY key;

	LONG result = ::RegOpenKeyEx(HKEY_CURRENT_USER, REG_PATH_STARTUP, 0, KEY_READ, &key);
	if (result != ERROR_SUCCESS)
		return false;

	LPBYTE filename[MAX_PATH];
	DWORD len = sizeof(filename);
	result = ::RegQueryValueEx(key, REG_KEY_APP_NAME, nullptr, nullptr, (LPBYTE)filename, &len);

	::RegCloseKey(key);

	return result == ERROR_SUCCESS;
}

void set_auto_start(const bool &enabled)
{
	HKEY key;

	LONG result = ::RegOpenKeyEx(HKEY_CURRENT_USER, REG_PATH_STARTUP, 0, KEY_WRITE, &key);
	if (result != ERROR_SUCCESS)
		return;

	if (enabled)
	{
		wchar_t filename[MAX_PATH];
		auto len = ::GetModuleFileName(nullptr, filename, sizeof(filename));
		::RegSetValueEx(key, REG_KEY_APP_NAME, 0, REG_SZ, (const BYTE *)filename, len * sizeof(wchar_t));
	}
	else
	{
		::RegDeleteValue(key, REG_KEY_APP_NAME);
	}

	::RegCloseKey(key);
}

constexpr const auto REG_KEY_PSR_FEATURE = L"DalPSRFeatureEnable";
constexpr const auto REG_PATH_PSR_FEATURE = L"SYSTEM\\CurrentControlSet\\Control\\Class\\{4d36e968-e325-11ce-bfc1-08002be10318}\\0000";

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

void set_psr_enabled(const bool &enabled)
{
	HKEY key;

	LONG result = ::RegOpenKeyEx(HKEY_LOCAL_MACHINE, REG_PATH_PSR_FEATURE, 0, KEY_WRITE, &key);
	if (result != ERROR_SUCCESS)
		return;

	auto value = enabled ? 1 : 0;
	::RegSetValueEx(key, REG_KEY_PSR_FEATURE, 0, REG_DWORD, reinterpret_cast<const BYTE *>(&value), sizeof(DWORD));

	::RegCloseKey(key);
}

} // namespace settings::app
