cmake_minimum_required (VERSION 2.8)
project (mechanic_module_mandelbrot)

include (CheckIncludeFiles)
include (CheckLibraryExists)

CHECK_INCLUDE_FILES (mechanic.h HAVE_MECHANIC_H)
CHECK_LIBRARY_EXISTS (mechanic mechanic_message "" MECHANIC_LIB)

add_subdirectory (src)

SET (CPACK_PACKAGE_DESCRIPTION_SUMMARY "The Mandelbrot Set for Mechanic")
SET (CPACK_PACKAGE_VENDOR "Mariusz Slonina (NCU)")
SET (CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE.txt")
SET (CPACK_RESOURCE_FILE_README "${CMAKE_CURRENT_SOURCE_DIR}/README.txt")
SET (CPACK_PACKAGE_VERSION_MAJOR "0")
SET (CPACK_PACKAGE_VERSION_MINOR "1")
SET (CPACK_PACKAGE_VERSION_PATCH "0")
SET (CPACK_PACKAGE_INSTALL_DIRECTORY "/usr/local")
SET (CPACK_SOURCE_IGNORE_FILES
  "/build/;/.git/;~$;${CPACK_SOURCE_IGNORE_FILES}")

INCLUDE(CPack)

add_custom_target(dist COMMAND ${CMAKE_MAKE_PROGRAM} package_source)
