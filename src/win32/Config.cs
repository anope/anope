/*
 * Config.cs - Windows Configuration
 *
 * (C) 2003-2016 Anope Team
 * Contact us at team@anope.org
 *
 * This program is free but copyrighted software; see the file COPYING for
 * details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 *
 * Written by Scott <stealtharcher.scott@gmail.com>
 * Written by Adam <Adam@anope.org>
 * Cleaned up by Naram Qashat <cyberbotx@anope.org>
 *
 * Compile with: csc /out:../../Config.exe /win32icon:anope-icon.ico Config.cs
 */

using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.Reflection;

namespace Config
{
	class Config
	{
		static string ExecutablePath, InstallDirectory, ExtraIncludeDirs, ExtraLibDirs, ExtraArguments;
		static bool UseNMake = true, BuildDebug = false;

		static bool CheckResponse(string InstallerResponse)
		{
			if (string.Compare(InstallerResponse, "yes", true) == 0 || string.Compare(InstallerResponse, "y", true) == 0)
				return true;
			return false;
		}

		static bool LoadCache()
		{
			try
			{
				string[] cache = File.ReadAllLines(string.Format(@"{0}\config.cache", ExecutablePath));
				if (cache.Length > 0)
					Console.WriteLine("Using defaults from config.cache");
				foreach (string line in cache)
				{
					int e = line.IndexOf('=');
					string name = line.Substring(0, e);
					string value = line.Substring(e + 1);

					if (name == "INSTDIR")
						InstallDirectory = value;
					else if (name == "DEBUG")
						BuildDebug = CheckResponse(value);
					else if (name == "USENMAKE")
						UseNMake = CheckResponse(value);
					else if (name == "EXTRAINCLUDE")
						ExtraIncludeDirs = value;
					else if (name == "EXTRALIBS")
						ExtraLibDirs = value;
					else if (name == "EXTRAARGS")
						ExtraArguments = value;
				}

				return true;
			}
			catch (Exception)
			{
			}

			return false;
		}

		static void SaveCache()
		{
			using (TextWriter tw = new StreamWriter(string.Format(@"{0}\config.cache", ExecutablePath)))
			{
				tw.WriteLine("INSTDIR={0}", InstallDirectory);
				tw.WriteLine("DEBUG={0}", BuildDebug ? "yes" : "no");
				tw.WriteLine("USENMAKE={0}", UseNMake ? "yes" : "no");
				tw.WriteLine("EXTRAINCLUDE={0}", ExtraIncludeDirs);
				tw.WriteLine("EXTRALIBS={0}", ExtraLibDirs);
				tw.WriteLine("EXTRAARGS={0}", ExtraArguments);
			}
		}

		static string HandleCache(int i)
		{
			switch (i)
			{
				case 0:
					Console.Write("[{0}] ", InstallDirectory);
					return InstallDirectory;
				case 1:
					Console.Write("[{0}] ", UseNMake ? "yes" : "no");
					return UseNMake ? "yes" : "no";
				case 2:
					Console.Write("[{0}] ", BuildDebug ? "yes" : "no");
					return BuildDebug ? "yes" : "no";
				case 3:
					Console.Write("[{0}] ", ExtraIncludeDirs);
					return ExtraIncludeDirs;
				case 4:
					Console.Write("[{0}] ", ExtraLibDirs);
					return ExtraLibDirs;
				case 5:
					Console.Write("[{0}] ", ExtraArguments);
					return ExtraArguments;
				default:
					break;
			}

			return null;
		}

		static string FindAnopeVersion()
		{
			if (!File.Exists(string.Format(@"{0}\src\version.sh", ExecutablePath)))
				return "Unknown";

			Dictionary<string, string> versions = new Dictionary<string, string>();
			string[] versionfile = File.ReadAllLines(string.Format(@"{0}\src\version.sh", ExecutablePath));
			foreach (string line in versionfile)
				if (line.StartsWith("VERSION_"))
				{
					string key = line.Split('_')[1].Split('=')[0];
					if (!versions.ContainsKey(key))
						versions.Add(key, line.Split('=')[1].Replace("\"", "").Replace("\'", ""));
				}

			try
			{
				if (versions.ContainsKey("BUILD"))
					return string.Format("{0}.{1}.{2}.{3}{4}", versions["MAJOR"], versions["MINOR"], versions["PATCH"], versions["BUILD"], versions["EXTRA"]);
				else
					return string.Format("{0}.{1}.{2}{3}", versions["MAJOR"], versions["MINOR"], versions["PATCH"], versions["EXTRA"]);
			}
			catch (Exception e)
			{
				Console.WriteLine(e.Message);
				return "Unknown";
			}
		}

		static void RunCMake(string cMake)
		{
			Console.WriteLine("cmake {0}", cMake);
			try
			{
				ProcessStartInfo processStartInfo = new ProcessStartInfo("cmake")
				{
					RedirectStandardError = true,
					RedirectStandardOutput = true,
					UseShellExecute = false,
					Arguments = cMake
				};
				Process pCMake = Process.Start(processStartInfo);
				StreamReader stdout = pCMake.StandardOutput, stderr = pCMake.StandardError;
				string stdoutr, stderrr;
				List<string> errors = new List<string>();
				while (!pCMake.HasExited)
				{
					if ((stdoutr = stdout.ReadLine()) != null)
						Console.WriteLine(stdoutr);
					if ((stderrr = stderr.ReadLine()) != null)
						errors.Add(stderrr);
				}
				foreach (string error in errors)
					Console.WriteLine(error);
				Console.WriteLine();
				if (pCMake.ExitCode == 0)
				{
					if (UseNMake)
						Console.WriteLine("To compile Anope, run 'nmake'. To install, run 'nmake install'");
					else
						Console.WriteLine("To compile Anope, open Anope.sln and build the solution. To install, do a build on the INSTALL project");
				}
				else
					Console.WriteLine("There was an error attempting to run CMake! Check the above error message, and contact the Anope team if you are unsure how to proceed.");
			}
			catch (Exception e)
			{
				Console.WriteLine();
				Console.WriteLine(DateTime.UtcNow + " UTC: " + e.Message);
				Console.WriteLine("There was an error attempting to run CMake! Check the above error message, and contact the Anope team if you are unsure how to proceed.");
			}
		}

		static int Main(string[] args)
		{
			bool IgnoreCache = false, NoIntro = false, DoQuick = false;

			if (args.Length > 0)
			{
				if (args[0] == "--help")
				{
					Console.WriteLine("Config utility for Anope");
					Console.WriteLine("------------------------");
					Console.WriteLine("Syntax: .\\Config.exe [options]");
					Console.WriteLine("-nocache     Ignore settings saved in config.cache");
					Console.WriteLine("-nointro     Skip intro (disclaimer, etc)");
					Console.WriteLine("-quick or -q Skip questions, go straight to cmake");
					return 0;
				}
				else if (args[0] == "-nocache")
					IgnoreCache = true;
				else if (args[0] == "-nointro")
					NoIntro = true;
				else if (args[0] == "-quick" || args[0] == "-q")
					DoQuick = true;
			}

			ExecutablePath = Path.GetDirectoryName(Assembly.GetExecutingAssembly().Location);

			string AnopeVersion = FindAnopeVersion();

			if (!NoIntro && File.Exists(string.Format(@"{0}\.BANNER", ExecutablePath)))
				Console.WriteLine(File.ReadAllText(string.Format(@"{0}\.BANNER", ExecutablePath)).Replace("CURVER", AnopeVersion).Replace("For more options type SOURCE_DIR/Config --help", ""));

			Console.WriteLine("Press Enter to begin");
			Console.WriteLine();
			Console.ReadKey();

			bool UseCache = false;

			if (DoQuick || !IgnoreCache)
			{
				UseCache = LoadCache();
				if (DoQuick && !UseCache)
				{
					Console.WriteLine("Can't find cache file (config.cache), aborting...");
					return 1;
				}
			}

			if (!DoQuick)
			{
				List<string> InstallerQuestions = new List<string>()
				{
					"Where do you want Anope to be installed?",
					"Would you like to build using NMake instead of using Visual Studio?\r\nNOTE: If you decide to use NMake, you must be in an environment where\r\nNMake can function, such as the Visual Studio command line. If you say\r\nyes to this while not in an environment that can run NMake, it can\r\ncause the CMake configuration to enter an endless loop. [y/n]",
					"Would you like to build a debug version of Anope? [y/n]",
					"Are there any extra include directories you wish to use?\nYou may only need to do this if CMake is unable to locate missing dependencies without hints.\nSeparate directories with semicolons and use slashes (aka /) instead of backslashes (aka \\).\nIf you need no extra include directories, enter NONE in all caps.",
					"Are there any extra library directories you wish to use?\nYou may only need to do this if CMake is unable to locate missing dependencies without hints.\nSeparate directories with semicolons and use slashes (aka /) instead of backslashes (aka \\).\nIf you need no extra library directories, enter NONE in all caps.",
					"Are there any extra arguments you wish to pass to CMake?\nIf you need no extra arguments to CMake, enter NONE in all caps."
				};

				for (int i = 0; i < InstallerQuestions.Count; ++i)
				{
					Console.WriteLine(InstallerQuestions[i]);
					string CacheResponse = null;
					if (UseCache)
						CacheResponse = HandleCache(i);
					string InstallerResponse = Console.ReadLine();
					Console.WriteLine();

					if (!string.IsNullOrWhiteSpace(CacheResponse) && string.IsNullOrWhiteSpace(InstallerResponse))
						InstallerResponse = CacheResponse;

					// Question 4+ are optional
					if (i < 3 && string.IsNullOrWhiteSpace(InstallerResponse))
					{
						Console.WriteLine("Invalid option");
						--i;
						continue;
					}

					switch (i)
					{
						case 0:
							if (!Directory.Exists(InstallerResponse))
							{
								Console.WriteLine("Directory does not exist! Creating directory.");
								Console.WriteLine();
								try
								{
									Directory.CreateDirectory(InstallerResponse);
									InstallDirectory = InstallerResponse;
								}
								catch (Exception e)
								{
									Console.WriteLine("Unable to create directory: " + e.Message);
									--i;
								}
							}
							else if (File.Exists(InstallerResponse + @"\include\services.h"))
							{
								Console.WriteLine("You cannot use the Anope source directory as the target directory!");
								--i;
							}
							else
								InstallDirectory = InstallerResponse;
							break;
						case 1:
							UseNMake = CheckResponse(InstallerResponse);
							if (UseNMake)
								++i;
							break;
						case 2:
							BuildDebug = CheckResponse(InstallerResponse);
							break;
						case 3:
							if (InstallerResponse == "NONE")
								ExtraIncludeDirs = null;
							else
								ExtraIncludeDirs = InstallerResponse;
							break;
						case 4:
							if (InstallerResponse == "NONE")
								ExtraLibDirs = null;
							else
								ExtraLibDirs = InstallerResponse;
							break;
						case 5:
							if (InstallerResponse == "NONE")
								ExtraArguments = null;
							else
								ExtraArguments = InstallerResponse;
							break;
						default:
							break;
					}
				}
			}

			Console.WriteLine("Anope will be compiled with the following options:");
			Console.WriteLine("Install directory: {0}", InstallDirectory);
			Console.WriteLine("Use NMake: {0}", UseNMake ? "Yes" : "No");
			Console.WriteLine("Build debug: {0}", BuildDebug ? "Yes" : "No");
			Console.WriteLine("Anope Version: {0}", AnopeVersion);
			Console.WriteLine("Extra Include Directories: {0}", ExtraIncludeDirs);
			Console.WriteLine("Extra Library Directories: {0}", ExtraLibDirs);
			Console.WriteLine("Extra Arguments: {0}", ExtraArguments);
			Console.WriteLine("Press Enter to continue...");
			Console.ReadKey();

			SaveCache();

			if (!string.IsNullOrWhiteSpace(ExtraIncludeDirs))
				ExtraIncludeDirs = string.Format("-DEXTRA_INCLUDE:STRING={0} ", ExtraIncludeDirs);
			else
				ExtraIncludeDirs = "";
			if (!string.IsNullOrWhiteSpace(ExtraLibDirs))
				ExtraLibDirs = string.Format("-DEXTRA_LIBS:STRING={0} ", ExtraLibDirs);
			else
				ExtraLibDirs = "";
			if (!string.IsNullOrWhiteSpace(ExtraArguments))
				ExtraArguments += " ";
			else
				ExtraArguments = "";

			InstallDirectory = "-DINSTDIR:STRING=\"" + InstallDirectory.Replace('\\', '/') + "\" ";
			string NMake = UseNMake ? "-G\"NMake Makefiles\" " : "";
			string Debug = BuildDebug ? "-DCMAKE_BUILD_TYPE:STRING=DEBUG " : "-DCMAKE_BUILD_TYPE:STRING=RELEASE ";
			string cMake = InstallDirectory + NMake + Debug + ExtraIncludeDirs + ExtraLibDirs + ExtraArguments + "\"" + ExecutablePath.Replace('\\', '/') + "\"";
			RunCMake(cMake);

			return 0;
		}
	}
}
