///Microsoft Developer Studio generated resource script.
//
#include "resource.h"

#define APSTUDIO_READONLY_SYMBOLS
/////////////////////////////////////////////////////////////////////////////
//
// Generated from the TEXTINCLUDE 2 resource.
//
#define APSTUDIO_HIDDEN_SYMBOLS
#include <windows.h>
#undef APSTUDIO_HIDDEN_SYMBOLS
#include "resource.h"

/////////////////////////////////////////////////////////////////////////////
#undef APSTUDIO_READONLY_SYMBOLS

/////////////////////////////////////////////////////////////////////////////
// English (U.S.) resources

#if !defined(AFX_RESOURCE_DLL) || defined(AFX_TARG_ENU)
#ifdef _WIN32
LANGUAGE LANG_ENGLISH, SUBLANG_ENGLISH_US
#pragma code_page(1252)
#endif //_WIN32

#ifndef _MAC

/////////////////////////////////////////////////////////////////////////////
//
// Version
//

VER_ANOPE VERSIONINFO
 FILEVERSION @VERSION_COMMA@
 PRODUCTVERSION @VERSION_COMMA@
#ifndef MINGW
 FILEFLAGSMASK VS_FFI_FILEFLAGSMASK
#endif
#ifdef _DEBUG
 FILEFLAGS VS_FF_DEBUG
#else
 FILEFLAGS 0x0L
#endif
 FILEOS VOS_NT
 FILETYPE VFT_APP
 FILESUBTYPE 0x0L
BEGIN
    BLOCK "StringFileInfo"
    BEGIN
        BLOCK "040904b0"
        BEGIN
            VALUE "CompanyName", "Anope Team"
            VALUE "FileDescription", "Anope IRC Services"
            VALUE "FileVersion", "@VERSION_FULL@"
            VALUE "InternalName", "Anope"
            VALUE "LegalCopyright", "Copyright (C) 2003-2019 Anope Team"
            VALUE "OriginalFilename", "anope.exe"
            VALUE "ProductName", "Anope"
            VALUE "ProductVersion", "@VERSION_DOTTED@"
        END
    END
    BLOCK "VarFileInfo"
    BEGIN
        VALUE "Translation", 0x409, 1200
    END
END

#endif    // !_MAC

/////////////////////////////////////////////////////////////////////////////
//
// Icon
//

// Icon with lowest ID value placed first to ensure application icon
// remains consistent on all systems.
ICON_APP                ICON                    "@Anope_SOURCE_DIR@/src/win32/anope-icon.ico"
#endif    // English (U.S.) resources
/////////////////////////////////////////////////////////////////////////////



#ifndef APSTUDIO_INVOKED
/////////////////////////////////////////////////////////////////////////////
//
// Generated from the TEXTINCLUDE 3 resource.
//


/////////////////////////////////////////////////////////////////////////////
#endif    // not APSTUDIO_INVOKED
