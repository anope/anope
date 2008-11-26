#include "version.h"

#ifdef _WIN32
# ifdef MODULE_COMPILE
#  define E extern __declspec(dllexport)
# else
#  define E extern __declspec(dllimport)
# endif
#else
# define E extern
#endif

extern "C"
{
	E int getAnopeBuildVersion();
	E int getAnopeMajorVersion();
	E int getAnopeMinorVersion();
	E int getAnopePatchVersion();

	int getAnopeBuildVersion()
	{
#if 0
		return VERSION_BUILD;
#endif
		return 0; // XXX
	}

	int getAnopeMajorVersion()
	{
		return VERSION_MAJOR;
	}

	int getAnopeMinorVersion()
	{
		return VERSION_MINOR;
	}

	int getAnopePatchVersion()
	{
		return VERSION_PATCH;
	}
}
