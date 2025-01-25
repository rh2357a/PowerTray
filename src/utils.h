#ifndef UTILS_H
#define UTILS_H

#include <string>
#include <stringapiset.h>

static std::string WStringToString(const std::wstring &wstr)
{
    int bufferSize = ::WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, NULL, 0, NULL, NULL);
    std::string str(bufferSize, 0);
    ::WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, &str[0], bufferSize, NULL, NULL);
    return str;
}

static std::wstring StringToWString(const std::string &str)
{
    int bufferSize = ::MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, NULL, 0);
    std::wstring wstr(bufferSize, 0);
    ::MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, &wstr[0], bufferSize);
    return wstr;
}

#endif
