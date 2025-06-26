#ifndef UTILS_H
#define UTILS_H

#include <string>
#include <unordered_map>

#include <minwindef.h>

class windows
{
  public:
    static bool is_administrator_enabled();
    static void exec(const std::string &cmd);
};

class strings
{
  public:
    static std::string to_string(const std::wstring &wstr);
    static std::wstring to_wstring(const std::string &str);
    static std::wstring to_wstring(const UCHAR *c_str);
};

class dll_wrapper
{
  private:
    HMODULE lib;
    std::unordered_map<std::string, FARPROC> functions;

  public:
    dll_wrapper(const std::string &lib_name);
    ~dll_wrapper();

  public:
    template <typename T>
    T get_function(const std::string &name)
    {
        return reinterpret_cast<T>(get_function_impl(name));
    }

  private:
    FARPROC get_function_impl(const std::string &name);
};

#endif
