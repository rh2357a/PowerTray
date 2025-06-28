#include <windows.h>
#include <initguid.h>
#include <powrprof.h>

#include "powerapi.h"
#include "utils.h"

const GUID power_api::SCHEME_BEST_PERFORMANCE = {
    0xded574b5,
    0x45a0,
    0x4f42,
    {0x87, 0x37, 0x46, 0x34, 0x5c, 0x09, 0xc2, 0x38},
};

const GUID power_api::SCHEME_BETTER_PERFORMANCE = {
    0x00000000,
    0x0000,
    0x0000,
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
};

const GUID power_api::SCHEME_BETTER_BATTERY = {
    0x961cc777,
    0x2547,
    0x4f9d,
    {0x81, 0x74, 0x7d, 0x86, 0x18, 0x1b, 0x8a, 0x7a},
};

GUID power_api::get_overlay_scheme()
{
    using func = DWORD(WINAPI *)(GUID *);
    auto func_inst = get_dll().get_function<func>("PowerGetEffectiveOverlayScheme");

    GUID guid;
    func_inst(&guid);

    return guid;
}

void power_api::set_overlay_scheme(GUID guid)
{
    using func = DWORD(WINAPI *)(GUID *);
    auto func_inst = get_dll().get_function<func>("PowerSetActiveOverlayScheme");

    func_inst(&guid);
}

std::vector<power_profile_node> power_api::get_power_profiles()
{
    std::vector<power_profile_node> nodes;

    while (true)
    {
        DWORD guid_buffer_size = 16;
        UCHAR guid_buffer[guid_buffer_size];

        DWORD result = ::PowerEnumerate(nullptr, nullptr, nullptr, ACCESS_SCHEME, nodes.size(), guid_buffer, &guid_buffer_size);
        if (result == ERROR_SUCCESS)
        {
            GUID guid;
            memcpy(&guid, guid_buffer, sizeof(GUID));

            DWORD length = 0;
            ::PowerReadFriendlyName(nullptr, &guid, nullptr, nullptr, nullptr, &length);

            UCHAR friendly_name_buffer[length];
            ::PowerReadFriendlyName(nullptr, &guid, nullptr, nullptr, friendly_name_buffer, &length);

            power_profile_node new_node{
                guid,
                strings::to_wstring(friendly_name_buffer),
            };

            nodes.push_back(new_node);

            continue;
        }

        break;
    }

    return nodes;
}

GUID power_api::get_power_profile()
{
    GUID *scheme, profile_guid;

    auto result = ::PowerGetActiveScheme(nullptr, &scheme);
    if (scheme != nullptr)
    {
        profile_guid = *scheme;
        ::LocalFree(scheme);
    }

    return profile_guid;
}

void power_api::set_power_profile(GUID guid)
{
    ::PowerSetActiveScheme(nullptr, &guid);
}
