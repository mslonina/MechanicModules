The Arnold Web module
---------------------

The sample Mechanic module to compute the Arnold Web. 

Requirements
------------

All requirements should be already met after Mechanic installation:

- CMAKE >= 2.8
- GCC >= 4.6 or equivalent
- MPI2 implementation (we favour OpenMPI)

Compiling the Arnold Web module
-------------------------------

>  mkdir build
>  cd build
>  CC=mpicc cmake .. -DCMAKE_INSTALL_PREFIX:PATH=/usr/local
>  make
>  sudo make install
