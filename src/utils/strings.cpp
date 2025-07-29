#include "utils.h"

#include <windows.h>

std::string utils::strings::to_string(const std::wstring &wstr)
{
	int size = ::WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, NULL, 0, NULL, NULL);
	std::string str(size, 0);
	::WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, &str[0], size, NULL, NULL);
	return str;
}

std::wstring utils::strings::to_wstring(const std::string &str)
{
	int size = ::MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, NULL, 0);
	std::wstring wstr(size, 0);
	::MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, &wstr[0], size);
	return wstr;
}

std::wstring utils::strings::to_wstring(const UCHAR *c_str)
{
	std::wstring name;
	name.assign((WCHAR *)c_str);
	return name;
}

void utils::strings::string_copy(std::span<CHAR> dest, const std::string &str)
{
	const auto c_str = str.c_str();
	lstrcpynA(dest.data(), c_str, dest.size());
}

void utils::strings::wstring_copy(std::span<WCHAR> dest, const std::wstring &str)
{
	const auto c_str = str.c_str();
	lstrcpynW(dest.data(), c_str, dest.size());
}
