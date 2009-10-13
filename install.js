//
// install.js - Windows Configuration
//
// (C) 2003-2009 Anope Team
// Contact us at team@anope.org
//
// This program is free but copyrighted software; see the file COPYING for
// details.
//
// Based on the original code of Epona by Lara.
// Based on the original code of Services by Andy Church.
//
// $Id$
//

var anopeVersion = "Unknown";
var vMaj, vMin, vPat, vBuild, vExtra;

var installerResponses = new Array();

var installerQuestions = [
	{
		'question' : [
			'In what directory do you want Anope to be installed?'
		],
		'short' : 'Install directory:',
		'default_answer' : '',
		'store_answer' : function(answer) {
			if (!answer)
			{
				WScript.Echo("You must give a directory!\n");
				return false;
			}
			if (!fso.FolderExists(answer))
			{
				if (fso.FileExists(answer))
				{
					WScript.Echo(answer + " exists, but is not a directory!\n");
					return false;
				}
				WScript.Echo(answer + " does not exist.  Create it ([yes]/no)?\n");
				var inputValue = InstallerInput().toLowerCase();
				if (!inputValue)
					inputValue = 'yes';
				if (inputValue != 'no')
					fso.CreateFolder(answer);
			}
			else if (fso.FileExists(answer + '\\include\\services.h'))
			{
				WScript.Echo("You cannot use the Anope source directory as a target directory.\n");
				return false;
			}
			installerResponses['Install Directory'] = answer.replace(/\\/g, '/');
			return true;
		},
		'cmake_argument' : function() {
			return '-DINSTDIR:STRING="' + installerResponses['Install Directory'] + '"';
		}
	},
	{
		'question' : [
			'Would you like to build using NMake instead of using Visual Studio?',
			'NOTE: If you decide to use NMake, you must be in an environment where',
			'      NMake can function, such as the Visual Studio command line.',
			'      If you say yes to this while not in an environment that can run',
			'      NMake, it can cause the CMake configuration to enter an endless',
			'      loop.'
		],
		'short' : 'Use NMake?',
		'options' : [
			'yes',
			'no'
		],
		'default_answer' : 'no',
		'store_answer' : function(answer) {
			installerResponses['Use NMake'] = answer;
			return true;
		},
		'cmake_argument' : function() {
			if (installerResponses['Use NMake'] == 'yes')
				return '-G"NMake Makefiles"';
			else
				return '';
		}
	},
	{
		'question' : [
			'Would you like to build a debug version of Anope?'
		],
		'short' : 'Build debug?',
		'options' : [
			'yes',
			'no'
		],
		'default_answer' : 'no',
		'store_answer' : function(answer) {
			installerResponses['Debug'] = answer;
			return true;
		},
		'cmake_argument' : function() {
			if (installerResponses['Debug'] == 'msvc')
				return '';
			else if (installerResponses['Debug'] == 'yes')
				return '-DCMAKE_BUILD_TYPE:STRING=DEBUG';
			else
				return '-DCMAKE_BUILD_TYPE:STRING=RELEASE';
		}
	},
	{
		'question' : [
			'Are you using Visual Studio 2008?  If you are, you need to answer yes',
			'to this question, otherwise CMake will not function properly.'
		],
		'short' : 'Using Visual Studio 2008?',
		'options' : [
			'yes',
			'no'
		],
		'default_answer' : 'no',
		'store_answer' : function(answer) {
			installerResponses['Visual Studio 2008'] = answer;
			return true;
		},
		'cmake_argument' : function() {
			if (installerResponses['Visual Studio 2008'] == 'yes')
				return '-G"Visual Studio 9 2008"';
			else
				return '';
		}
	},
];

var bannerReplacements = [
	{
		'findtext' : /CURVER/g,
		'replacement' : function() { FindAnopeVersion(); return anopeVersion; }
	},
	 {
		'findtext' : / For more options type SOURCE_DIR\/Config --help/g,
		'replacement' : function() { return ''; }
	}
];

var ScriptPath = WScript.ScriptFullName.substr(0, WScript.ScriptFullName.length - WScript.ScriptName.length);

var fso = WScript.CreateObject('Scripting.FileSystemObject');
var x, y, z;

if (fso.FileExists(ScriptPath + '.BANNER'))
{
	var bannerStream = fso.OpenTextFile(ScriptPath + '.BANNER');
	var bannerText = bannerStream.ReadAll();
	bannerStream.close();

	for (x in bannerReplacements)
	{
		var thisReplacement = bannerReplacements[x];
		bannerText = bannerText.replace(thisReplacement['findtext'], thisReplacement['replacement']);
	}

	WScript.Echo(bannerText + "\n");
}
else
	WScript.Echo("ERROR: Cannot find banner file!\n");

WScript.Echo('Press Enter to Begin...');
InstallerInput();
WScript.Echo('');

for (x in installerQuestions)
{
	var thisQuestion = installerQuestions[x];
	var validResponse = false;
	var validOpts = new Array();
	if (thisQuestion.short == 'Build debug?' && installerResponses['Use NMake'] == 'no')
	{
		installerResponses['Debug'] = 'msvc';
		continue;
	}
	if (thisQuestion.short == 'Using Visual Studio 2008?' && installerResponses['Debug'] != 'msvc')
	{
		installerResponses['Visual Studio 2008'] = 'no';
		continue;
	}
	while (!validResponse)
	{
		for (y in thisQuestion.question)
		{
			var qLine = thisQuestion.question[y];
			WScript.Echo(qLine);
		}
		WScript.Echo('');
		var choiceLine = '';
		if (thisQuestion.options)
		{
			for (y in thisQuestion.options)
			{
				choiceLine += thisQuestion.options[y] + ', ';
				validOpts[thisQuestion.options[y]] = true;
			}
			choiceLine = choiceLine.substring(0, choiceLine.length - 2);
			WScript.Echo('Available Options: ' + choiceLine);
		}
		if (thisQuestion.default_answer)
			WScript.Echo('Default Answer: ' + thisQuestion.default_answer + "\n");
		WScript.Echo(thisQuestion.short);
		var inputValue = InstallerInput().toLowerCase();
		if (!inputValue)
			inputValue = thisQuestion.default_answer;
		if (choiceLine && !validOpts[inputValue])
			WScript.Echo("ERROR: Invalid option '" + inputValue + "'\n");
		else if (thisQuestion.store_answer(inputValue))
			validResponse = true;
	}
	WScript.Echo('');
}

WScript.Echo("\nAnope will be compiled with the following options:\n");
for (x in installerResponses)
{
	var thisResponse = installerResponses[x];
	WScript.Echo("\t" + x + ":\t\t[" + thisResponse.toUpperCase() + "]");
}
WScript.Echo("\tAnope Version:\t\t\t" + anopeVersion);
WScript.Echo("\nTo continue, please press Enter...");
InstallerInput();

var cmake = 'cmake';
for (x in installerQuestions)
{
	var thisQuestion = installerQuestions[x];
	cmake += ' ' + thisQuestion.cmake_argument();
}
var fixedScriptPath = ScriptPath.replace(/\\/g, '/');
cmake += ' "' + fixedScriptPath + '"';
WScript.Echo(cmake + "\n");

var shell = WScript.CreateObject('WScript.Shell');
var cmake_shell = shell.exec('%comspec% /c ' + cmake);
while (!cmake_shell.Status)
{
	if (!cmake_shell.StdOut.AtEndOfStream)
		WScript.Echo(cmake_shell.StdOut.ReadLine());
	else if (!cmake_shell.StdErr.AtEndOfStream)
		WScript.Echo(cmake_shell.StdErr.ReadLine());
}

if (cmake_shell.ExitCode == 0)
{
	if (installerResponses['Use NMake'] == 'yes') WScript.Echo("\nTo compile Anope, run 'nmake'. To install, run 'nmake install'.\n");
	else WScript.Echo("\nTo compile Anope, open Anope.sln and build the solution. To install,\ndo a build on the INSTALL project.\n");
	WScript.Echo("If you update Anope, you should run this script again to ensure\nall available options are set.\n");
}
else
	WScript.Echo("\nThere was an error attempting to run CMake! Check the above error message,\nand contact the Anope team if you are unsure how to proceed.\n");

// -----------------------------------------------------------------

// Functions

function FindAnopeVersion() {
	if (!fso.FileExists(ScriptPath + 'version.log'))
	{
		anopeVersion = 'Unknown';
		return;
	}

	var versionLog = fso.OpenTextFile(ScriptPath + 'version.log');
	while (!versionLog.atEndOfStream)
	{
		var versionLine = versionLog.readline();
		var thisMatch = versionLine.replace('\n', '');
		while (thisMatch.match(/\"/g))
			thisMatch = thisMatch.replace('"', '');
		versionLine = thisMatch;
		if (versionLine.match(/VERSION_MAJOR=/g))
		{
			vMaj = versionLine.replace('VERSION_MAJOR=', '');
			continue;
		}
		if (versionLine.match(/VERSION_MINOR=/g))
		{
			vMin = versionLine.replace('VERSION_MINOR=', '');
			continue;
		}
		if (versionLine.match(/VERSION_PATCH=/g))
		{
			vPat = versionLine.replace('VERSION_PATCH=', '');
			continue;
		}
		if (versionLine.match(/VERSION_EXTRA=/g))
		{
			vExtra = versionLine.replace('VERSION_EXTRA=', '');
			continue;
		}
		if (versionLine.match(/VERSION_BUILD=/g))
		{
			vBuild = versionLine.replace('VERSION_BUILD=', '');
			continue;
		}
	}
	versionLog.close();
	anopeVersion = vMaj + '.' + vMin + '.' + vPat + '.' + vBuild + vExtra;
	return;
}

function InstallerInput() {
	var input = WScript.StdIn.Readline();
	return input;
}
