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

macro(inline_cmake TARGET FILE)
  file(STRINGS ${FILE} SRC)
  set(CODE "")
  set(IN_CODE OFF)
  foreach(LINE IN LISTS SRC)
    if(IN_CODE)
      string(REGEX REPLACE "^/// " "" CLEAN_LINE ${LINE})
      if(CLEAN_LINE MATCHES "^END CMAKE$")
        cmake_language(EVAL CODE "${CODE}")
        set(CODE "")
        set(IN_CODE OFF)
      else()
        set(CODE "${CODE}\n${CLEAN_LINE}")
      endif()
    elseif(LINE MATCHES "^/// BEGIN CMAKE$")
        message(STATUS "Executing inline CMake code for ${TARGET}")
        set(IN_CODE ON)
    endif()
  endforeach()
endmacro()

macro(build_module SRC MODULE_SRC)
  string(REGEX MATCH "\\.c$" ANOPE18MODULE ${MODULE_SRC})
  if(ANOPE18MODULE)
    message(FATAL_ERROR "Anope 1 modules are not compatible with Anope 2!\nOffending module: ${MODULE_SRC}")
  endif()
  string(REGEX MATCH "\\.cpp$" CPP ${MODULE_SRC})
  if(CPP)
    file(RELATIVE_PATH FNAME ${SRC} ${MODULE_SRC})
    # Convert the real source file extension to have a library extension
    string(REGEX REPLACE "\\.cpp$" "${CMAKE_SHARED_LIBRARY_SUFFIX}" SO ${FNAME})
    # Generate the module and set its linker flags, also set it to depend on the main Anope executable to be built beforehand
    add_library(${SO} MODULE ${MODULE_SRC})
    # Execute inline CMake code for the module
    inline_cmake(${SO} ${MODULE_SRC})
    set_target_properties(${SO} PROPERTIES
      BUILD_WITH_INSTALL_RPATH ON
      FOLDER "Modules"
      INSTALL_RPATH_USE_LINK_PATH ON
      PREFIX ""
      SUFFIX ""
    )
    add_dependencies(${SO} ${PROGRAM_NAME})
    if(HAVE_LOCALIZATION)
      add_dependencies(${SO} module_language)
    endif()
    # For Windows only, have the module link to the export library of Anope.
    if(WIN32)
      target_link_libraries(${SO} PUBLIC ${PROGRAM_NAME})
      set_target_properties(${PROGRAM_NAME} PROPERTIES VERSION "${VERSION_DOTTED}")
    elseif(APPLE)
      target_link_libraries(${SO} PUBLIC ${PROGRAM_NAME})
    endif()
    # Set the module to be installed to the module directory under the data directory
    install(TARGETS ${SO}
      DESTINATION ${MODULE_DIR}
      LIBRARY)
  endif()
endmacro()

macro(build_modules SRC)
  if(NOT ${SRC} STREQUAL ${CMAKE_CURRENT_SOURCE_DIR} AND EXISTS "${SRC}/CMakeLists.txt")
    add_subdirectory("${SRC}")
  else()
    file(GLOB MODULES_SRCS CONFIGURE_DEPENDS "${SRC}/*")
    foreach(MODULE_SRC ${MODULES_SRCS})
      if(IS_DIRECTORY "${MODULE_SRC}")
        build_modules("${MODULE_SRC}")
      else()
        build_module("${SRC}" "${MODULE_SRC}")
      endif()
    endforeach()
  endif()
endmacro()

macro(build_subdir)
  file(GLOB_RECURSE MODULES_SUBDIR_SRCS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} CONFIGURE_DEPENDS "*.cpp")
  list(SORT MODULES_SUBDIR_SRCS)

  cmake_path(GET CMAKE_CURRENT_SOURCE_DIR FILENAME FOLDER_NAME)
  set(SO "${FOLDER_NAME}${CMAKE_SHARED_LIBRARY_SUFFIX}")

  # Generate the module and set it's linker flags, also set it to depend on the main Anope executable to be built beforehand
  add_library(${SO} MODULE ${MODULES_SUBDIR_SRCS})
  set_target_properties(${SO} PROPERTIES
    BUILD_WITH_INSTALL_RPATH ON
    FOLDER "Modules"
    INSTALL_RPATH_USE_LINK_PATH ON
    PREFIX ""
    SUFFIX ""
  )
  add_dependencies(${SO} ${PROGRAM_NAME})
  if(HAVE_LOCALIZATION)
    add_dependencies(${SO} module_language)
  endif()
  # For Windows only, have the module link to the export library of Anope .
  if(WIN32)
    target_link_libraries(${SO} PUBLIC ${PROGRAM_NAME})
    set_target_properties(${SO} PROPERTIES VERSION "${VERSION_DOTTED}")
  elseif(APPLE)
    target_link_libraries(${SO} PUBLIC ${PROGRAM_NAME})
  endif()

  # Set the module to be installed to the module directory under the data directory
  install(TARGETS ${SO}
    DESTINATION ${MODULE_DIR}
    LIBRARY)
endmacro()

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
build_modules(${CMAKE_CURRENT_SOURCE_DIR})
