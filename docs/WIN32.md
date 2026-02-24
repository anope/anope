# Building Anope on Windows

## Dependencies

You will need the following software installed to build Anope:

* [CMake 3.20 or newer](https://cmake.org/download/#latest)
* [Conan 2 or newer](https://conan.io/downloads) (optional if you don't want to build extra modules)
* [NSIS 3 or newer](https://nsis.sourceforge.io/Download)
* [Visual Studio 2022 or newer](https://visualstudio.microsoft.com/downloads/) (*NOT* Visual Studio Code)

## Building

First you need to download the latest version of the Anope source code from [the releases page](https://github.com/anope/anope/releases) and unpack it using Windows Explorer.

---

Once you have the source code unpacked you need open the Command Prompt and move to the directory in which Anope has been unpacked. You can do this by pressing Control+R and then entering `cmd.exe` and pressing enter. Once the terminal opens you can move to the directory using the following command (assuming you unpacked to `C:\Users\Example\Downloads\anope-2.1`):

```cmd
cd C:\Users\Example\Downloads\anope-2.1
```

---

If you want to build with multiple-language support or want to use the enc_argon2, mysql, regex_pcre2, regex_posix, regex_tre, sqlite, or ssl_openssl extra modules you will now need to install the third-party dependencies using Conan. Before you can do this you need to create a Conan profile for C++17 using the following command:

```cmd
conan profile detect
notepad ~\.conan2\profiles\default
```

When Notepad opens you should find the line beginning with `compiler.cppstd=` and replace the entire line with `compiler.cppstd=17` and save the file.

Now you're ready to install the third-party dependencies using the following command:


```cmd
conan install .\src\win32 --build missing --deployer runtime_deploy --deployer-folder .\build\extradll --output-folder .
call .\conanbuild.bat
```

This will probably take a long time if its the first time you have run it.

---

Now you're ready to build Anope.

```cmd
cd .\build
cmake -A x64 -D "CMAKE_BUILD_TYPE=Release" -D "CMAKE_TOOLCHAIN_FILE=conan_toolchain.cmake" ..
msbuild PACKAGE.vcxproj /P:Configuration=Release /P:Platform=x64 /VERBOSITY:MINIMAL
```

Once the build finishes the installer will be available in the build directory. You can install this by opening the directory in Windows Explorer and then running it as you would with most Windows installers.

Once installed Anope will be available at `C:\Program Files\Anope`.
