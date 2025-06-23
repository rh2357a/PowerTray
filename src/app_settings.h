#ifndef APP_SETTINGS_H
#define APP_SETTINGS_H

class app_settings
{
public:
    static bool is_startup_enabled();
    static void set_startup_enabled(bool enabled);

public:
    static bool is_psr_enabled();
    static void set_psr_enabled(bool enabled);
};

#endif
