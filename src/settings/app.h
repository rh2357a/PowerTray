#ifndef SETTINGS_APP_H
#define SETTINGS_APP_H

namespace settings::app {

bool is_auto_start();
void set_auto_start(bool enabled);

bool is_psr_enabled();
void set_psr_enabled(bool enabled);

bool is_mpo_enabled();
void set_mpo_enabled(bool enabled);

} // namespace settings::app

#endif
