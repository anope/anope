//
// install.js - Windows Configuration
//
// (C) 2003-2011 Anope Team
// Contact us at team@anope.org
//
// This program is free but copyrighted software; see the file COPYING for
// details.
//
// Based on the original code of Epona by Lara.
// Based on the original code of Services by Andy Church.
//
//

var anopeVersion = "Unknown";
var vMaj, vMin, vPat, vBuild, vExtra;
var drivesToCheck = ['C', 'D', 'E', 'F', 'G', 'H'];

var installerResponses = new Array();
var softwareVersions = {
                                                'Compiler' : false,
                                                'MySQLDB' : false
                                        };

var installerQuestions = [
                                                {
                                                        'question' : [
                                                                                'Do you want to compile Anope with MySQL Support?',
                                                                                'NOTE: You will need to have installed MySQL 3.23 or Above'
                                                                          ],
                                                        'short' : 'Enable MySQL Support?',
                                                        'options' : [
                                                                                'yes',
                                                                                'no'
                                                                        ],
                                                        'default_answer' : 'no',
                                                        'store_answer' : function (answer) {
                                                                                        if (answer == 'yes') {
                                                                                                if (!findMySQL()) {
                                                                                                        WScript.Echo("\nERROR: Cannot find MySQL - See error messages above for details.\n");
                                                                                                        return false;
                                                                                                }
                                                                                        }
                                                                                        installerResponses['MySQL DB Support'] = answer;
                                                                                        return true;
                                                                                   },
                                                        'commit_config' : function() {
                                                                                        if (installerResponses['MySQL DB Support'] == 'yes') {
                                                                                                f.WriteLine("USE_MYSQL=1");
                                                                                                f.WriteLine("MYSQL_LIB=\""+softwareVersions['MySQLDB'].installedDrive+":\\"+softwareVersions['MySQLDB'].libpaths[0]+"\"");
                                                                                                f.WriteLine("MYSQL_INC=\""+softwareVersions['MySQLDB'].installedDrive+":\\"+softwareVersions['MySQLDB'].incpaths[0]+"\"");
                                                                                                f.WriteLine("LIBS=$(LIBS) /LIBPATH:$(MYSQL_LIB)");
                                                                                                f.WriteLine("MYSQL_LIB_PATH=/LIBPATH:$(MYSQL_LIB)");
                                                                                                f.WriteLine("BASE_CFLAGS=$(BASE_CFLAGS) /I $(MYSQL_INC)");
                                                                                                f.WriteLine("MYSQL_INC_PATH=/I $(MYSQL_INC)");
                                                                                                f.WriteLine("RDB_C=rdb.c");
                                                                                                f.WriteLine("RDB_O=rdb.obj");
                                                                                                f.WriteLine("MYSQL_C=mysql.c");
                                                                                                f.WriteLine("MYSQL_O=mysql.obj");
                                                                                                f.WriteLine("BASE_CFLAGS=/D USE_MYSQL /D USE_RDB $(BASE_CFLAGS) /D HAVE_MYSQL_MYSQL_H");
                                                                                                f.WriteLine("MYPASQL_BUILD=$(CC) /LD $(MYSQL_INC_PATH) src\\mypasql.c /link $(MYSQL_LIB_PATH) $(LFLAGS) /DEF:src\mypasql.def libmysql.lib zlib.lib ws2_32.lib advapi32.lib /NODEFAULTLIB:LIBCMTD.lib");
                                                                                                f.WriteLine("LIBS=$(LIBS) libmysql.lib zlib.lib");                                                                             
                                                                                        }
                                                                                        else {
                                                                                                f.WriteLine("USE_MYSQL=0");
                                                                                        }
                                                        }                                                                                                                                        
                                                },
                                               
                                       ];

var buildPackages = [
                                        {
                                                        'name' : 'Microsoft Visual Studio 2010 (64bit)',
                                                        'libpaths' : [
                                                                     'Program Files (x86)\\Microsoft Visual Studio 10.0\\VC\\lib',
                                                                     'Program Files\\Microsoft Platform SDK for Windows Server 2003 R2\\Lib'
								     ],
                                                        'incpaths': [
                                                                     'Program Files (x86)\\Microsoft Visual Studio 10.0\\VC\\include',
                                                                     'Program Files\\Microsoft Platform SDK for Windows Server 2003 R2\\include'
                                                         ],
                                                         'nmake' : [
                                                                     'Program Files\\Microsoft Platform SDK for Windows Server 2003 R2\\Bin',
                                                         ],
							 'additional_switches' : [ '/w' ],
							 'installedDrive' : 'C'
                                        },
                                        {
                                                        'name' : 'Microsoft Visual Studio 2010',
                                                        'libpaths' : [
                                                                     'Program Files\\Microsoft Visual Studio 10.0\\VC\\lib',
                                                                     'Program Files\\Microsoft Platform SDK for Windows Server 2003 R2\\Lib'
                                                                     ],
                                                        'incpaths' : [
                                                                     'Program Files\\Microsoft Visual Studio 10.0\\VC\\include',
                                                                     'Program Files\\Microsoft Platform SDK for Windows Server 2003 R2\\Include'
                                                                     ],
                                                        'nmake' : [
                                                                     'Program Files\\Microsoft Platform SDK for Windows Server 2003 R2\\Bin',
                                                                  ],
                                                        'additional_switches' : [ '/w' ],
                                                        'installedDrive' : 'C'
                                        },
                                        {
                                                        'name' : 'Microsoft Visual Studio 2008',
                                                        'libpaths' : [                                                                               
                                                                     'Program Files\\Microsoft Visual Studio 9.0\\VC\\Lib',
                                                                     'Program Files\\Microsoft Platform SDK for Windows Server 2003 R2\\Lib'
                                                                     ],
                                                        'incpaths' : [                                                                               
                                                                     'Program Files\\Microsoft Visual Studio 9.0\\VC\\Include',
                                                                     'Program Files\\Microsoft Platform SDK for Windows Server 2003 R2\\Include'
                                                                     ],
                                                        'nmake' : [
														             'Program Files\\Microsoft Platform SDK for Windows Server 2003 R2\\Bin',
                                                                  ],
                                                        'additional_switches' : [ '/w' ],
                                                        'installedDrive' : 'C'
                                        },
                                        {
                                                        'name' : 'Microsoft Visual Studio 2008 (64bit)',
                                                        'libpaths' : [
                                                                     'Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\Lib',
                                                                     'Program Files\\Microsoft Platform SDK for Windows Server 2003 R2\\Lib'
                                                                     ],
                                                        'incpaths' : [
                                                                     'Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\Include',
                                                                     'Program Files\\Microsoft Platform SDK for Windows Server 2003 R2\\Include'
                                                                     ],
                                                        'nmake' : [
                                                                                                                             'Program Files\\Microsoft Platform SDK for Windows Server 2003 R2\\Bin',
                                                                  ],
                                                        'additional_switches' : [ '/w' ],
                                                        'installedDrive' : 'C'
                                        },
 
                                        {
                                                        'name' : 'Microsoft Visual Studio 2005',
                                                        'libpaths' : [
                                                                                'Program Files\\Microsoft Visual Studio 8\\VC\\Lib',
                                                                                'Program Files\\Microsoft Platform SDK for Windows Server 2003 R2\\Lib'
                                                                         ],
                                                        'incpaths' : [
                                                                                'Program Files\\Microsoft Visual Studio 8\\VC\\Include',
                                                                                'Program Files\\Microsoft Platform SDK for Windows Server 2003 R2\\Include'
                                                                         ],
                                                        'nmake' : [
                                                                                'Program Files\\Microsoft Platform SDK for Windows Server 2003 R2\\Bin',
                                                                                ''
                                                                        ],
                                                        'additional_switches' : [
                                                                                                '/w'
                                                                      	],
							'installedDrive' : 'C'
                                        },
                                        {                                        
                                                        'name' : 'Microsoft Visual Studio 2005 (Old PSDK)',
                                                        'libpaths' : [
                                                                                'Program Files\\Microsoft Visual Studio 8\\VC\\Lib',
                                                                                'Program Files\\Microsoft Platform SDK\\Lib'
                                                                         ],
                                                        'incpaths' : [
                                                                                'Program Files\\Microsoft Visual Studio 8\\VC\\Include',
                                                                                'Program Files\\Microsoft Platform SDK\\Include'
                                                                         ],
                                                        'nmake' : [
                                                                                'Program Files\\Microsoft Platform SDK\\Bin',
                                                                                ''
                                                                        ],
                                                        'additional_switches' : [
                                                                                                '/w'
                                                               		],
							'installedDrive' : 'C'
                                        },
                                        
                                        {
                                                        'name' : 'Microsoft Visual Studio .NET 2003',
                                                        'libpaths' : [
                                                                                'Program Files\\Microsoft Visual Studio .NET 2003\\VC7\\Lib',
                                                                                'Program Files\\Microsoft Visual Studio .NET 2003\\PlatformSDK\\Lib'
                                                                         ],
                                                        'incpaths' : [
                                                                                'Program Files\\Microsoft Visual Studio .NET 2003\\VC7\\Include',
                                                                                'Program Files\\Microsoft Visual Studio .NET 2003\\PlatformSDK\\Include'
                                                                         ],
                                                        'nmake' : [
                                                                                'Program Files\\Microsoft Visual Studio .NET 2003\\VC7\\Bin',
                                                                                ''
                                                                        ],                                                                    
                                                        'additional_switches' : false,
							'installedDrive' : 'C'
                                        }
                                ];                  
                                                              
                                
        var mysqlVersions = [
						{
							'name' : 'MySQL 5.1',
							'libpaths' : [
									  'Program Files\\MySQL\\MySQL Server 5.1\\Lib\\opt'
								],
							'incpaths' : [
									  'Program Files\\MySQL\\MySQL Server 5.1\\Include'
								],
							'dllfile' : 'Program Files\\MySQL\\MySQL Server 5.1\\bin\\libmysql.dll',
							'installedDrive' : 'C'
						},

                                                {
                                                        'name' : 'MySQL 5.0',
                                                        'libpaths' : [
                                                                                'Program Files\\MySQL\\MySQL Server 5.0\\Lib\\opt'
                                                                        ],
                                                        'incpaths' : [
                                                                                'Program Files\\MySQL\\MySQL Server 5.0\\Include'
                                                                        ],
                                                        'dllfile' : 'Program Files\\MySQL\\MySQL Server 5.0\\Bin\\libmysql.dll', 
							'installedDrive' : 'C'
                                                },
                                                
                                                {
                                                        'name' : 'MySQL 4.1',
                                                        'libpaths' : [
                                                                                'Program Files\\MySQL\\MySQL Server 4.1\\Lib\\opt'
                                                                        ],
                                                        'incpaths' : [
                                                                                'Program Files\\MySQL\\MySQL Server 4.1\\Include'
                                                                        ],
                                                        'dllfile' : 'Program Files\\MySQL\\MySQL Server 4.1\\Bin\\libmysql.dll', 
                                                	'installedDrive' : 'C'
						},        
                                                
                                                {
                                                        'name' : 'MySQL 4.0',
                                                        'libpaths' : [
                                                                                'Program Files\\MySQL\\MySQL Server 4.0\\Lib\\opt'
                                                                        ],
                                                        'incpaths' : [
                                                                                'Program Files\\MySQL\\MySQL Server 4.0\\Include'
                                                                        ],
                                                        'dllfile' : 'Program Files\\MySQL\\MySQL Server 4.0\\Bin\\libmysql.dll',
							'installedDrive' : 'C'
                                                },                                                                                                                                                                           
                                                        
                                                {
                                                        'name' : 'MySQL 3.23 or older (or other default path)',
                                                        'libpaths' : [
                                                                                'mysql\\lib\\opt'
                                                                        ],
                                                        'incpaths' : [
                                                                                'mysql\\include'
                                                                        ],
                                                        'dllfile' : 'mysql\\Bin\\libmysql.dll',
							'installedDrive' : 'C'                               
                                                }

                                ];                  
                                
        var bannerReplacements = [
                                                                {
                                                                        'findtext' : /CURVER/g,
                                                                        'replacement' : function() { FindAnopeVersion(); return anopeVersion; }
                                                                 },
                                                                 {
                                                                        'findtext' : / For more options type .\/Config --help/g,
                                                                        'replacement' : function() { return ''; }
                                                                 }
                                                ];                                                                       
                                                                        
                                                                        
        var fso = WScript.CreateObject("Scripting.FileSystemObject"); 
        var x, y, z;
        
        if (fso.FileExists('.BANNER')) {
                var bannerStream = fso.OpenTextFile(".BANNER");
                var bannerText = bannerStream.ReadAll();              
                bannerStream.close();
                
                for (x in bannerReplacements) {
                        var thisReplacement = bannerReplacements[x];
                        bannerText = bannerText.replace(thisReplacement['findtext'], thisReplacement['replacement']);
                }
                
                WScript.Echo(bannerText+"\n");
        }    
        else {
                WScript.Echo("ERROR: Cannot find banner file!\n");
        }
        
        WScript.Echo("Press Enter to Begin...");
        InstallerInput();        
        WScript.Echo("");
      
        for (x in installerQuestions) {
                var thisQuestion = installerQuestions[x];
                var validResponse = false;
                var validOpts = new Array();
                while (!validResponse) {
                        for (y in thisQuestion.question) {
                                var qLine = thisQuestion.question[y];
                                WScript.Echo(qLine);
                        }
                        WScript.Echo('');
                        var choiceLine = '';
                        for (y in thisQuestion.options) {
                                choiceLine += thisQuestion.options[y] + ', ';
                                validOpts[thisQuestion.options[y]] = true;
                        }
                        choiceLine = choiceLine.substring(0, choiceLine.length - 2);
                        WScript.Echo("Available Options: "+choiceLine);
                        WScript.Echo("Default Answer: "+thisQuestion.default_answer+"\n");
                        WScript.Echo(thisQuestion.short);
                        var inputValue = InstallerInput().toLowerCase();
                        if (!inputValue) {
                                inputValue = thisQuestion.default_answer;
                        }
                        if (!validOpts[inputValue]) {
                                WScript.Echo("ERROR: Invalid option '"+inputValue+"'\n");
                        }
                        else if (thisQuestion.store_answer(inputValue)) {
                                validResponse = true;
                        }
                }                
                WScript.Echo("");
        }
        
        if (!findCompiler()) {
                WScript.Echo("\nERROR: No suitable build tools were found!");
                WScript.Echo("Please ensure you have downloaded and installed a version of Visual C++ and/or PlatformSDK.\n");
                WScript.Echo("For more information on the tools needed to build Anope on Windows, see:\nhttp://wiki.anope.org/index.php/Windows:1.8#Compiling\n");
        }
        else {
                WScript.Echo("\nBuild tools were found successfully!\n");
                WScript.Echo("\nAnope will be compiled with the following options:\n");
                for (x in installerResponses) {
                        var thisResponse = installerResponses[x];
                        WScript.Echo("\t"+x+":\t\t["+thisResponse.toUpperCase()+"]");
                }        
                for (x in softwareVersions) {
                        var thisVer = softwareVersions[x];
                        if (!thisVer) {
                                WScript.Echo("\t"+x+" Version:\t\tNot Enabled");
                        }
                        else {
                                WScript.Echo("\t"+x+" Version:\t\t"+thisVer.name);
                        }        
                }
		    WScript.Echo("\tAnope Version:\t\t\t"+anopeVersion);
                WScript.Echo("\nTo continue, please press Enter...");
                InstallerInput();    
       
                var f = fso.OpenTextFile("Makefile.inc.win32", 2);
                f.WriteLine("#");
                f.WriteLine("# Generated by install.js");
                f.WriteLine("#");

                if (typeof(softwareVersions['Compiler'].additional_switches) !== 'boolean') {
                        var switch_line = '';
                        for (x in softwareVersions['Compiler'].additional_switches) {
                                switch_line += softwareVersions['Compiler'].additional_switches[x]+" ";
                        }
                        f.WriteLine("VC6="+switch_line);
                }
                var path_line = '';
                for (x in softwareVersions['Compiler'].libpaths) {
                        path_line += "/LIBPATH:\""+softwareVersions['Compiler'].installedDrive+":\\"+softwareVersions['Compiler'].libpaths[x]+"\" ";
                }
                f.WriteLine("LIBPATH="+path_line);
                path_line = '';
                var path_line_rc = '';
                for (x in softwareVersions['Compiler'].incpaths) {
                        path_line += "/I \""+softwareVersions['Compiler'].installedDrive+":\\"+softwareVersions['Compiler'].incpaths[x]+"\" ";
                        path_line_rc += "/i \""+softwareVersions['Compiler'].installedDrive+":\\"+softwareVersions['Compiler'].incpaths[x]+"\" ";
                }
                f.WriteLine("INCFLAGS="+path_line);
                f.WriteLine("VERSION="+anopeVersion);
                f.WriteLine("PROGRAM=anope.exe");
                f.WriteLine("DATDEST=data");
                f.WriteLine("CC=cl");
                f.WriteLine("RC=rc");
                f.WriteLine("MAKE=nmake -f Makefile.win32");
                f.WriteLine("BASE_CFLAGS=$(VC6) /O2 /MD $(INCFLAGS)");
                f.WriteLine("RC_FLAGS="+path_line_rc);
                f.WriteLine("LIBS=wsock32.lib advapi32.lib /NODEFAULTLIB:libcmtd.lib");
                f.WriteLine("LFLAGS=$(LIBPATH)");               
                   
                for (x in installerQuestions) {
                        var thisQuestion = installerQuestions[x];
                        thisQuestion.commit_config();
                } 
                        
                f.WriteLine("MORE_CFLAGS = /I\"../include\"");
                f.WriteLine("CFLAGS = /nologo $(CDEFS) $(BASE_CFLAGS) $(MORE_CFLAGS)");
                f.close();
                
                generateRC();
                
                WScript.Echo("\nConfiguration Complete!");
                WScript.Echo("-----------------------\n");
                WScript.Echo("Anope has been configured to your system. To compile, simply type:");
                WScript.Echo("nmake -f Makefile.win32\n");
		    WScript.Echo("If you update Anope, you should run this script again to ensure\nall available options are set.\n");
          
        }                        
        // Fin.
        
        // -----------------------------------------------------------------
        
        // Functions
        
        function FindAnopeVersion() {
                if (!fso.FileExists('version.log')) {
                        anopeVersion = 'Unknown';
                        return;
                }                               
                
                var versionLog = fso.OpenTextFile("version.log");
                while (!versionLog.atEndOfStream) {
                        var versionLine = versionLog.readline();
                        var thisMatch = versionLine.replace('\n', '');
			while (thisMatch.match(/\"/g)) {
				thisMatch = thisMatch.replace('"', '');	
			}
			versionLine = thisMatch;
                        if (versionLine.match(/VERSION_MAJOR=/g)) {
                                vMaj = versionLine.replace('VERSION_MAJOR=', '');
                                continue;
                        }
                        if (versionLine.match(/VERSION_MINOR=/g)) {
                                vMin = versionLine.replace('VERSION_MINOR=', '');
                                continue;
                        } 
                        if (versionLine.match(/VERSION_PATCH=/g)) {
                                vPat = versionLine.replace('VERSION_PATCH=', '');
                                continue;
                        }       
                        if (versionLine.match(/VERSION_EXTRA=/g)) {
                                vExtra = versionLine.replace('VERSION_EXTRA=', '');
                                continue;
                        }                
                        if (versionLine.match(/VERSION_BUILD=/g)) {
                                vBuild = versionLine.replace('VERSION_BUILD=', '');
                                continue;
                        }                                                                                                
                }
                versionLog.close();
                anopeVersion = vMaj+"."+vMin+"."+vPat+"."+vBuild+vExtra;
                return;
        }
        
        function InstallerInput() {
                var input = WScript.StdIn.Readline();
                return input;          
        }
        
        function findMySQL() {
                WScript.Echo("\nLooking for MySQL...\n");
		var installedDrive = "";
                for (x in mysqlVersions) {
                        var thisSQLVer = mysqlVersions[x];
                        WScript.Echo("Looking for: "+thisSQLVer.name+"...");
                        if (!(installedDrive = findFile("libmysql.lib", thisSQLVer.libpaths))) {
                                WScript.Echo("ERROR: Cannot find libmysql.lib - This version is probably not installed...\n");
                                continue;
                        }
                        if (!findFile("mysql.h", thisSQLVer.incpaths)) {
                                WScript.Echo("ERROR: Cannot find mysql.h - Half of this version of MySQL is installed (strange)...\n");
                                continue;
                        }
                        WScript.Echo("SUCCESS: "+thisSQLVer.name+" is installed, and is complete!\n");
			thisSQLVer.installedDrive = installedDrive;
                        softwareVersions.MySQLDB = thisSQLVer;
                        return true;
                }
                return false;
        }
        
        function findCompiler() {
                WScript.Echo("\nLooking for a suitable compiler...\n");
                var noPSDK = false;
		var installedDrive = "";                
                for (x in buildPackages) {
                        var thisPack = buildPackages[x];
                        WScript.Echo("Looking for: "+thisPack.name+"...");
                        if (!(installedDrive = findFile("MSVCRT.lib", thisPack.libpaths))) {
                                WScript.Echo("ERROR: Cannot find MSVCRT.lib - This version is probably not installed...\n");
                                continue;
                        }
                        if (!findFile("wsock32.lib", thisPack.libpaths)) {
                                WScript.Echo("ERROR: Cannot find wsock32.lib - Probably missing PlatformSDK...\n");
                                noPSDK = true;
                                continue;
                        }
                        if (!findFile("advapi32.lib", thisPack.libpaths)) {
                                WScript.Echo("ERROR: Cannot find advapi32.lib - Probably missing PlatformSDK...\n");
                                noPSDK = true;                                
                                continue;
                        }
                        if (!findFile("stdio.h", thisPack.incpaths)) {
                                WScript.Echo("ERROR: Cannot find stdio.h - Missing core header files...\n");
                                continue;
                        }
                        if (!findFile("windows.h", thisPack.incpaths)) {
                                WScript.Echo("ERROR: Cannot find windows.h - Probably missing PlatformSDK headers...\n");
                                noPSDK = true;                                
                                continue;
                        }
                        if (!findFile("nmake.exe", thisPack.nmake)) {
                                WScript.Echo("ERROR: Cannot find a copy of nmake.exe...\n");
                                WScript.Echo("In order to compile Anope, you need a working copy of nmake.exe on your system.");
                                WScript.Echo("A freely available copy can be downloaded from the url below.");
                                WScript.Echo("nmake.exe is also available in the PlatformSDK which can be freely downloaded from Microsoft.\n");
                                WScript.Echo("nmake.exe:\nhttp://download.microsoft.com/download/vc15/patch/1.52/w95/en-us/nmake15.exe\n");
                                break;
                        }                                
                        WScript.Echo("SUCCESS: "+thisPack.name+" was found, and is complete!");
			thisPack.installedDrive = installedDrive;
                        softwareVersions.Compiler = thisPack;
                        return true;
                }
                if (noPSDK) {
                        WScript.Echo("Some of the build tools were detected on your computer, but the essential PlatformSDK components were missing.");
                        WScript.Echo("You will need to download the PlatformSDK from the URL below, ensuring that the Core Windows files, and Debugging Tools are installed.");
                        WScript.Echo("For more details on installing the PlatformSDK, visit http://wiki.anope.org/index.php/Windows:1.8#Compiling\n");
                }
                return false;
        }
        
        function findFile(fileName, arrayOfPaths) {
                for (z in arrayOfPaths) {
                        var thisPath = arrayOfPaths[z];
			for (y in drivesToCheck) {
	                	var thisDrive = drivesToCheck[y];
			        if (fso.FileExists(thisDrive+":\\"+thisPath+"\\"+fileName)) {
        	                        return thisDrive;
                	        }
                	}
		}
                return false;                                
        }
        
        function generateRC() {
                var version_matches = [
                                                                {
                                                                        'find' : /VERSION_COMMA/g,
                                                                        'replacement' : vMaj+","+vMin+","+vPat+","+vBuild
                                                                },
                                                                
                                                                {
                                                                        'find' : /VERSION_FULL/g,
                                                                        'replacement' : anopeVersion
                                                                },
                                                                
                                                                {
                                                                        'find' : /VERSION_DOTTED/g,
                                                                        'replacement' : vMaj+"."+vMin+"."+vPat+"."+vBuild
                                                                }
                                                        ];
                                                        
                var template = fso.OpenTextFile("src/win32.rc.template", 1);
                var output = fso.OpenTextFile("src/win32.rc", 2, true);
                if (!template) {
                        WScript.Echo("ERROR: Unable to generate win32.rc file - Couldn't open source file..");
                }
                if (!output) {
                        WScript.Echo("ERROR: Unable to generate win32.rc file - Couldn't open output file..");
                }
                var templateText = template.ReadAll();
                template.close();
                
                for (x in version_matches) {
                        var thisVerStr = version_matches[x];
                        while (templateText.match(thisVerStr.find)) {
                                templateText = templateText.replace(thisVerStr.find, thisVerStr.replacement);
                        }
                }
                
                output.WriteLine(templateText);
                output.close();
        }
                                                                                                                                                                
