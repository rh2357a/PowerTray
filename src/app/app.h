#ifndef APP_APP_H
#define APP_APP_H

#include <windef.h>

namespace app {

void run();
void mainloop();

LRESULT CALLBACK wndproc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

void on_app_exit();

void on_menu_create();
void on_menu_update();
void on_menu_show();

void open_edit_profile();
void toggle_psr();
void toggle_auto_start();
void toggle_mpo();

} // namespace app

#endif
