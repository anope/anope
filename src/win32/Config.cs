/*
 * Config.cs - Windows Configuration
 *
 * (C) 2003-2012 Anope Team
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
 *
 * Compile with: csc /out:../../Config.exe /win32icon:anope-icon.ico Config.cs
 *
 */


using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;

namespace Config
{
	class Config
	{
		static string InstallDirectory, VSVersion, VSShortVer, ExtraArguments;
		static bool UseNMake = true, BuildDebug = false;

		static bool CheckResponse(string InstallerResponse)
		{
			if (System.String.Compare(InstallerResponse, "yes", true) == 0 || System.String.Compare(InstallerResponse, "y", true) == 0)
				return true;
			return false;
		}
		
		static bool LoadCache()
		{
			try
			{
				string[] cache = File.ReadAllLines("config.cache");
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
					else if (name == "EXTRAARGS")
						ExtraArguments = value;
					else if (name == "VSVERSION")
						VSVersion = value;
					else if (name == "VSSHORTVER")
						VSShortVer = value;
				}

				return true;
			}
			catch (Exception) { }
			
			return false;
		}
		
		static void SaveCache()
		{
			TextWriter tw = new StreamWriter("config.cache");
			tw.WriteLine("INSTDIR=" + InstallDirectory);
			tw.WriteLine("DEBUG=" + (BuildDebug ? "yes" : "no"));
			tw.WriteLine("USENMAKE=" + (UseNMake ? "yes" : "no"));
			tw.WriteLine("EXTRAARGS=" + ExtraArguments);
			tw.WriteLine("VSVERSION=" + VSVersion);
			tw.WriteLine("VSSHORTVER=" + VSShortVer);
			tw.Close();
		}
		
		static string HandleCache(int i)
		{
			switch (i)
			{
				case 0:
					Console.Write("[" + InstallDirectory + "] ");
					return InstallDirectory;
				case 1:
					Console.Write("[" + UseNMake + "] ");
					return (UseNMake ? "yes" : "no");
				case 2:
					Console.Write("[" + VSShortVer + "] ");
					return VSShortVer;
				case 3:
					Console.Write("[" + BuildDebug + "] ");
					return (BuildDebug ? "yes" : "no");
				case 4:
					Console.Write("[" + ExtraArguments + "] ");
					return ExtraArguments;
				default:
					break;
			}
			
			return null;
		}

		static string FindAnopeVersion()
		{
			if (!File.Exists(@"src\version.sh"))
				return "Unknown";

			Dictionary<string, string> versions = new Dictionary<string, string>();
			string[] versionfile = File.ReadAllLines(@"src\version.sh");
			foreach (string line in versionfile)
			{
				if (line.StartsWith("VERSION_"))
				{
					string key = line.Split('_')[1].Split('=')[0];
					if (!versions.ContainsKey(key))
					{
						versions.Add(key, line.Split('=')[1].Replace("\"", "").Replace("\'", ""));
					}
				}
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
				StreamReader stdout = pCMake.StandardOutput;
				StreamReader stderr = pCMake.StandardError;
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
				{
					Console.WriteLine(error);
				}
				Console.WriteLine("");
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
				Console.WriteLine("");
				Console.WriteLine(DateTime.UtcNow + " UTC: " + e.Message);
				Console.WriteLine("There was an error attempting to run CMake! Check the above error message, and contact the Anope team if you are unsure how to proceed.");
			}
		}

		static int Main()
		{
			string AnopeVersion = FindAnopeVersion();

			if (File.Exists(".BANNER"))
				Console.WriteLine(File.ReadAllText(".BANNER").Replace("CURVER", AnopeVersion).Replace("For more options type SOURCE_DIR/Config --help", ""));

			Console.WriteLine("Press Enter to begin");
			Console.WriteLine("");
			Console.ReadKey();
			
			bool UseCache = LoadCache();

			Dictionary<int, string> InstallerQuestions = new Dictionary<int, string>();
			InstallerQuestions.Add(0, "Where do you want Anope to be installed?");
			InstallerQuestions.Add(1, "Would you like to build using NMake instead of using Visual Studio?\r\nNOTE: If you decide to use NMake, you must be in an environment where\r\nNMake can function, such as the Visual Studio command line. If you say\r\nyes to this while not in an environment that can run NMake, it can\r\ncause the CMake configuration to enter an endless loop. [y/n]");
			InstallerQuestions.Add(2, "Are you using Visual Studio 2008 or 2010? You can leave this blank\nand have CMake try and auto detect it, but this usually doesn't\nwork correctly. [2008/2010]");
			InstallerQuestions.Add(3, "Would you like to build a debug version of Anope? [y/n]");
			InstallerQuestions.Add(4, "Are there any extra arguments you wish to pass to cmake?\nYou may only need to do this if cmake is unable to locate missing dependencies without hints.\nTo do this, set the variable EXTRA_INCLUDE like this: -DEXTRA_INCLUDE:STRING=c:/some/path/include;c:/some/path/bin;c:/some/path/lib");

			for (int i = 0; i < InstallerQuestions.Count; ++i)
			{
				Console.WriteLine(InstallerQuestions[i]);
				string CacheResponse = null;
				if (UseCache)
					CacheResponse = HandleCache(i);
				string InstallerResponse = Console.ReadLine();
				Console.WriteLine("");
				
				if (CacheResponse != null && (InstallerResponse == null || InstallerResponse.Length < 1))
					InstallerResponse = CacheResponse;

				// Question 4 is optional
				if (i != 4 && (InstallerResponse == null || InstallerResponse.Length < 1))
				{
					Console.WriteLine("Invlaid option");
					--i;
					continue;
				}

				switch (i)
				{
					case 0:
						if (!Directory.Exists(InstallerResponse))
						{
							Console.WriteLine("Directory does not exist! Creating directory.");
							Console.WriteLine("");
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
						if (InstallerResponse == "2010")
						{
							VSVersion = "-G\"Visual Studio 10\" ";
							VSShortVer = "2010";
						}
						else if (InstallerResponse == "2008")
						{
							VSVersion = "-G\"Visual Studio 9 2008\" ";
							VSShortVer = "2008";
						}
						break;
					case 3:
						BuildDebug = CheckResponse(InstallerResponse);
						break;
					case 4:
						ExtraArguments = InstallerResponse;
						break;
					default:
						break;
				}
			}

			Console.WriteLine("Anope will be compiled with the following options:");
			Console.WriteLine("Install directory: {0}", InstallDirectory);
			Console.WriteLine("Use NMake: {0}", UseNMake ? "Yes" : "No");
			if (VSShortVer != null)
				Console.WriteLine("Using Visual Studio: {0}", VSShortVer);
			else
				Console.WriteLine("Using Visual Studio: No");
			Console.WriteLine("Build debug: {0}", BuildDebug ? "Yes" : "No");
			Console.WriteLine("Anope Version: {0}", AnopeVersion); ;
			if (ExtraArguments != null)
				Console.WriteLine("Extra Arguments: {0}", ExtraArguments);
			Console.WriteLine("Press Enter to continue...");
			Console.ReadKey();
			
			SaveCache();
			
			if (ExtraArguments != null)
				ExtraArguments += " ";
			
			InstallDirectory = "-DINSTDIR:STRING=\"" + InstallDirectory.Replace('\\', '/') + "\" ";
			string NMake = UseNMake ? "-G\"NMake Makefiles\" " : "";
			string Debug = BuildDebug ? "-DCMAKE_BUILD_TYPE:STRING=DEBUG " : "-DCMAKE_BUILD_TYPE:STRING=RELEASE ";
			string cMake = InstallDirectory + NMake + Debug + VSVersion + ExtraArguments + Environment.CurrentDirectory.Replace('\\','/');
			RunCMake(cMake);

			return 0;
		}
	}
}

