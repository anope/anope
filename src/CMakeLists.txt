# If compiling with Visual Studio, create a static library out of win32/win32_memory.cpp to be included with everything else, needed to override its override of new/delete operators
if(MSVC)
  add_library("win32_memory" STATIC "win32/win32_memory.cpp")
  set_target_properties("win32_memory" PROPERTIES LINK_LIBRARIES "")
endif()

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

# Under Windows, we also include a resource file to the build
if(WIN32)
  list(APPEND SRC_SRCS ${CMAKE_CURRENT_BINARY_DIR}/win32/win32.rc)
endif()

# Generate the Anope executable and set it's linker flags, also set it to export it's symbols even though it's not a module
add_executable(${PROGRAM_NAME} ${SRC_SRCS})
set_target_properties(${PROGRAM_NAME} PROPERTIES
  BUILD_WITH_INSTALL_RPATH ON
  ENABLE_EXPORTS ON
  INSTALL_RPATH_USE_LINK_PATH ON
)
if(WIN32)
  set_target_properties(${PROGRAM_NAME} PROPERTIES VERSION "${VERSION_DOTTED}")
endif()

# Building the Anope executable requires the version.h header to be generated
add_dependencies(${PROGRAM_NAME} headers)

# Also require the language files if we have gettext
if(HAVE_LOCALIZATION)
  add_dependencies(${PROGRAM_NAME} language)
endif()

# Set Anope to be installed to the bin directory
install(TARGETS ${PROGRAM_NAME}
  DESTINATION ${BIN_DIR}
  RUNTIME
)

# Go into the following directories and run their CMakeLists.txt as well
if(NOT DISABLE_TOOLS)
  add_subdirectory(tools)
endif()
