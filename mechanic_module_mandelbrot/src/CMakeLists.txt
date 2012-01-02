include_directories(.)
add_library (mechanic_module_mandelbrot SHARED mechanic_module_mandelbrot.c)
target_link_libraries (mechanic_module_mandelbrot mechanic m)
install (TARGETS mechanic_module_mandelbrot DESTINATION lib)
