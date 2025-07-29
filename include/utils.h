#ifndef UTILS_H
#define UTILS_H

#include <minwindef.h>

#include <string>
#include <span>

namespace utils::strings {

std::string to_string(const std::wstring &wstr);
std::wstring to_wstring(const std::string &str);
std::wstring to_wstring(const UCHAR *c_str);

void string_copy(std::span<CHAR> dest, const std::string &str);
void wstring_copy(std::span<WCHAR> dest, const std::wstring &str);

} // namespace utils::strings

namespace utils {

class dll
{
private:
	static bool s_init;

public:
	template <typename _FuncType>
	static _FuncType get(const std::string &dll_name, const std::string &func_name)
	{
		if (!s_init)
		{
			atexit(free);
			s_init = true;
		}

		const auto &inst = get_func_inst(dll_name, func_name);
		return reinterpret_cast<_FuncType>(inst);
	}

private:
	static FARPROC get_func_inst(const std::string &dll_name, const std::string &func_name);
	static void free();
};

} // namespace utils

#endif
