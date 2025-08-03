#ifndef APP_ARGS_H
#define APP_ARGS_H

#include "api/power.h"

#include <windef.h>

#include <optional>

namespace app ::args {

void init();

std::optional<api::power::mode> get_mode();

bool has_open_edit_profile();
bool has_toggle_psr();
bool has_toggle_auto_start();

bool from_restart();

} // namespace app::args

#endif
