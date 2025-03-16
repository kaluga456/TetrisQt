#ifndef ABOUT_H
#define ABOUT_H

#define APP_NAME "TetrisQt"
#ifdef _DEBUG
#define APP_BUILD_TYPE "debug"
#else
#define APP_BUILD_TYPE "release"
#endif
#define APP_BUILD_DATE __DATE__
#define APP_FULL_NAME APP_NAME " - " APP_BUILD_TYPE " build: " APP_BUILD_DATE
#define APP_COMPANY "kaluga456"
#define APP_URL "https://github.com/kaluga456/TetrisQt"

#endif // ABOUT_H
