/*
 * Config.cs - Windows Configuration
 *
 * (C) 2003-2010 Anope Team
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
 * Compile with: csc /out:Config.exe Config.cs
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
		static string InstallDirectory;
		static bool UseNMake, BuildDebug;

		static bool CheckResponse(string InstallerResponse)
		{
			if (System.String.Compare(InstallerResponse, "yes", true) == 0 || System.String.Compare(InstallerResponse, "y", true) == 0)
				return true;
			return false;
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

			Dictionary<int, string> InstallerQuestions = new Dictionary<int, string>();
			InstallerQuestions.Add(0, "Where do you want Anope to be installed?");
			InstallerQuestions.Add(1, "Would you like to build using NMake instead of using Visual Studio?\r\nNOTE: If you decide to use NMake, you must be in an environment where\r\nNMake can function, such as the Visual Studio command line. If you say\r\nyes to this while not in an environment that can run NMake, it can\r\ncause the CMake configuration to enter an endless loop. [y/n]");
			InstallerQuestions.Add(2, "Would you like to build a debug version of Anope? [y/n]");

			for (int i = 0; i < InstallerQuestions.Count; ++i)
			{
				Console.WriteLine(InstallerQuestions[i]);
				string InstallerResponse = Console.ReadLine();
				Console.WriteLine("");

				if (InstallerResponse == null || InstallerResponse.Length < 1)
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
						break;
					case 2:
						BuildDebug = CheckResponse(InstallerResponse);
						break;
					default:
						break;
				}
			}

			Console.WriteLine("Anope will be compiled with the following options:");
			Console.WriteLine("Install directory: {0}", InstallDirectory);
			Console.WriteLine("Use NMake: {0}", UseNMake ? "Yes" : "No");
			Console.WriteLine("Using Visual Studio 2010: {0}", !UseNMake ? "Yes" : "No");
			Console.WriteLine("Build debug: {0}", BuildDebug ? "Yes" : "No");
			Console.WriteLine("Anope Version: {0}", AnopeVersion); ;
			Console.WriteLine("Press Enter to continue...");
			Console.ReadKey();
			InstallDirectory = "-DINSTDIR:STRING=\"" + InstallDirectory.Replace('\\', '/') + "\" ";
			string NMake = UseNMake ? "-G\"NMake Makefiles\" " : "";
			string Debug = BuildDebug ? "-DCMAKE_BUILD_TYPE:STRING=DEBUG " : "-DCMAKE_BUILD_TYPE:STRING=RELEASE ";
			string VisualStudio = !UseNMake ? "-G\"Visual Studio 10\" " : "";
			string cMake = InstallDirectory + NMake + Debug + VisualStudio + Environment.CurrentDirectory.Replace('\\','/');
			RunCMake(cMake);

			return 0;
		}
	}
}

