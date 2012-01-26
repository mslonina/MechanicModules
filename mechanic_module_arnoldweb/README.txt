The Arnold Web module
=====================

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

The Mechanic can handle basic module configuration. This feature is in development, and
can be enabled during compilation:

>  CC=mpicc cmake .. -DCMAKE_INSTALL_PREFIX:PATH=/usr/local -DLRC:BOOL=ON

Scripts
-------

In the `scripts` directory you will find some helper tools for data postprocessing:

- `run.sh` -- examples how to run the Mechanic
- `h52ascii-{bash,tcsh}` -- tool to extract ASCII data from the Mechanic datafile (Bash
and Tcsh version)
- `dynamical-map.gnu` -- Gnuplot script to create dynamical map with `pm3d`

Using the module
----------------

1. Run the Mechanic (it will take some time):
>  mpirun -np 5 mechanic -p arnoldweb -n arnoldweb -x 512 -y 512 -d 25000

2. Extract output data for dynamical map (gnuplot, pm3d):
>  h52ascii arnoldweb-master-00.h5 512 /data/master

3. Plot the data:
>  gnuplot dynamical-map.gnu

Configuration of the module
---------------------------

When the module is compiled with configuration handling enabled, you can tell the Mechanic
to use basic configuration file:
>  mpirun -np 5 mechanic -p arnoldweb -n arnoldweb -x 512 -y 512 -d 25000 -m arnold.cfg

where the file is in the form:

>  [arnold]
>  xmin = 0.8
>  xmax = 1.2
>  ymin = 0.8
>  ymax = 1.2
>  step = 0.25
>  tend = 20000.0
>  eps = 0.01
>  driver = 1

You can switch here between Saba2 and Saba3 symplectic drivers (driver=1 or driver=2).

The configuration is handled by the Libreadconfig, please refer to library docs for
details.
