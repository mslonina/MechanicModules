include_directories(.)
add_library (mechanic_module_arnoldweb SHARED mechanic_module_arnoldweb.c
  mechanic_module_arnoldweb_lib.c)
target_link_libraries (mechanic_module_arnoldweb mechanic m)
install (TARGETS mechanic_module_arnoldweb DESTINATION lib)
