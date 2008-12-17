cmake_minimum_required(VERSION 2.6)

#
# If the user specifies -DCMAKE_BUILD_TYPE on the command line, take their definition
# and dump it in the cache along with proper documentation, otherwise set CMAKE_BUILD_TYPE
# to Debug prior to calling PROJECT()
#
IF(CMAKE_BUILD_TYPE)
  SET(CMAKE_BUILD_TYPE ${CMAKE_BUILD_TYPE} CACHE STRING "Choose the type of build, options are: None(CMAKE_CXX_FLAGS or CMAKE_C_FLAGS used) Debug Release RelWithDebInfo MinSizeRel.")
ELSE(CMAKE_BUILD_TYPE)
  SET(CMAKE_BUILD_TYPE DEBUG CACHE STRING "Choose the type of build, options are: None(CMAKE_CXX_FLAGS or CMAKE_C_FLAGS used) Debug Release RelWithDebInfo MinSizeRel.")
ENDIF(CMAKE_BUILD_TYPE)

project(Anope CXX)
enable_language(C)

include(CheckFunctionExists)
include(CheckIncludeFile)
include(CheckTypeSize)
include(CheckCXXCompilerFlag)
include(CheckLibraryExists)

option(USE_RUN_CC_PL "Use run-cc.pl for building" OFF)

set(CXXFLAGS "${CXXFLAGS} ${CMAKE_CXX_CFLAGS} -Wall -ansi -pedantic -Wshadow -fno-leading-underscore -export-dynamic -I${Anope_SOURCE_DIR}/include -I${Anope_BINARY_DIR}/include -I${Anope_BINARY_DIR}/lang")
#message(STATUS "CMAKE_BUILD_TYPE: ${CMAKE_BUILD_TYPE}")
if(CMAKE_BUILD_TYPE)
  set(CXXFLAGS "${CXXFLAGS} ${CMAKE_CXX_FLAGS_${CMAKE_BUILD_TYPE}}")
endif(CMAKE_BUILD_TYPE)

if(CMAKE_DL_LIBS)
  set(LDFLAGS "${LDFLAGS} ${CMAKE_DL_LIBS}")
endif(CMAKE_DL_LIBS)

#set(MY_COMPILER ${CMAKE_CXX_COMPILER})
#set(MY_COMP_ARG)
set(CMAKE_CXX_LINK_EXECUTABLE "<CMAKE_CXX_COMPILER> ${CXXFLAGS} <CMAKE_CXX_LINK_FLAGS> <LINK_FLAGS> <OBJECTS> -o <TARGET> ${LDFLAGS}")
set(CMAKE_CXX_CREATE_SHARED_MODULE "<CMAKE_CXX_COMPILER> ${CXXFLAGS} <CMAKE_SHARED_LIBRARY_CXX_FLAGS> <LANGUAGE_COMPILE_FLAGS> <LINK_FLAGS> <CMAKE_SHARED_LIBRARY_CREATE_CXX_FLAGS> <CMAKE_SHARED_LIBRARY_SONAME_CXX_FLAG><TARGET_SONAME> -o <TARGET> <OBJECTS> ${LDFLAGS}")

check_cxx_compiler_flag(-pipe HAVE_PIPE_FLAG)
if(HAVE_PIPE_FLAG)
  set(CXXFLAGS "${CXXFLAGS} -pipe")
endif(HAVE_PIPE_FLAG)

check_library_exists(nsl inet_ntoa "" HAVE_NSL_LIB)
if(HAVE_NSL_LIB)
  set(LDFLAGS "${LDFLAGS} -lnsl")
endif(HAVE_NSL_LIB)
check_library_exists(socket socket "" HAVE_SOCKET_LIB)
if(HAVE_SOCKET_LIB)
  set(LDFLAGS "${LDFLAGS} -lsocket")
endif(HAVE_SOCKET_LIB)
check_library_exists(resolv res_query "" HAVE_RESOLV_LIB)
if(HAVE_RESOLV_LIB)
  set(LDFLAGS "${LDFLAGS} -lresolv")
endif(HAVE_RESOLV_LIB)
check_library_exists(bsd revoke "" HAVE_BSD_LIB)
if(HAVE_BSD_LIB)
  set(LDFLAGS "${LDFLAGS} -lbsd")
endif(HAVE_BSD_LIB)

try_run(RUN_VA_LIST_AS_ARRAY COMPILE_VA_LIST_AS_ARRAY
  ${Anope_SOURCE_DIR} ${Anope_SOURCE_DIR}/va_list_check.c
)
if(COMPILE_VA_LIST_AS_ARRAY AND NOT RUN_VA_LIST_AS_ARRAY)
  set(HAVE_VA_LIST_AS_ARRAY 1)
endif(COMPILE_VA_LIST_AS_ARRAY AND NOT RUN_VA_LIST_AS_ARRAY)

if(NOT DEFUMASK)
  if(RUNGROUP)
    set(DEFUMASK "007")
  else(RUNGROUP)
    set(DEFUMASK "077")
  endif(RUNGROUP)
endif(NOT DEFUMASK)
check_include_file(sys/types.h HAVE_SYS_TYPES_H)
check_include_file(execinfo.h HAVE_BACKTRACE)
check_function_exists(gethostbyname HAVE_GETHOSTBYNAME)
check_function_exists(gettimeofday HAVE_GETTIMEOFDAY)
check_function_exists(setgrent HAVE_SETGRENT)
check_function_exists(strcasecmp HAVE_STRCASECMP)
check_function_exists(stricmp HAVE_STRICMP)
check_include_file(strings.h HAVE_STRINGS_H)
check_function_exists(strlcat HAVE_STRLCAT)
check_function_exists(strlcpy HAVE_STRLCPY)
check_include_file(sys/select.h HAVE_SYS_SELECT_H)
check_function_exists(umask HAVE_UMASK)
check_type_size(int16_t INT16_T)
check_type_size(uint16_t UINT16_T)
check_type_size(u_int16_t U_INT16_T)
check_type_size(int32_t INT32_T)
check_type_size(uint32_t UINT32_T)
check_type_size(u_int32_t U_INT32_T)

if(BINDIR)
  set(SERVICES_BIN "${BINDIR}/services")
else(BINDIR)
  set(SERVICES_BIN "$ENV{HOME}/services/services")
  set(BINDIR "$ENV{HOME}/services")
endif(BINDIR)

if(DATADIR)
  set(SERVICES_DIR "${DATADIR}")
  #set(MODULE_PATH "${DATADIR}/modules/")
else(DATADIR)
  set(SERVICES_DIR "$ENV{HOME}/services")
  #set(MODULE_PATH "$ENV{HOME}/services/modules/")
  set(DATADIR "$ENV{HOME}/services")
endif(DATADIR)

configure_file(${Anope_SOURCE_DIR}/include/sysconf.h.cmake ${Anope_SOURCE_DIR}/include/sysconf.h)
configure_file(${Anope_SOURCE_DIR}/src/bin/anoperc.cmake ${Anope_SOURCE_DIR}/src/bin/anoperc)

if(CXXFLAGS)
  string(STRIP ${CXXFLAGS} CXXFLAGS)
endif(CXXFLAGS)
if(LDFLAGS)
  string(STRIP ${LDFLAGS} LDFLAGS)
endif(LDFLAGS)

find_program(GREP grep)
find_program(SH sh)
find_program(CHGRP chgrp)
find_program(CHMOD chmod)
#message(STATUS "grep: ${GREP}")
find_program(PERL perl)
if(PERL)
  if(USE_RUN_CC_PL)
    #set(MY_COMPILER ${Anope_SOURCE_DIR}/run-cc.pl)
    #set(MY_COMP_ARG ${CMAKE_CXX_COMPILER})
    set(CMAKE_CXX_COMPILE_OBJECT "${Anope_SOURCE_DIR}/run-cc.pl <CMAKE_CXX_COMPILER> <DEFINES> <FLAGS> -o <OBJECT> -c <SOURCE>")
    set(CMAKE_CXX_LINK_EXECUTABLE "${Anope_SOURCE_DIR}/run-cc.pl <CMAKE_CXX_COMPILER> ${CXXFLAGS} <CMAKE_CXX_LINK_FLAGS> <LINK_FLAGS> <OBJECTS> -o <TARGET> ${LDFLAGS}")
    set(CMAKE_CXX_CREATE_SHARED_MODULE "${Anope_SOURCE_DIR}/run-cc.pl <CMAKE_CXX_COMPILER> ${CXXFLAGS} <CMAKE_SHARED_LIBRARY_CXX_FLAGS> <LANGUAGE_COMPILE_FLAGS> <LINK_FLAGS> <CMAKE_SHARED_LIBRARY_CREATE_CXX_FLAGS> <CMAKE_SHARED_LIBRARY_SONAME_CXX_FLAG><TARGET_SONAME> -o <TARGET> <OBJECTS> ${LDFLAGS}")
  endif(USE_RUN_CC_PL)
endif(PERL)

#set(CMAKE_BUILD_TYPE DEBUG)
#message(STATUS "CMAKE_CXX_FLAGS_${CMAKE_BUILD_TYPE}: ${CMAKE_CXX_FLAGS_${CMAKE_BUILD_TYPE}}")
#message(STATUS "CMAKE_CXX_COMPILE_OBJECT: ${CMAKE_CXX_COMPILE_OBJECT}")
#set(CMAKE_CXX_COMPILE_OBJECT "${Anope_SOURCE_DIR}/run-cc.pl <CMAKE_CXX_COMPILER> <DEFINES> <FLAGS> -o <OBJECT> -c <SOURCE>")
#message(STATUS "CMAKE_CXX_COMPILE_OBJECT: ${CMAKE_CXX_COMPILE_OBJECT}")
# CMAKE_CXX_COMPILE_OBJECT: <CMAKE_CXX_COMPILER> <DEFINES> <FLAGS> -o <OBJECT> -c <SOURCE>
# CMAKE_CXX_CREATE_SHARED_LIBRARY: <CMAKE_CXX_COMPILER> <CMAKE_SHARED_LIBRARY_CXX_FLAGS> <LANGUAGE_COMPILE_FLAGS> <LINK_FLAGS> <CMAKE_SHARED_LIBRARY_CREATE_CXX_FLAGS> <CMAKE_SHARED_LIBRARY_SONAME_CXX_FLAG><TARGET_SONAME> -o <TARGET> <OBJECTS> <LINK_LIBRARIES>
# CMAKE_CXX_CREATE_SHARED_MODULE: <CMAKE_CXX_COMPILER> <CMAKE_SHARED_LIBRARY_CXX_FLAGS> <LANGUAGE_COMPILE_FLAGS> <LINK_FLAGS> <CMAKE_SHARED_LIBRARY_CREATE_CXX_FLAGS> <CMAKE_SHARED_LIBRARY_SONAME_CXX_FLAG><TARGET_SONAME> -o <TARGET> <OBJECTS> <LINK_LIBRARIES>
# CMAKE_CXX_FLAGS_DEBUG: -g
# CMAKE_CXX_FLAGS_RELEASE: -O3 -DNDEBUG
# CMAKE_CXX_LINK_EXECUTABLE: <CMAKE_CXX_COMPILER> <FLAGS> <CMAKE_CXX_LINK_FLAGS> <LINK_FLAGS> <OBJECTS> -o <TARGET> <LINK_LIBRARIES>
# CMAKE_CXX_OUTPUT_EXTENSION: .o
# CMAKE_SHARED_LIBRARY_CXX_FLAGS: -fPIC
# CMAKE_SHARED_LIBRARY_CREATE_CXX_FLAGS: -shared
# CMAKE_SHARED_LIBRARY_SONAME_CXX_FLAG: -Wl,-soname,

add_subdirectory(data)
add_subdirectory(lang)
add_subdirectory(src)
add_subdirectory(include)

install(CODE "file(MAKE_DIRECTORY \"${DATADIR}/backups\")")
install(CODE "file(MAKE_DIRECTORY \"${DATADIR}/logs\")")
install(CODE "file(MAKE_DIRECTORY \"${DATADIR}/modules/runtime\")")
if(RUNGROUP)
  install(CODE "execute_process(COMMAND ${CHMOD} 2775 \"${DATADIR}/backups\")")
  install(CODE "execute_process(COMMAND ${CHMOD} 2775 \"${DATADIR}/logs\")")
  install(CODE "execute_process(COMMAND ${CHMOD} 2775 \"${DATADIR}/modules/runtime\")")
  install(CODE "execute_process(COMMAND ${CHGRP} -R ${RUNGROUP} \"${DATADIR}\")")
endif(RUNGROUP)
