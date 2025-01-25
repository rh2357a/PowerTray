#ifndef POWERAPI_H
#define POWERAPI_H

#include <vector>
#include <memory>
#include <string>

#include <powrprof.h>
#include <initguid.h>

struct PowerProfileNode
{
    GUID schemeGuid;
    std::wstring friendlyName;

    PowerProfileNode(const GUID &guid, const std::wstring &name) : schemeGuid(guid), friendlyName(name) {}
};

const GUID POWER_OVERLAY_SCHEME_BEST_PERFORMANCE = {0xded574b5, 0x45a0, 0x4f42, {0x87, 0x37, 0x46, 0x34, 0x5c, 0x09, 0xc2, 0x38}};
const GUID POWER_OVERLAY_SCHEME_BETTER_PERFORMANCE = {0x00000000, 0x0000, 0x0000, {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}};
const GUID POWER_OVERLAY_SCHEME_BETTER_BATTERY = {0x961cc777, 0x2547, 0x4f9d, {0x81, 0x74, 0x7d, 0x86, 0x18, 0x1b, 0x8a, 0x7a}};

void InitializePowerAPI();
void DestroyPowerAPI();
GUID PowerGetEffectiveOverlayScheme();
void PowerSetActiveOverlayScheme(GUID guid);
GUID *GetCurrentPowerProfile();
std::vector<PowerProfileNode> GetPowerProfiles();

#endif
