#ifndef ABOUT_H
#define ABOUT_H

#define APP_NAME "Tetris"
#ifdef _DEBUG
#define APP_BUILD_TYPE "debug"
#else
#define APP_BUILD_TYPE "release"
#endif
#define APP_BUILD_DATE __DATE__
#define APP_FULL_NAME APP_NAME " - " APP_BUILD_TYPE " build: " APP_BUILD_DATE
#define APP_URL ""

#endif // ABOUT_H
