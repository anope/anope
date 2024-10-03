# Find all the *.cpp files within the current source directory, and sort the list
file(GLOB SRC_SRCS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "*.cpp")

if(WIN32)
  list(APPEND SRC_SRCS win32/socket.cpp)
  list(APPEND SRC_SRCS win32/windows.cpp)
  list(APPEND SRC_SRCS win32/dl/dl.cpp)
  list(APPEND SRC_SRCS win32/pipe/pipe.cpp)
  list(APPEND SRC_SRCS win32/sigaction/sigaction.cpp)
endif()

if(HAVE_EPOLL)
  list(APPEND SRC_SRCS socketengines/epoll.cpp)
elseif(HAVE_KQUEUE)
  list(APPEND SRC_SRCS socketengines/kqueue.cpp)
elseif(HAVE_POLL)
  list(APPEND SRC_SRCS socketengines/poll.cpp)
else()
  list(APPEND SRC_SRCS socketengines/select.cpp)
endif()

list(SORT SRC_SRCS)

# Set all the files to use C++ as well as set their compile flags (use the module-specific compile flags, though)
set_source_files_properties(${SRC_SRCS} PROPERTIES LANGUAGE CXX COMPILE_FLAGS "${CXXFLAGS}")

# Under Windows, we also include a resource file to the build
if(WIN32)
  # Make sure that the resource file is seen as an RC file to be compiled with a resource compiler, not a C++ compiler
  set_source_files_properties(${CMAKE_CURRENT_BINARY_DIR}/win32/win32.rc LANGUAGE RC)
  # Add the resource file to the list of sources
  list(APPEND SRC_SRCS ${CMAKE_CURRENT_BINARY_DIR}/win32/win32.rc)

  set_source_files_properties(${CMAKE_CURRENT_BINARY_DIR}/win32/win32.rc COMPILE_FLAGS "/i\"${Anope_SOURCE_DIR}/include\"")
endif()

# If compiling with Visual Studio, create a static library out of win32/win32_memory.cpp to be included with everything else, needed to override its override of new/delete operators
if(MSVC)
  set_source_files_properties(win32/win32_memory.cpp PROPERTIES LANGUAGE CXX COMPILE_FLAGS "${CXXFLAGS}")
  add_library(win32_memory STATIC win32/win32_memory.cpp)
  set(WIN32_MEMORY win32_memory)
  set(EXTRA_LDFLAGS "/OPT:NOREF") # https://sourceware.org/bugzilla/show_bug.cgi?id=12633
else()
  set(WIN32_MEMORY)
endif()

# My Addons
find_package(OpenSSL REQUIRED)
#target_link_libraries(${PROGRAM_NAME} OpenSSL::SSL OpenSSL::Crypto)

# Generate the Anope executable and set it's linker flags, also set it to export it's symbols even though it's not a module
add_executable(${PROGRAM_NAME} ${SRC_SRCS})
set_target_properties(${PROGRAM_NAME} PROPERTIES LINKER_LANGUAGE CXX LINK_FLAGS "${LDFLAGS} ${EXTRA_LDFLAGS}" ENABLE_EXPORTS ON INSTALL_RPATH_USE_LINK_PATH ON BUILD_WITH_INSTALL_RPATH ON)

# On Windows, also link Anope to the wsock32 and Ws2_32 library, as well as set the version
if(WIN32)
  target_link_libraries(${PROGRAM_NAME} wsock32 Ws2_32 ${LINK_LIBS} ${WIN32_MEMORY})
  set_target_properties(${PROGRAM_NAME} PROPERTIES VERSION "${VERSION_DOTTED}")
else()
  target_link_libraries(${PROGRAM_NAME} ${LINK_LIBS})
endif()

# If being built with localisation we might need to link against libintl.
if(HAVE_LOCALIZATION AND Intl_LIBRARY)
  target_link_libraries(${PROGRAM_NAME} ${Intl_LIBRARY})
endif()

# Building the Anope executable requires the version.h header to be generated
add_dependencies(${PROGRAM_NAME} headers)
# Also require the language files if we have gettext
if(HAVE_LOCALIZATION)
  add_dependencies(${PROGRAM_NAME} language)
endif()

# Get the filename of the Anope executable as it is in on this system
set(SERVICES_BINARY "$<TARGET_FILE:${PROGRAM_NAME}>")
get_filename_component(SERVICES_BINARY ${SERVICES_BINARY} NAME)
# Add the Anope executable to the list of files for CPack to ignore
add_to_cpack_ignored_files("${SERVICES_BINARY}$" TRUE)

# Generate sysconf.h from the earlier configuration
configure_file(${Anope_SOURCE_DIR}/include/sysconf.h.cmake ${Anope_BINARY_DIR}/include/sysconf.h)

# Go into the following directories and run their CMakeLists.txt as well
if(NOT DISABLE_TOOLS)
  add_subdirectory(tools)
endif()

# Set Anope to be installed to the bin directory
install(TARGETS ${PROGRAM_NAME}
  DESTINATION ${BIN_DIR}
  RUNTIME
)
