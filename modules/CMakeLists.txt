# If using Windows, add the MODULE_COMPILE define
if(WIN32)
  add_definitions(-DMODULE_COMPILE)
endif()

# enable extra modules if conan is used
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/../conanbuildinfo.cmake")
  function(enable_extra NAME PACKAGE)
    if(DEFINED "CONAN_${PACKAGE}_ROOT")
      message("Enabling the ${NAME} module")
      # copy the modules out of extra so it gets picked up by build_modules
      file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/extra/${NAME}.cpp" DESTINATION "${CMAKE_CURRENT_SOURCE_DIR}")
    else()
      message("Unable to enable the ${NAME} module (missing library)")
    endif()
  endfunction()

  function(copy_extra NAME)
    file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/extra/${NAME}.cpp" DESTINATION "${CMAKE_CURRENT_SOURCE_DIR}")
  endfunction()

  enable_extra("enc_argon2" "ARGON2")
  enable_extra("mysql" "LIBMYSQLCLIENT")
  enable_extra("regex_pcre2" "PCRE2")
  enable_extra("sqlite" "SQLITE3")
  enable_extra("ssl_openssl" "OPENSSL")
  # this uses Wldap so should always be available
  copy_extra("ldap")

  # Package extra dlls
  file(GLOB EXTRA_DLLS "${Anope_SOURCE_DIR}/extradll/bin/*.dll" "${Anope_SOURCE_DIR}/extradll/lib/*.dll")
  install(FILES ${EXTRA_DLLS} DESTINATION ${BIN_DIR})
endif()

macro(build_modules SRC)
  if(NOT ${SRC} STREQUAL ${CMAKE_CURRENT_SOURCE_DIR} AND EXISTS "${SRC}/CMakeLists.txt")
    add_subdirectory("${SRC}")
  else()
    file(GLOB MODULES_SRCS "${SRC}/*")
    foreach(MODULE_SRC ${MODULES_SRCS})
      if(IS_DIRECTORY "${MODULE_SRC}")
        build_modules("${MODULE_SRC}")
      else()
        string(REGEX MATCH "\\.c$" ANOPE18MODULE ${MODULE_SRC})
        if(ANOPE18MODULE)
          message(FATAL_ERROR "Anope 1 modules are not compatible with Anope 2!\nOffending module: ${MODULE_SRC}")
        endif()
        string(REGEX MATCH "\\.cpp$" CPP ${MODULE_SRC})
        if(CPP)
          set_source_files_properties(${MODULE_SRC} PROPERTIES LANGUAGE CXX COMPILE_FLAGS "${CXXFLAGS}")

          file(RELATIVE_PATH FNAME ${SRC} ${MODULE_SRC})
          # Convert the real source file extension to have a library extension
          string(REGEX REPLACE "\\.cpp$" "${CMAKE_SHARED_LIBRARY_SUFFIX}" SO ${FNAME})
          # Reset linker flags
          set(TEMP_LDFLAGS)
          # Reset extra dependencies
          set(TEMP_DEPENDENCIES)
          # Calculate the library dependencies for the given source file
          calculate_libraries(${MODULE_SRC} TEMP_LDFLAGS TEMP_DEPENDENCIES)
          # For Visual Studio only, include win32_memory static library, required to override Visual Studio's overrides of the new/delete operators
          if(MSVC)
            set(WIN32_MEMORY win32_memory)
          else()
            set(WIN32_MEMORY)
          endif()
          # Generate the module and set its linker flags, also set it to depend on the main Anope executable to be built beforehand
          add_library(${SO} MODULE ${MODULE_SRC})
          # Windows requires this because it's weird
          if(WIN32)
            set(WIN32_NO_LIBS "/nodefaultlib:\"libcmt.lib\" /OPT:NOREF")
          else()
            set(WIN32_NO_LIBS)
          endif()
          set_target_properties(${SO} PROPERTIES
            BUILD_WITH_INSTALL_RPATH ON
            FOLDER "Modules"
            INSTALL_RPATH_USE_LINK_PATH ON
            LINKER_LANGUAGE CXX
            LINK_FLAGS "${TEMP_LDFLAGS} ${WIN32_NO_LIBS}"
            PREFIX ""
            SUFFIX ""
          )
          add_dependencies(${SO} ${PROGRAM_NAME})
          if(HAVE_LOCALIZATION)
            add_dependencies(${SO} module_language)
          endif()
          target_link_libraries(${SO} ${TEMP_DEPENDENCIES})
          # For Windows only, have the module link to the export library of Anope as well as wsock32 and Ws2_32 libraries (most of the modules probably don't need this, but this is to be on the safe side), also set its version
          if(WIN32)
            target_link_libraries(${SO} ${PROGRAM_NAME} wsock32 Ws2_32 ${WIN32_MEMORY})
            set_target_properties(${PROGRAM_NAME} PROPERTIES VERSION "${VERSION_DOTTED}")
          elseif(APPLE)
            target_link_libraries(${SO} ${PROGRAM_NAME})
          endif()
          # Set the module to be installed to the module directory under the data directory
          install(TARGETS ${SO}
            DESTINATION ${MODULE_DIR}
            LIBRARY)
        endif()
      endif()
    endforeach()
  endif()
endmacro()

macro(build_subdir)
  file(GLOB_RECURSE MODULES_SUBDIR_SRCS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "*.cpp")
  list(SORT MODULES_SUBDIR_SRCS)

  GET_FILENAME_COMPONENT(FOLDER_NAME ${CMAKE_CURRENT_SOURCE_DIR} NAME)
  set(SO "${FOLDER_NAME}${CMAKE_SHARED_LIBRARY_SUFFIX}")

  # Set all the files to use C++ as well as set their compile flags (use the module-specific compile flags, though)
  set_source_files_properties(${MODULES_SUBDIR_SRCS} PROPERTIES LANGUAGE CXX COMPILE_FLAGS "${CXXFLAGS}")

  # Iterate through the source files in the subdirectory
  foreach(SRC ${MODULES_SUBDIR_SRCS})
    # Reset linker flags
    set(TEMP_LDFLAGS)
    # Reset extra dependencies
    set(TEMP_DEPENDENCIES)
    # Calculate the library dependencies for the given source file
    calculate_libraries(${SRC} SKIP_LIBRARIES MODULE TEMP_LDFLAGS TEMP_DEPENDENCIES)

    # Append this source file's linker flags to the subdirectoy's linker flags, if there are any to append
    if(TEMP_DEPENDENCIES)
      list(APPEND SUBDIR_EXTRA_DEPENDS ${TEMP_DEPENDENCIES})
    endif()
  endforeach()

  # Remove duplicates from the linker flags
  if(SUBDIR_LDFLAGS)
    list(REMOVE_DUPLICATES SUBDIR_LDFLAGS)
  endif()

  # Remove duplicates from the extra dependencies
  if(SUBDIR_EXTRA_DEPENDS)
    list(REMOVE_DUPLICATES SUBDIR_EXTRA_DEPENDS)
  endif()

  # For Visual Studio only, include win32_memory static library, required to override Visual Studio's overrides of the new/delete operators
  if(MSVC)
    set(WIN32_MEMORY win32_memory)
  else()
    set(WIN32_MEMORY)
  endif()

  # Generate the module and set it's linker flags, also set it to depend on the main Anope executable to be built beforehand
  add_library(${SO} MODULE ${MODULES_SUBDIR_SRCS})
  set_target_properties(${SO} PROPERTIES
    BUILD_WITH_INSTALL_RPATH ON
    FOLDER "Modules"
    INSTALL_RPATH_USE_LINK_PATH ON
    LINKER_LANGUAGE CXX
    LINK_FLAGS "${SUBDIR_LDFLAGS}"
    PREFIX ""
    SUFFIX ""
  )
  add_dependencies(${SO} ${PROGRAM_NAME})
  if(HAVE_LOCALIZATION)
    add_dependencies(${SO} module_language)
  endif()
  target_link_libraries(${SO} ${SUBDIR_EXTRA_DEPENDS})
  # For Windows only, have the module link to the export library of Anope as well as wsock32 and Ws2_32 libraries (most of the modules probably don't need this, but this is to be on the safe side), also set it's version
  if(WIN32)
    target_link_libraries(${SO} ${PROGRAM_NAME} wsock32 Ws2_32 ${WIN32_MEMORY})
    set_target_properties(${PROGRAM_NAME} PROPERTIES VERSION "${VERSION_DOTTED}")
  elseif(APPLE)
    target_link_libraries(${SO} ${PROGRAM_NAME})
  endif()

  # Set the module to be installed to the module directory under the data directory
  install(TARGETS ${SO}
    DESTINATION ${MODULE_DIR}
    LIBRARY)
endmacro()

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
build_modules(${CMAKE_CURRENT_SOURCE_DIR})
