#ifndef MAIN_H
#define MAIN_H

#include <windef.h>

void win_main();

LRESULT CALLBACK wnd_proc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

void on_app_exit();
void on_menu_create();
void on_menu_update();
void on_menu_show();

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
	win_main();
	return 0;
}

#endif
