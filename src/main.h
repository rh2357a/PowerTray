#ifndef MAIN_H
#define MAIN_H

#include "utils.h"

#include <windef.h>

#include <string>
#include <vector>

void win_main(std::vector<std::string> args);

LRESULT CALLBACK wnd_proc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

void on_app_exit();
void on_menu_create();
void on_menu_update();
void on_menu_show();

void toggle_psr();

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR cmd_line, int)
{
	auto args = utils::strings::split(cmd_line, ' ');
	win_main(args);
	return 0;
}

#endif
