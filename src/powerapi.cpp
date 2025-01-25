#include <cstdint>
#include <memory>
#include <string>

#include <windows.h>

#include "powerapi.h"
#include "utils.h"

typedef DWORD(WINAPI *PowerSetActiveOverlayScheme_t)(GUID *pSchemeGuid);
typedef DWORD(WINAPI *PowerGetEffectiveOverlayScheme_t)(GUID *pSchemeGuid);

HMODULE hLib = nullptr;
PowerSetActiveOverlayScheme_t fnPowerSetActiveOverlayScheme = nullptr;
PowerGetEffectiveOverlayScheme_t fnPowerGetEffectiveOverlayScheme = nullptr;

void InitializePowerAPI()
{
    hLib = LoadLibrary(L"powrprof.dll");
    fnPowerGetEffectiveOverlayScheme = (PowerGetEffectiveOverlayScheme_t)GetProcAddress(hLib, "PowerGetEffectiveOverlayScheme");
    fnPowerSetActiveOverlayScheme = (PowerSetActiveOverlayScheme_t)GetProcAddress(hLib, "PowerSetActiveOverlayScheme");
}

void DestroyPowerAPI()
{
    if (hLib != nullptr)
        ::FreeLibrary(hLib);
    fnPowerGetEffectiveOverlayScheme = nullptr;
    fnPowerSetActiveOverlayScheme = nullptr;
}

GUID PowerGetEffectiveOverlayScheme()
{
    GUID guid;
    fnPowerGetEffectiveOverlayScheme(&guid);
    return guid;
}

void PowerSetActiveOverlayScheme(GUID guid)
{
    fnPowerSetActiveOverlayScheme(&guid);
}

GUID *GetCurrentPowerProfile()
{
    GUID *activeScheme;
    ::PowerGetActiveScheme(nullptr, &activeScheme);
    return activeScheme;
}

std::vector<PowerProfileNode> GetPowerProfiles()
{
    std::vector<PowerProfileNode> nodes;

    int index = 0;

    while (true)
    {
        DWORD guidBufferSize = 16;
        UCHAR guidBuffer[guidBufferSize];

        DWORD result = ::PowerEnumerate(nullptr, nullptr, nullptr, ACCESS_SCHEME, index, guidBuffer, &guidBufferSize);
        if (result == ERROR_SUCCESS)
        {
            GUID schemeGuid;
            memcpy(&schemeGuid, guidBuffer, sizeof(GUID));

            DWORD length = 0;
            ::PowerReadFriendlyName(nullptr, &schemeGuid, nullptr, nullptr, nullptr, &length);

            UCHAR nameBuffer[length];
            ::PowerReadFriendlyName(nullptr, &schemeGuid, nullptr, nullptr, nameBuffer, &length);

            std::wstring name;
            name.assign((WCHAR *)nameBuffer);
            nodes.push_back(PowerProfileNode(schemeGuid, name));
            index++;
            continue;
        }

        break;
    }

    return nodes;
}
