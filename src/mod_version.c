#include "version.h"

#ifndef _WIN32
#define E extern
#define I extern
#else
#ifndef MODULE_COMPILE
#define E extern __declspec(dllexport)
#define I extern __declspec(dllimport)
#else
#define E extern __declspec(dllimport)
#define I extern __declspec(dllexport)
#endif
#endif

E int getAnopeBuildVersion();
E int getAnopeMajorVersion();
E int getAnopeMinorVersion();
E int getAnopePatchVersion();

int getAnopeBuildVersion() {
    return VERSION_BUILD;
}

int getAnopeMajorVersion() {
    return VERSION_MAJOR;
}

int getAnopeMinorVersion() { 
    return VERSION_MINOR;
}

int getAnopePatchVersion() {
    return VERSION_PATCH;
}

