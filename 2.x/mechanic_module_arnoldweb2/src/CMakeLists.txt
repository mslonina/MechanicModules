include_directories(.)
add_library (mechanic_module_aweb SHARED mechanic_module_aweb.c
  mechanic_module_aweb_lib.c)
target_link_libraries (mechanic_module_aweb mechanic2 readconfig m)
install (TARGETS mechanic_module_aweb DESTINATION lib${LIB_SUFFIX})
