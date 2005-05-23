' Configuration for Anope
'
' (C) 2003-2005 Anope Team
' Contact us at info@anope.org
'
' This program is free but copyrighted software; see the file COPYING for
' details.
'
' Based on the original code of Epona by Lara.
' Based on the original code of Services by Andy Church.


' Required Variables
Dim StdIn
Dim UseMySQL, UseDBEnc, CompilerVer
Dim fso
Set fso = CreateObject("Scripting.FileSystemObject")

' Setup StdIn for use
Set StdIn = WScript.StdIn

' Display introductory header
WScript.Echo ""
WScript.Echo "   ___"
WScript.Echo "  / _ \  http://www.anope.org"
WScript.Echo " | /_\ | _ __  _ _  _ _   ___"
WScript.Echo " |  _  || '_ \/ _ \/ _ \ / _ \"
WScript.Echo " | | | || | |  |_|  |_| |  __/"
WScript.Echo " |_| |_||_| |_\___/|  _/ \___|"
WScript.Echo "                   | |"
WScript.Echo "                   |_| IRC Services"
WScript.Echo "                        v1.7.10-rc2"
WScript.Echo ""
WScript.Echo ""
WScript.Echo "This program will help you to compile your Services, and ask you"
WScript.Echo "questions regarding the compile-time settings of it during the"
WScript.Echo "process."
WScript.Echo ""
WScript.Echo "Anope is a set of Services for IRC networks that allows users to"
WScript.Echo "manage their nicks and channels in a secure and efficient way,"
WScript.Echo "and administrators to manage their network with powerful tools."
WScript.Echo ""
WScript.Echo "Do not forget to read all the documents located in docs/,"
WScript.Echo "especially the README and INSTALL files."
WScript.Echo ""
WScript.Echo "For all your Anope needs please visit our portal at"
WScript.Echo "http://www.anope.org/"
WScript.Echo ""
WScript.Echo "Press Enter to Continue..."
StdIn.ReadLine

' Enable MySQL Support?
Do While (UseMySQL <> "Y" AND UseMySQL <> "N" AND UseMySQL <> "YES" AND UseMySQL <> "NO")
        WScript.Echo "Would you like to compile Anope with MySQL Support?"
        WScript.Echo "(NOTE: You must have MySQL 3.23 or Later installed)"
        WScript.Echo ""
        WScript.Echo "Yes / No (Default)"
        UseMySQL = UCase(Trim(StdIn.ReadLine))
        If (UseMySQL = "") Then
                UseMySQL = "N"
        End If
        If (UseMySQL <> "Y" AND UseMySQL <> "N" AND UseMySQL <> "YES" AND UseMySQL <> "NO") Then
                WScript.Echo ""
                WScript.Echo "Invalid Selection!"
                WScript.Echo ""
        End If
Loop

WScript.Echo ""

If (UseMySQL = "Y" OR UseMySQL = "YES") Then
        If (fso.FileExists("C:\mysql\lib\opt\libmysql.lib") = False) Then
                WScript.Echo "ERROR: Cannot find LibMySQL.lib in c:\mysql\lib\opt!"
                WScript.Echo "MySQL Support Disabled.."
                UseMySQL = "0"
        ElseIf (fso.FileExists("C:\mysql\include\mysql.h") = False) Then
                WScript.Echo "ERROR: Cannot find mysql.h in c:\mysql\include!"
                WScript.Echo "MySQL Support Disabled.."
                UseMySQL = "0"
        Else
                WScript.Echo "All required files for MySQL Support have been located!"
                WScript.Echo "MySQL Support Enabled.."
                UseMySQL = "1"
        End If
Else
        WScript.Echo "MySQL Support Disabled.."
        UseMySQL = "0"
End If

WScript.Echo ""

' Enable Database Encryption Support?
Do While (UseDBEnc <> "Y" AND UseDBEnc <> "N" AND UseDBEnc <> "YES" AND UseDBEnc <> "NO")
        WScript.Echo "Would you like to enable Database Encryption?"
        WScript.Echo "(NOTE: If you enable encryption, you will NOT be able to recover"
        WScript.Echo "passwords at a later date. GETPASS and SENDPASS will also be useless)"
        WScript.Echo ""
        WScript.Echo "Yes / No (Default)"
        UseDBEnc = UCase(Trim(StdIn.ReadLine))
        If (UseDBEnc = "") Then
                UseDBEnc = "N"
        End If
        If (UseDBEnc <> "Y" AND UseDBEnc <> "N" AND UseDBEnc <> "YES" AND UseDBEnc <> "NO") Then
                WScript.Echo ""
                WScript.Echo "Invalid Selection!"
                WScript.Echo ""
        End If
Loop

WScript.Echo ""

If (UseDBEnc = "Y" OR UseDBEnc = "YES") Then
        WScript.Echo "Database Encryption Enabled.."
        UseDBEnc = "1"
Else
        WScript.Echo "Database Encryption Disabled.."
        UseDBEnc = "0"
End If

WScript.Echo ""
WScript.Echo "I will now check you have all the things I need..."

Dim libPath, libPath2
If (fso.FolderExists("C:\Program Files\Microsoft Visual Studio .NET 2003\VC7\Lib")) Then
        WScript.Echo "I found a copy of Microsoft Visual Studio .NET 2003.."
        libPath = "C:\Program Files\Microsoft Visual Studio .NET 2003\VC7\Lib"
        libPath2 = "C:\Program Files\Microsoft Visual Studio .NET 2003\Vc7\PlatformSDK\Lib"
        If (fso.FileExists(libPath & "/MSVCRT.lib") = False And fso.FileExists(libPath2 & "/MSVCRT.lib") = False) Then
                WScript.Echo "Hm. I can't seem to find the default library.. Are you sure this is installed properly?"
                libPath = ""
                libPath2 = ""
        ElseIf (fso.FileExists(libPath & "/wsock32.lib") = False And fso.FileExists(libPath2 & "/wsock32.lib") = False) Then
                WScript.Echo "I couldn't seem to find wsock32.lib.. We kind of need this.."
                libPath = ""
                libPath2 = ""
        ElseIf (fso.FileExists(libPath & "/advapi32.lib") = False And fso.FileExists(libPath2 & "/advapi32.lib") = False) Then
                WScript.Echo "I couldn't seem to find advapi32.lib.. We kind of need this.."
                libPath = ""
                libPath2 = ""
        ElseIf (fso.FileExists(libPath & "/uuid.lib") = False And fso.FileExists(libPath2 & "/uuid.lib") = False) Then
                WScript.Echo "I couldn't seem to find uuid.lib.. We kind of need this.."
                libPath = ""
                libPath2 = ""
        End If
End If

If (fso.FolderExists("C:\Program Files\Microsoft Visual Studio\VC98\Lib") And LibPath = "") Then
        WScript.Echo "I found a copy of Microsoft Visual Studio 6.. It's old, but we can use it.."
        libPath = "C:\Program Files\Microsoft Visual Studio\VC98\Lib"
        CompilerVer = "VC6"
        If (fso.FileExists(libPath & "/MSVCRT.lib") = False) Then
                WScript.Echo "Hm. I can't seem to find the default library.. Are you sure this is installed properly?"
                libPath = ""
        ElseIf (fso.FileExists(libPath & "/wsock32.lib") = False) Then
                WScript.Echo "I couldn't seem to find wsock32.lib.. We kind of need this.."
                libPath = ""
        ElseIf (fso.FileExists(libPath & "/advapi32.lib") = False) Then
                WScript.Echo "I couldn't seem to find advapi32.lib.. We kind of need this.."
                libPath = ""
        ElseIf (fso.FileExists(libPath & "/uuid.lib") = False) Then
                WScript.Echo "I couldn't seem to find uuid.lib.. We kind of need this.."
                libPath = ""
        End If
End If

If (fso.FolderExists("C:\Program Files\Microsoft Platform SDK\Lib") And LibPath = "") Then
        WScript.Echo "I found a copy of Microsoft Platform SDK.."
        libPath = "C:\Program Files\Microsoft Platform SDK\Lib"
        CompilerVer = "SDK"
        If (fso.FileExists(libPath & "/MSVCRT.lib") = False) Then
                WScript.Echo "Hm. I can't seem to find the default library.. Are you sure this is installed properly?"
                libPath = ""
        ElseIf (fso.FileExists(libPath & "/wsock32.lib") = False) Then
                WScript.Echo "I couldn't seem to find wsock32.lib.. We kind of need this.."
                libPath = ""
        ElseIf (fso.FileExists(libPath & "/advapi32.lib") = False) Then
                WScript.Echo "I couldn't seem to find advapi32.lib.. We kind of need this.."
                libPath = ""
        ElseIf (fso.FileExists(libPath & "/uuid.lib") = False) Then
                WScript.Echo "I couldn't seem to find uuid.lib.. We kind of need this.."
                libPath = ""
        End If
End If

If (fso.FileExists("C:\Program Files\Microsoft Visual Studio .NET 2003\VC7\Bin\nmake.exe") = False AND fso.FileExists("C:\Program Files\Microsoft Visual Studio\VC98\Bin\nmake.exe") = False AND fso.FileExists("C:\nmake.exe") = False AND fso.FileExists("C:\Program Files\Microsoft Platform SDK\Bin\nmake.exe") = False) Then
                WScript.Echo ""
                WScript.Echo "I couldn't seem to find a copy of nmake.exe on your system.."
                WScript.Echo ""
                WScript.Echo "I can continue without it for now, but you'll need it when you want to compile."
                WScript.Echo "I suggest downloading a copy from the URL below, and placing it in your C:\ drive."
                WScript.Echo ""
                WScript.Echo "http://download.microsoft.com/download/vc15/patch/1.52/w95/en-us/nmake15.exe"
                WScript.Echo ""
                WScript.Echo "You should place nmake.exe in c:\"
                WScript.Echo ""
End If
        
If (libPath = "") Then
        Dim tmpPath
        WScript.Echo "I couldn't find any of the paths I was looking for.."
        WScript.Echo "If you have installed the Visual C++ Libraries in a non-standard location, enter"
        WScript.Echo "this location below, and I will try and look there.."
        WScript.Echo "(NOTE: Do NOT enter a trailing slash)"
        WScript.Echo ""
        WScript.Echo "Path to Visual C++ Libraries: "
        tmpPath = Trim(StdIn.ReadLine)
        If (fso.FolderExists(tmpPath) AND fso.FileExists(tmpPath & "/MSVCRT.lib")) Then
                libPath = tmpPath
                If (fso.FileExists(libPath & "/wsock32.lib") = False) Then
                        WScript.Echo "I couldn't seem to find wsock32.lib.. We kind of need this.."
                        libPath = ""
                ElseIf (fso.FileExists(libPath & "/advapi32.lib") = False) Then
                        WScript.Echo "I couldn't seem to find advapi32.lib.. We kind of need this.."
                        libPath = ""
                ElseIf (fso.FileExists(libPath & "/uuid.lib") = False) Then
                        WScript.Echo "I couldn't seem to find uuid.lib.. We kind of need this.."
                        libPath = ""
                Else
                        WScript.Echo "Okay, I found what I was looking for.."
                End If
        Else
                WScript.Echo "I couldn't find the default library in that folder."
        End If
End If

If (libPath <> "") Then
                Dim f, f2, i, verMaj, verMin, verPatch, verBuild
                Const ForReading = 1, ForWriting = 2
                WScript.Echo "Looks like you've got all the libraries I need.."
                Set f2 = fso.OpenTextFile("version.log", ForReading)
                Do While (i < 7)
                        f2.SkipLine()
                        i = i + 1
                Loop
                verMaj = Replace(Replace(Trim(f2.ReadLine), "VERSION_MAJOR=" & chr(34), ""), chr(34), "")
                verMin = Replace(Replace(Trim(f2.ReadLine), "VERSION_MINOR=" & chr(34), ""), chr(34), "")
                verPatch = Replace(Replace(Trim(f2.ReadLine), "VERSION_PATCH=" & chr(34), ""), chr(34), "")
                verBuild = Replace(Replace(Trim(f2.ReadLine), "VERSION_BUILD=" & chr(34), ""), chr(34), "")
                verStr = Replace(verStr, chr(34), "")
                f2.close
                Set f = fso.OpenTextFile("Makefile.inc.win32", ForWriting)
                f.WriteLine("USE_MYSQL=" & UseMySQL)
                If (UseMySQL = "1") Then
                        f.WriteLine("MYSQL_LIB=" & chr(34) & "c:\\mysql\\lib\\opt" & chr(34))
                        f.WriteLine("MYSQL_INC="  & chr(34) & "c:\\mysql\\include" & chr(34))
                End If
                f.WriteLine("DB_ENCRYPTION=" & UseDBEnc)
                If (CompilerVer = "VC6") Then
                        f.WriteLine("VC6=/QIfist")
                End If
                if (libPath2 <> "") Then
                        libPath = libPath2 & chr(34) & " /LIBPATH:" & chr(34) & libPath
                End If
                f.WriteLine("VERSION=" & verMaj & "." & verMin & "." & verPatch & "." & verBuild)
                f.WriteLine("LIBPATH=" & libPath)
                f.WriteLine("PROGRAM=anope.exe")
                f.WriteLine("DATDEST=data")
                f.WriteLine("CC=cl")
                f.WriteLine("RC=rc")
                f.WriteLine("MAKE=nmake -f Makefile.win32")
                f.WriteLine("BASE_CFLAGS=/O2 /MD")
                f.WriteLine("LIBS=wsock32.lib advapi32.lib /NODEFAULTLIB:libcmtd.lib")
                f.WriteLine("LFLAGS=/LIBPATH:" & chr(34) & "$(LIBPATH)" & chr(34))
                If (UseMySQL = "1") Then
                        f.WriteLine("LIBS=$(LIBS) /LIBPATH:$(MYSQL_LIB)")
                        f.WriteLine("MYSQL_LIB_PATH=/LIBPATH:$(MYSQL_LIB)")
                        f.WriteLine("BASE_CFLAGS=$(BASE_CFLAGS) /I $(MYSQL_INC)")
                        f.WriteLine("MYSQL_INC_PATH=/I $(MYSQL_INC)")
                        f.WriteLine("RDB_C=rdb.c")
                        f.WriteLine("RDB_O=rdb.obj")
                        f.WriteLine("MYSQL_C=mysql.c")
                        f.WriteLine("MYSQL_O=mysql.obj")
                        f.WriteLine("BASE_CFLAGS=/D USE_MYSQL /D USE_RDB $(BASE_CFLAGS) /D HAVE_MYSQL_MYSQL_H")
                        f.WriteLine("MYPASQL_BUILD=$(CC) /LD $(MYSQL_INC_PATH) src\mypasql.c /link $(MYSQL_LIB_PATH) $(LFLAGS) \")
                        f.WriteLine("/DEF:src\mypasql.def libmysql.lib zlib.lib ws2_32.lib advapi32.lib /NODEFAULTLIB:LIBCMTD.lib")
                        f.WriteLine("LIBS=$(LIBS) libmysql.lib zlib.lib")
                End If
                If (UseDBEnc = "1") Then
                        f.WriteLine("BASE_CFLAGS=/D USE_ENCRYPTION /D ENCRYPT_MD5 $(BASE_CFLAGS)")
                End If
                f.WriteLine("MORE_CFLAGS = /I" & chr(34) & "../include" & chr(34))
                f.WriteLine("CFLAGS = /nologo $(VC6) $(CDEFS) $(BASE_CFLAGS) $(MORE_CFLAGS)")
                f.close()
                WScript.Echo "Configuration Complete!"
                WScript.Echo ""
                WScript.Echo "Type nmake -f Makefile.win32 to Compile Anope"
Else
                WScript.Echo ""
                WScript.Echo "Sorry, but you didn't have all the required libraries installed."
                WScript.Echo ""
                WScript.Echo "See http://windows.anope.org for a list of downloads needed to install Anope"
                WScript.Echo ""
End If

