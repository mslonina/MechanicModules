The Hello module
================

This is just a simple example of the Mechanic API.

Compilation
-----------

  mkdir build
  cd build
  CC=mpicc cmake ..
  make

Usage
-----

  mpirun -np 4 mechanic -p hello

Note
----
  
You have to adjust your LD_LIBRARY_PATH to point to the location of the compiled module or install it system wide.
