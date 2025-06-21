#ifndef POWERAPI_H
#define POWERAPI_H

#include <vector>
#include <initguid.h>

#include "utils.h"

struct power_profile_node
{
    GUID guid;
    std::wstring friendly_name;

    power_profile_node(const GUID &guid, const std::wstring &name) : guid(guid), friendly_name(name) {}
};

class power_api
{
public:
    static const GUID SCHEME_BEST_PERFORMANCE;
    static const GUID SCHEME_BETTER_PERFORMANCE;
    static const GUID SCHEME_BETTER_BATTERY;

private:
    dll_wrapper dll;

private:
    power_api() : dll("powrprof.dll") {}

private:
    static dll_wrapper &self()
    {
        static power_api instance;
        return instance.dll;
    }

public:
    static GUID get_overlay_scheme();
    static void set_overlay_scheme(GUID guid);

public:
    static std::vector<power_profile_node> get_power_profiles();
    static GUID *get_power_profile();
    static void set_power_profile(GUID guid);
};

#endif
