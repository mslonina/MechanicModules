/**
 * @file
 * The Hello, a sample Mechanic module
 */

/**
 * To understand what Mechanic is and what it does, let us write a well-known
 * "Hello World!". We will create small C library, let us call it @c Hello
 * and save it in @c mechanic_module_hello.c file.
 *
 * We need to compile the example code to a shared library. We can do that by
 * calling
 *
 * @code
 * mpicc -fPIC -c mechanic_module_hello.c -o mechanic_module_hello.o
 * mpicc -shared mechanic_module_hello.o -o libmechanic_module_hello.so
 * @endcode
 *
 * Mechanic need to know, where our module is, so we need to adjust
 * @c LD_LIBRARY_PATH (it depends on shell you are using) to the place
 * we saved our module. If you are a @c Bash user, try the following setting
 * in your @ct .bashrc @tc file:
 *
 * @code
 * export LD_LIBRARY_PATH=/usr/lib/:/usr/local/lib:.
 * @endcode
 *
 * We run @M with our module by
 *
 * @code
 * mpirun -np 3 mechanic -p hello
 * @endcode
 *
 * This will tell Mechanic to run on three nodes, in a task farm mode,
 * with one master node and two workers. The master node will send default
 * initial condition (pixel coordinates) to each worker and receive data
 * in @c TaskData structure (in this case the coordinates of the pixel).
 *
 * The output should be similar to:
 *
 * @code
 * -> Mechanic
 *    v. 0.12-UNSTABLE-3-6-2
 *    Author: MSlonina, TCfA, NCU
 *    Bugs: mariusz.slonina@gmail.com
 *    http://mechanics.astri.umk.pl/projects/mechanic
 * !! Config file not specified/doesn't exist. Will use defaults.
 * -> Mechanic will use these startup values:
 * (...)
 * -> Hello from worker[1]
 * -> Hello from worker[2]
 * @endcode
 *
 * Two last lines were printed using our simple module. In the working
 * directory you should find also @c mechanic-master-00.h5 file. It is a data
 * file written by the master node, and each run of Mechanic will produce
 * such file. It containes all information about the setup of the simulation
 * and data received from workers.
 *
 * If you try
 *
 * @code
 * h5dump -n mechanic-master-00.h5
 * @endcode
 *
 * you should see the following output:
 *
 * @code
 * HDF5 "mechanic-master.h5" {
 * FILE_CONTENTS {
 *  group      /
 *  dataset    /board
 *  group      /config
 *  group      /config/mechanic
 *  dataset    /config/mechanic/default
 *  dataset    /config/mechanic/logs
 *  group      /data
 *  dataset    /data/master
 *  }
 * }
 * @endcode
 *
 * which describes the data storage in master file. There are two additional
 * files in the working dir with suffixes 01 and 02 -- these are checkpoint
 * files, see @ref checkpoint for detailes.
 *
 * @section hello The Hello Module
 *
 * Let us go step-by-step through the @c Hello module. Each Mechanic module must
 * contain the preprocessor directive
 *
 * @code
 * #include "mechanic.h"
 * @endcode
 *
 * The module specific header file
 *
 * @code
 * #include "mechanic_module_hello.h"
 * @endcode
 *
 * is optional. Since each module is a normal C code, you can also use any
 * other headers and link to any other library during compilation.
 *
 * Every function in the module is prefixed with the name of
 * the module -- thus, you should use unique names for your modules. The file
 * name prefix, @c mechanic_module_ is required for proper module loading.
 *
 * The first three functions:
 *
 * - @c hello_init()
 * - @c hello_cleanup()
 * - @c hello_task_process()
 *
 * are required for the module to work in real-life applications. 
 * If any of them is missing, Mechanic will switch to their default prototypes.
 * The fourth one, @c hello_worker_out() is optional
 * and belongs to the templateable functions group
 * (see @ref template-system).
 *
 * Each function should return an integer value,
 * 0 on success and errcode on failure, which is important for proper error
 * handling.
 *
 * - @c hello_init is called on module initialization
 *   and you need to provide some information about the module, especially,
 *   @c md->output_length, which is the length of the results array sended from the
 *   worker node to master node. The @c TaskInfo type contains information
 *   about the module, and will be extended in the future. The structure is
 *   available for all module functions. The @c TaskInfo type has
 *   the following shape:
 *   @code
 *   @icode core/mechanic.h MODULEINFO
 *   @endcode
 *
 * - @c hello_cleanup currently does nothing,
 *   however, it is required for future development.
 *
 * - @c hello_task_process
 *   is the heart of your module. Here you can compute almost any type of
 *   numerical problem or even you can call external application from here.
 *   There are technically no contradictions for including Fortran based
 *   code. In this simple example we just assign coordinates of the
 *   simulation (see @ref coords) to the result array @c r->res. The array is
 *   defined in @c TaskData structure, as follows:
 *
 *   @code
 *   @icode core/mechanic.h MASTERDATA
 *   @endcode
 *
 *   Currently, @c MECHANIC_DATATYPE is set to @c double, so we need to do
 *   proper casting from integer to double. The result array
 *   has the @c md->output_length size, in this case 3. The @c TaskData structure is
 *   available for all module functions.
 *
 * - @c hello_worker_out
 *   prints formatted message from the worker node on the screen, after the
 *   node did its job. The @c mechanic_message() (see @ref devel) is available
 *   for all modules, and can be used for printing different kinds
 *   of messages, i.e. some debug information or warning.
 *
 */

#include "mechanic.h"
#include "mechanic_module_hello.h"

/**
 * Implementation of module_init().
 */
int hello_init(int mpi_size, int node, TaskInfo* md, TaskConfig* d){

  md->input_length = 3;
  md->output_length = 6;

  return MECHANIC_TASK_SUCCESS;
}

/**
 * Implementation of module_cleanup().
 */
int hello_cleanup(int mpi_size, int node, TaskInfo* md, TaskConfig* d){

  return MECHANIC_TASK_SUCCESS;
}

/**
 * Implementation of module_node_in().
 */
int hello_master_in(int mpi_size, int node, TaskInfo* md, TaskConfig* d,
    TaskData* inidata) {

  inidata->data[0] = 99.0;

  printf("Configuration test:\n");
  printf("name: %s\n", d->name);
  printf("file: %s\n", d->datafile);
  printf("module: %s\n", d->module);

  return MECHANIC_TASK_SUCCESS;
}

/**
 * Implementation of module_task_prepare().
 */
int hello_task_prepare(int node, TaskInfo* md, TaskConfig* d,
    TaskData* inidata, TaskData* r) {

  inidata->data[1] = (double) d->xres;
  inidata->data[2] = (double) node;

  return MECHANIC_TASK_SUCCESS;
}

/**
 * Implementation of module_task_process().
 */
int hello_task_process(int node, TaskInfo* md, TaskConfig* d,
    TaskData* inidata, TaskData* r)
{

  r->data[0] = (double) r->coords[0];
  r->data[1] = (double) r->coords[1];
  r->data[2] = (double) r->coords[2];
  r->data[3] = inidata->data[0];
  r->data[4] = inidata->data[1];
  r->data[5] = inidata->data[2];

  return MECHANIC_TASK_SUCCESS;
}

/**
 * Implementation of module_node_out().
 */
int hello_worker_out(int nodes, int node, TaskInfo* md, TaskConfig* d,
    TaskData* inidata, TaskData* r){

  mechanic_message(MECHANIC_MESSAGE_INFO, "Hello from worker[%d]\n", node);

  return MECHANIC_TASK_SUCCESS;
}


