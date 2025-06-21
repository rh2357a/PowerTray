#include <windows.h>
#include <shlobj.h>

#include "utils.h"

bool windows::is_administrator_enabled()
{
    return ::IsUserAnAdmin();
}

void windows::exec(const std::string &cmd)
{
    STARTUPINFOA si = {};
    si.cb = sizeof(STARTUPINFOA);
    PROCESS_INFORMATION pi;

    if (::CreateProcessA(nullptr, (LPSTR)cmd.c_str(), nullptr, nullptr, false, 0, nullptr, nullptr, &si, &pi))
    {
        ::CloseHandle(pi.hProcess);
        ::CloseHandle(pi.hThread);
    }
}

std::string strings::to_string(const std::wstring &wstr)
{
    int size = ::WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, NULL, 0, NULL, NULL);
    std::string str(size, 0);
    ::WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, &str[0], size, NULL, NULL);
    return str;
}

std::wstring strings::to_wstring(const std::string &str)
{
    int size = ::MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, NULL, 0);
    std::wstring wstr(size, 0);
    ::MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, &wstr[0], size);
    return wstr;
}

std::wstring strings::to_wstring(const UCHAR *c_str)
{
    std::wstring name;
    name.assign((WCHAR *)c_str);
    return name;
}

dll_wrapper::dll_wrapper(const std::string &lib_name)
{
    auto wstr = strings::to_wstring(lib_name);
    lib = ::LoadLibraryW(wstr.c_str());
}

dll_wrapper::~dll_wrapper()
{
    ::FreeLibrary(lib);
}

FARPROC dll_wrapper::get_function_impl(const std::string &name)
{
    if (!lib)
        return nullptr;

    auto it = functions.find(name);
    if (it != functions.end())
        return it->second;

    FARPROC proc = ::GetProcAddress(lib, name.c_str());
    if (proc)
        functions[name] = proc;

    return proc;
}
