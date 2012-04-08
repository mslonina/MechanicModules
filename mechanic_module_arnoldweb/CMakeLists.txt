cmake_minimum_required (VERSION 2.8)
project (mechanic_module_arnoldweb)

option (LRC "Build with LRC configuration file support" off)

include (CheckIncludeFiles)
include (CheckLibraryExists)

CHECK_INCLUDE_FILES (mechanic.h HAVE_MECHANIC_H)
CHECK_LIBRARY_EXISTS (mechanic mechanic_message "" MECHANIC_LIB)

CONFIGURE_FILE (
  ${CMAKE_CURRENT_SOURCE_DIR}/src/config.h.in
  ${CMAKE_CURRENT_BINARY_DIR}/src/config.h
)

set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c99 -lreadconfig")

if (LRC) 
  add_definitions (-DLRC)
  set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -lreadconfig")
endif (LRC)

add_subdirectory (src)

SET (CPACK_PACKAGE_DESCRIPTION_SUMMARY "The Arnold Web module for Mechanic")
SET (CPACK_PACKAGE_VENDOR "Celestial Mechanics Group, Torun Centre for Astronomy (NCU)")
SET (CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE.txt")
SET (CPACK_RESOURCE_FILE_README "${CMAKE_CURRENT_SOURCE_DIR}/README.txt")
SET (CPACK_PACKAGE_VERSION_MAJOR "0")
SET (CPACK_PACKAGE_VERSION_MINOR "12")
SET (CPACK_PACKAGE_VERSION_PATCH "4")
SET (CPACK_PACKAGE_INSTALL_DIRECTORY "/usr/local")
SET (CPACK_SOURCE_PACKAGE_FILE_NAME
  "${CMAKE_PROJECT_NAME}-${CPACK_PACKAGE_VERSION_MAJOR}.${CPACK_PACKAGE_VERSION_MINOR}.${CPACK_PACKAGE_VERSION_PATCH}")
SET (CPACK_SOURCE_IGNORE_FILES
  "/build/;/.git/;*.h5;*eps;/tests;${CPACK_SOURCE_IGNORE_FILES}")

INCLUDE(CPack)

add_custom_target(dist COMMAND ${CMAKE_MAKE_PROGRAM} package_source)
