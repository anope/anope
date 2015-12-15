# If using Windows, add the MODULE_COMPILE define
if(WIN32)
  add_definitions(-DMODULE_COMPILE)
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
        string(REGEX MATCH "\\.cpp$" CPP ${MODULE_SRC})
        if(CPP)
          set_source_files_properties(${MODULE_SRC} PROPERTIES LANGUAGE CXX COMPILE_FLAGS "${CXXFLAGS}")

          file(RELATIVE_PATH FNAME ${SRC} ${MODULE_SRC})
          # Convert the real source file extension to have a .so extension
          string(REGEX REPLACE "\\.cpp$" "" SO ${FNAME})

          file(RELATIVE_PATH DEST "${Anope_SOURCE_DIR}/modules" ${SRC})
          if(DEST)
            set(TARG "module_${DEST}.${SO}")
          else()
            set(TARG "module_${SO}")
          endif()

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
          add_library(${TARG} MODULE ${MODULE_SRC})
          # Windows requires this because it's weird
          if(WIN32)
            set(WIN32_NO_LIBS "/nodefaultlib:\"libcmt.lib\" /OPT:NOREF")
          else()
            set(WIN32_NO_LIBS)
          endif()
          set_target_properties(${TARG} PROPERTIES LINKER_LANGUAGE CXX PREFIX "" LINK_FLAGS "${TEMP_LDFLAGS} ${WIN32_NO_LIBS}" INSTALL_RPATH_USE_LINK_PATH ON BUILD_WITH_INSTALL_RPATH ON OUTPUT_NAME "${SO}")
          add_dependencies(${TARG} ${PROGRAM_NAME})
          if(GETTEXT_FOUND)
                  add_dependencies(${TARG} module_language)
          endif()
          target_link_libraries(${TARG} ${TEMP_DEPENDENCIES})
          # For Windows only, have the module link to the export library of Anope as well as wsock32 and Ws2_32 libraries (most of the modules probably don't need this, but this is to be on the safe side), also set its version
          if(WIN32)
            target_link_libraries(${TARG} ${PROGRAM_NAME} wsock32 Ws2_32 ${WIN32_MEMORY})
            set_target_properties(${PROGRAM_NAME} PROPERTIES VERSION "${VERSION_DOTTED}")
          else()
            if(APPLE)
              target_link_libraries(${TARG} ${PROGRAM_NAME})
            endif()
          endif()
          calculate_dependencies(${MODULE_SRC})
          # Set the module to be installed to the module directory under the data directory
          install(TARGETS ${TARG} DESTINATION ${LIB_DIR}/modules/${DEST})
        endif()
      endif()
    endforeach()
  endif()
endmacro()

macro(build_subdir)
  file(GLOB_RECURSE MODULES_SUBDIR_SRCS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "*.cpp")
  list(SORT MODULES_SUBDIR_SRCS)

  file(RELATIVE_PATH DEST "${Anope_SOURCE_DIR}/modules" ${CMAKE_CURRENT_SOURCE_DIR})
  get_filename_component(DIR_NAME ${DEST} DIRECTORY)
  get_filename_component(FOLDER_NAME ${DEST} NAME)
  if(DIR_NAME)
    set(SO "${DIR_NAME}.${FOLDER_NAME}")
  else()
    set(SO "${FOLDER_NAME}")
  endif()

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
    calculate_dependencies(${SRC})

    # Append this source file's linker flags to the subdirectoy's linker flags, if there are any to append
    if(TEMP_DEPENDENCIES)
      append_to_list(SUBDIR_EXTRA_DEPENDS ${TEMP_DEPDENCIES})
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

  # Generate the module and set its linker flags, also set it to depend on the main Anope executable to be built beforehand
  add_library(${SO} MODULE ${MODULES_SUBDIR_SRCS})
  set_target_properties(${SO} PROPERTIES LINKER_LANGUAGE CXX PREFIX "" LINK_FLAGS "${SUBDIR_LDFLAGS}" INSTALL_RPATH_USE_LINK_PATH ON BUILD_WITH_INSTALL_RPATH ON OUTPUT_NAME "${FOLDER_NAME}")
  add_dependencies(${SO} ${PROGRAM_NAME})
  if(GETTEXT_FOUND)
    add_dependencies(${SO} module_language)
  endif()
  target_link_libraries(${SO} ${SUBDIR_EXTRA_DEPENDS})
  # For Windows only, have the module link to the export library of Anope as well as wsock32 and Ws2_32 libraries (most of the modules probably don't need this, but this is to be on the safe side), also set its version
  if(WIN32)
    target_link_libraries(${SO} ${PROGRAM_NAME} wsock32 Ws2_32 ${WIN32_MEMORY})
    set_target_properties(${PROGRAM_NAME} PROPERTIES VERSION "${VERSION_DOTTED}")
  else()
    if(APPLE)
      target_link_libraries(${SO} ${PROGRAM_NAME})
    endif()
  endif()

  install(TARGETS ${SO} DESTINATION ${LIB_DIR}/modules/${DIR_NAME})
endmacro()

build_modules(${CMAKE_CURRENT_SOURCE_DIR})

