#ifndef CATSERV_DEFS_H
#define CATSERV_DEFS_H

#ifdef _WIN32
extern __declspec(dllexport) char *s_CatServ;
#else
E char *s_CatServ;
#endif

#endif

