#include "utils.h"

#include <windows.h>

#include <unordered_map>

std::unordered_map<std::string, HMODULE> libs;
std::unordered_map<std::string, std::unordered_map<std::string, FARPROC>> functions;

bool utils::dll::s_init = false;

FARPROC utils::dll::get_func_inst(const std::string &dll_name, const std::string &func_name)
{
	if (!libs.contains(dll_name))
	{
		auto wstr = strings::to_wstring(dll_name);
		libs[dll_name] = ::LoadLibraryW(wstr.c_str());
		functions[dll_name] = {};
	}

	if (!functions[dll_name].contains(func_name))
	{
		auto func = ::GetProcAddress(libs[dll_name], func_name.c_str());
		functions[dll_name][func_name] = func;
		return func;
	}

	return functions[dll_name][func_name];
}

void utils::dll::free()
{
	for (const auto &[name, lib] : libs)
		::FreeLibrary(lib);

	libs.clear();
	functions.clear();
}
