#ifndef RESOURCES_H
#define RESOURCES_H

#define __R_STR_IMPL(x) #x
#define __R_STR(x)      __R_STR_IMPL(x)

#define APP_AUTHOR      "rh2357a"
#define APP_NAME        __R_STR(_APP_NAME)
#define APP_FILENAME    __R_STR(_APP_FILENAME)
#define APP_VERSION     "1.3.7.0"
#define APP_VERSION_RES 1, 3, 7, 0
#define APP_DESCRIPTION APP_NAME " v" APP_VERSION

#define IDI_APP 101

#endif
