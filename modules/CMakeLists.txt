# If using Windows, add the MODULE_COMPILE define
if(WIN32)
  add_definitions(-DMODULE_COMPILE)
endif(WIN32)

macro(build_modules SRC)
  if(NOT ${SRC} STREQUAL ${CMAKE_CURRENT_SOURCE_DIR} AND EXISTS "${SRC}/CMakeLists.txt")
    add_subdirectory("${SRC}")
  else(NOT ${SRC} STREQUAL ${CMAKE_CURRENT_SOURCE_DIR} AND EXISTS "${SRC}/CMakeLists.txt")
    file(GLOB MODULES_SRCS "${SRC}/*")
    foreach(MODULE_SRC ${MODULES_SRCS})
      if(IS_DIRECTORY "${MODULE_SRC}")
        build_modules("${MODULE_SRC}")
      else(IS_DIRECTORY "${MODULE_SRC}")
        string(REGEX MATCH "\\.c$" ANOPE18MODULE ${MODULE_SRC})
        if(ANOPE18MODULE)
          message(FATAL_ERROR "Anope 1 modules are not compatible with Anope 2!\nOffending module: ${MODULE_SRC}")
        endif(ANOPE18MODULE)
        string(REGEX MATCH "\\.cpp$" CPP ${MODULE_SRC})
        if(CPP)
          set_source_files_properties(${MODULE_SRC} PROPERTIES LANGUAGE CXX COMPILE_FLAGS "${CXXFLAGS}")

          file(RELATIVE_PATH FNAME ${SRC} ${MODULE_SRC})
          # Convert the real source file extension to have a .so extension
          string(REGEX REPLACE "\\.cpp$" ".so" SO ${FNAME})
          # Temporary variable for the current source's include directories
          set(TEMP_INCLUDES)
          # Calculate the header file dependencies for the given source file
          calculate_depends(${MODULE_SRC} TEMP_INCLUDES)
          # If there were some extra include directories, add them to the list
          if(TEMP_INCLUDES)
            append_to_list(EXTRA_INCLUDES ${TEMP_INCLUDES})
          endif(TEMP_INCLUDES)

          # Reset linker flags
          set(TEMP_LDFLAGS)
          # Reset extra dependencies
          set(TEMP_DEPENDENCIES)
          # Calculate the library dependencies for the given source file
          calculate_libraries(${MODULE_SRC} TEMP_LDFLAGS TEMP_DEPENDENCIES)
          # Reset has_function
          set(HAS_FUNCTION)
          # Check the function dependencies for the given source file
          check_functions(${MODULE_SRC} HAS_FUNCTION)
          # Only continue if this module has all of the required functions
          if(HAS_FUNCTION)
            # For Visual Studio only, include win32_memory static library, required to override Visual Studio's overrides of the new/delete operators
            if(MSVC)
              set(WIN32_MEMORY win32_memory)
            else(MSVC)
              set(WIN32_MEMORY)
            endif(MSVC)
            # Generate the module and set its linker flags, also set it to depend on the main Anope executable to be built beforehand
            add_library(${SO} MODULE ${MODULE_SRC})
            # Windows requires this because it's weird
            if(WIN32)
              set(WIN32_NO_LIBS "/nodefaultlib:\"libcmt.lib\" /OPT:NOREF")
            else(WIN32)
              set(WIN32_NO_LIBS)
            endif(WIN32)
            set_target_properties(${SO} PROPERTIES LINKER_LANGUAGE CXX PREFIX "" SUFFIX "" LINK_FLAGS "${TEMP_LDFLAGS} ${WIN32_NO_LIBS}" INSTALL_RPATH_USE_LINK_PATH ON BUILD_WITH_INSTALL_RPATH ON)
            add_dependencies(${SO} ${PROGRAM_NAME})
            if(GETTEXT_FOUND)
              add_dependencies(${SO} module_language)
            endif(GETTEXT_FOUND)
            target_link_libraries(${SO} ${TEMP_DEPENDENCIES})
            # For Windows only, have the module link to the export library of Anope as well as wsock32 and Ws2_32 libraries (most of the modules probably don't need this, but this is to be on the safe side), also set its version
            if(WIN32)
              target_link_libraries(${SO} ${PROGRAM_NAME} wsock32 Ws2_32 ${WIN32_MEMORY})
              set_target_properties(${PROGRAM_NAME} PROPERTIES VERSION "${VERSION_DOTTED}")
            else(WIN32)
              if(APPLE)
                target_link_libraries(${SO} ${PROGRAM_NAME})
              endif(APPLE)
            endif(WIN32)
            # Set the module to be installed to the module directory under the data directory
            install(TARGETS ${SO} DESTINATION ${LIB_DIR}/modules)
          endif(HAS_FUNCTION)
        endif(CPP)
      endif(IS_DIRECTORY "${MODULE_SRC}")
    endforeach(MODULE_SRC ${MODULES_SRCS})
  endif(NOT ${SRC} STREQUAL ${CMAKE_CURRENT_SOURCE_DIR} AND EXISTS "${SRC}/CMakeLists.txt")
endmacro(build_modules)

macro(build_subdir)
  file(GLOB_RECURSE MODULES_SUBDIR_SRCS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "*.cpp")
  sort_list(MODULES_SUBDIR_SRCS)

  GET_FILENAME_COMPONENT(FOLDER_NAME ${CMAKE_CURRENT_SOURCE_DIR} NAME)
  set(SO "${FOLDER_NAME}.so")

  # Set all the files to use C++ as well as set their compile flags (use the module-specific compile flags, though)
  set_source_files_properties(${MODULES_SUBDIR_SRCS} PROPERTIES LANGUAGE CXX COMPILE_FLAGS "${CXXFLAGS}")

  set(HAS_FUNCTION TRUE)

  # Iterate through the source files in the subdirectory
  foreach(SRC ${MODULES_SUBDIR_SRCS})
    if(HAS_FUNCTION)
      # Temporary variable for the current source's include directories
      set(TEMP_INCLUDES)
      # Calculate the header file dependencies for the given source file
      calculate_depends(${SRC} TEMP_INCLUDES)
      # If there were some extra include directories, add them to the list
      if(TEMP_INCLUDES)
        include_directories(${TEMP_INCLUDES})
      endif(TEMP_INCLUDES)

      # Reset linker flags
      set(TEMP_LDFLAGS)
      # Reset extra dependencies
      set(TEMP_DEPENDENCIES)
      # Calculate the library dependencies for the given source file
      calculate_libraries(${SRC} SKIP_LIBRARIES MODULE TEMP_LDFLAGS TEMP_DEPENDENCIES)
      # Check the function dependencies for the given source file
      check_functions(${SRC} HAS_FUNCTION)

      # Append this source file's linker flags to the subdirectoy's linker flags, if there are any to append
      if(TEMP_DEPENDENCIES)
        append_to_list(SUBDIR_EXTRA_DEPENDS ${TEMP_DEPDENCIES})
      endif(TEMP_DEPENDENCIES)
    endif(HAS_FUNCTION)
  endforeach(SRC ${MODULES_SUBDIR_SRCS})

  # Continue if library and function requirements are met
  if(HAS_FUNCTION)
    # Remove duplicates from the linker flags
    if(SUBDIR_LDFLAGS)
      remove_list_duplicates(SUBDIR_LDFLAGS)
    endif(SUBDIR_LDFLAGS)

    # Remove duplicates from the extra dependencies
    if(SUBDIR_EXTRA_DEPENDS)
      remove_list_duplicates(SUBDIR_EXTRA_DEPENDS)
    endif(SUBDIR_EXTRA_DEPENDS)

    # For Visual Studio only, include win32_memory static library, required to override Visual Studio's overrides of the new/delete operators
    if(MSVC)
      set(WIN32_MEMORY win32_memory)
    else(MSVC)
      set(WIN32_MEMORY)
    endif(MSVC)

    # Generate the module and set it's linker flags, also set it to depend on the main Anope executable to be built beforehand
    add_library(${SO} MODULE ${MODULES_SUBDIR_SRCS})
    set_target_properties(${SO} PROPERTIES LINKER_LANGUAGE CXX PREFIX "" SUFFIX "" LINK_FLAGS "${SUBDIR_LDFLAGS}" INSTALL_RPATH_USE_LINK_PATH ON BUILD_WITH_INSTALL_RPATH ON)
    add_dependencies(${SO} ${PROGRAM_NAME})
    if(GETTEXT_FOUND)
      add_dependencies(${SO} module_language)
    endif(GETTEXT_FOUND)
    target_link_libraries(${SO} ${SUBDIR_EXTRA_DEPENDS})
    # For Windows only, have the module link to the export library of Anope as well as wsock32 and Ws2_32 libraries (most of the modules probably don't need this, but this is to be on the safe side), also set it's version
    if(WIN32)
      target_link_libraries(${SO} ${PROGRAM_NAME} wsock32 Ws2_32 ${WIN32_MEMORY})
      set_target_properties(${PROGRAM_NAME} PROPERTIES VERSION "${VERSION_DOTTED}")
    else(WIN32)
      if(APPLE)
        target_link_libraries(${SO} ${PROGRAM_NAME})
      endif(APPLE)
    endif(WIN32)

    # Set the module to be installed to the module directory under the data directory
    install(TARGETS ${SO} DESTINATION ${LIB_DIR}/modules)

  endif(HAS_FUNCTION)
endmacro(build_subdir)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
build_modules(${CMAKE_CURRENT_SOURCE_DIR})

