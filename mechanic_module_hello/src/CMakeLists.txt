include_directories(.)
add_library (mechanic_module_hello SHARED mechanic_module_hello.c)
target_link_libraries (mechanic_module_hello mechanic)
install (TARGETS mechanic_module_hello DESTINATION lib${LIB_SUFFIX})
