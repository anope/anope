# Find all the *.cpp files within the current source directory, and sort the list
file(GLOB SRC_SRCS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "*.cpp")

if(WIN32)
  append_to_list(SRC_SRCS win32/dir/dir.cpp)
  append_to_list(SRC_SRCS win32/socket.cpp)
  append_to_list(SRC_SRCS win32/windows.cpp)
  append_to_list(SRC_SRCS win32/dl/dl.cpp)
  append_to_list(SRC_SRCS win32/pipe/pipe.cpp)
  append_to_list(SRC_SRCS win32/pthread/pthread.cpp)
  append_to_list(SRC_SRCS win32/sigaction/sigaction.cpp)
endif(WIN32)

if(HAVE_EPOLL)
  append_to_list(SRC_SRCS socketengines/socketengine_epoll.cpp)
else(HAVE_EPOLL)
  if(HAVE_KQUEUE)
    append_to_list(SRC_SRCS socketengines/socketengine_kqueue.cpp)
  else(HAVE_KQUEUE)
    if(HAVE_POLL)
      append_to_list(SRC_SRCS socketengines/socketengine_poll.cpp)
    else(HAVE_POLL)
      append_to_list(SRC_SRCS socketengines/socketengine_select.cpp)
    endif(HAVE_POLL)
  endif(HAVE_KQUEUE)
endif(HAVE_EPOLL)

sort_list(SRC_SRCS)

# Set all the files to use C++ as well as set their compile flags (use the module-specific compile flags, though)
set_source_files_properties(${SRC_SRCS} PROPERTIES LANGUAGE CXX COMPILE_FLAGS "${CXXFLAGS}")

# Create an empty list to store extra include directories
set(EXTRA_INCLUDES)
# Iterate through all the source files
foreach(SRC ${SRC_SRCS})
  # Temporary variable for the current source's include directories
  set(TEMP_INCLUDES)
  # Calculate the header file dependencies for the given source file
  calculate_depends(${SRC} TEMP_INCLUDES)
  # If there were some extra include directories, add them to the list
  if(TEMP_INCLUDES)
    append_to_list(EXTRA_INCLUDES ${TEMP_INCLUDES})
  endif(TEMP_INCLUDES)
endforeach(SRC)
# If there were extra include directories, remove the duplicates and add the directories to the include path
if(EXTRA_INCLUDES)
  remove_list_duplicates(EXTRA_INCLUDES)
  include_directories(${EXTRA_INCLUDES})
endif(EXTRA_INCLUDES)

# Under Windows, we also include a resource file to the build
if(WIN32)
  # Make sure that the resource file is seen as an RC file to be compiled with a resource compiler, not a C++ compiler
  set_source_files_properties(${CMAKE_CURRENT_BINARY_DIR}/win32/win32.rc LANGUAGE RC)
  # Add the resource file to the list of sources
  append_to_list(SRC_SRCS ${CMAKE_CURRENT_BINARY_DIR}/win32/win32.rc)
  # For MinGW, we have to change the compile flags
  if(MINGW)
    set(RC_CFLAGS "-DMINGW -Ocoff -I${Anope_SOURCE_DIR}/include")
    # If any sort of debugging is being enabled, add a _DEBUG define to the flags for the resource compiler
    if(CMAKE_BUILD_TYPE STREQUAL "DEBUG" OR CMAKE_BUILD_TYPE STREQUAL "RELWITHDEBINFO")
      set(RC_CFLAGS "${RC_CFLAGS} -D_DEBUG")
    endif(CMAKE_BUILD_TYPE STREQUAL "DEBUG" OR CMAKE_BUILD_TYPE STREQUAL "RELWITHDEBINFO")
    set_source_files_properties(${CMAKE_CURRENT_BINARY_DIR}/win32/win32.rc COMPILE_FLAGS "${RC_CFLAGS}")
  # For anything else, assumingly Visual Studio at this point, use a different set of compile flags
  else(MINGW)
    set_source_files_properties(${CMAKE_CURRENT_BINARY_DIR}/win32/win32.rc COMPILE_FLAGS "/i\"${Anope_SOURCE_DIR}/include\"")
  endif(MINGW)
endif(WIN32)

# If compiling with Visual Studio, create a static library out of win32/win32_memory.cpp to be included with everything else, needed to override its override of new/delete operators
if(MSVC)
  set_source_files_properties(win32/win32_memory.cpp PROPERTIES LANGUAGE CXX COMPILE_FLAGS "${CXXFLAGS}")
  add_library(win32_memory STATIC win32/win32_memory.cpp)
  set(WIN32_MEMORY win32_memory)
  set(EXTRA_LDFLAGS "/OPT:NOREF") # https://sourceware.org/bugzilla/show_bug.cgi?id=12633
else(MSVC)
  set(WIN32_MEMORY)
endif(MSVC)

# Generate the Anope executable and set it's linker flags, also set it to export it's symbols even though it's not a module
add_executable(${PROGRAM_NAME} ${SRC_SRCS})
set_target_properties(${PROGRAM_NAME} PROPERTIES LINKER_LANGUAGE CXX LINK_FLAGS "${LDFLAGS} ${EXTRA_LDFLAGS}" ENABLE_EXPORTS ON INSTALL_RPATH_USE_LINK_PATH ON BUILD_WITH_INSTALL_RPATH ON)
# On Windows, also link Anope to the wsock32 and Ws2_32 library, as well as set the version
if(WIN32)
  target_link_libraries(${PROGRAM_NAME} wsock32 Ws2_32 ${LINK_LIBS} ${GETTEXT_LIBRARIES} ${WIN32_MEMORY})
  set_target_properties(${PROGRAM_NAME} PROPERTIES VERSION "${VERSION_DOTTED}")
else(WIN32)
  target_link_libraries(${PROGRAM_NAME} ${LINK_LIBS} ${GETTEXT_LIBRARIES})
endif(WIN32)
# Building the Anope executable requires the version.h header to be generated
add_dependencies(${PROGRAM_NAME} headers)
# Also require the language files if we have gettext
if(GETTEXT_FOUND)
  add_dependencies(${PROGRAM_NAME} language)
endif(GETTEXT_FOUND)

# Get the filename of the Anope executable as it is in on this system
get_target_property(SERVICES_BINARY ${PROGRAM_NAME} LOCATION)
get_filename_component(SERVICES_BINARY ${SERVICES_BINARY} NAME)
# Add the Anope executable to the list of files for CPack to ignore
add_to_cpack_ignored_files("${SERVICES_BINARY}$" TRUE)

# Generate sysconf.h from the earlier configuration
configure_file(${Anope_SOURCE_DIR}/include/sysconf.h.cmake ${Anope_BINARY_DIR}/include/sysconf.h)

# Go into the following directories and run their CMakeLists.txt as well
if(NOT DISABLE_TOOLS)
  add_subdirectory(tools)
endif(NOT DISABLE_TOOLS)

# Set Anope to be installed to the bin directory
install(TARGETS ${PROGRAM_NAME}
  DESTINATION ${BIN_DIR}
)
