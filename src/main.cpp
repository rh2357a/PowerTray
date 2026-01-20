#include "app/app.h"
#include "app/args.h"

#include <windows.h>

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
    app::args::init();
    app::run();
    return 0;
}
