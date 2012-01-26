/**
 * @file
 * The Arnold Web module for Mechanic: The Mechanic part
 * 
 * Working code for the Mechanic-0.12 master branch
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

/**
 * Include Mechanic datatypes and function prototypes
 */
#include "mechanic.h"

/**
 * Include module-related stuff
 */
#include "mechanic_module_arnoldweb.h"

/**
 * Each API function receives full information about the run, such as:
 * - Worker node id
 * - MPI_COMM_SIZE
 * - Configuration data and module information
 *
 * For the detailed documentation of datatypes and full list of API functions, 
 * please refer to the technical documentation of the Mechanic at the project website:
 * 
 * http://github.com/mslonina/Mechanic
 *
 * Note: each function should return MECHANIC_TASK_SUCCESS on success or one of the following error codes:
 *
 * - 911 -- MECHANIC_ERR_MPI -- MPI related error
 * - 912 -- MECHANIC_ERR_HDF -- HDF related error
 * - 913 -- MECHANIC_ERR_MODULE -- Module subsystem related error
 * - 914 -- MECHANIC_ERR_SETUP -- Setup subsystem related error
 * - 915 -- MECHANIC_ERR_MEM -- Memory allocation related error
 * - 916 -- MECHANIC_ERR_CHECKPOINT -- Checkpoint subsystem related error
 * - 999 -- MECHANIC_ERR_OTHER -- Any other error
 */

/**
 * @function
 * Implements module_init().
 *
 * This function is called very early during module initialization. We must set up here
 * some basic information on the master data storage -- the length of the initial
 * condition vector `input_length`, which is sended to each worker node and the length of the
 * master result `output_length`, which is sended from the worker node to the master. 
 *
 * The Arnold Web: Since we are developing the dynamical map, we need to store coordinates
 * of the map and the final state of the system (MEGNO). We also prepare the initial
 * condition for each worker node -- we need here a vector of length 6. 
 *
 * One more feature: the Mechanic can handle additional module configuration. To do so, we need 
 * to define the size of the configuration `options` and call the module_setup_schema().
 * Since this feature is in development, we decided to use conditional compilation here.
 * See the README.txt for details.
 */
int arnoldweb_init(int mpi_size, int node, TaskInfo *info, TaskConfig *config) {
  
  info->output_length = 4;
  info->input_length = 6;

#ifdef LRC
  info->options = 8;
#endif

  return MECHANIC_TASK_SUCCESS;
}

#ifdef LRC
/**
 * @function
 * Implements module_setup_schema().
 *
 * We specify here the custom module configuration. The structure has the form:
 * {namespace, variable, value, type}. We must specify the variables and their defaults for the whole 
 * configuration structure (see `info->options`). We can override the defaults by using the config file (# marks comments):
 * 
 * [arnold]
 * xmin = 0.8
 * xmax = 1.2
 * ymin = 0.8
 * ymax = 1.2
 * step = 0.25
 * tend = 20000.0
 * eps = 0.01
 * driver = 1 
 * 
 * After the configuration is done, we may use it as follows:
 * > xmin = LRC_option2double("arnold", "xmin", info->moptions);
 */
int arnoldweb_setup_schema(TaskInfo *info) {

  info->mconfig[0] = (LRC_configDefaults) {"arnold", "step", "0.25", LRC_DOUBLE};
  info->mconfig[1] = (LRC_configDefaults) {"arnold", "tend", "1000.0", LRC_DOUBLE};
  info->mconfig[2] = (LRC_configDefaults) {"arnold", "xmin", "0.8", LRC_DOUBLE};
  info->mconfig[3] = (LRC_configDefaults) {"arnold", "xmax", "1.2", LRC_DOUBLE};
  info->mconfig[4] = (LRC_configDefaults) {"arnold", "ymin", "0.8", LRC_DOUBLE};
  info->mconfig[5] = (LRC_configDefaults) {"arnold", "ymax", "1.2", LRC_DOUBLE};
  info->mconfig[6] = (LRC_configDefaults) {"arnold", "eps", "0.01", LRC_DOUBLE};
  info->mconfig[7] = (LRC_configDefaults) {"arnold", "driver", "1", LRC_INT};

  return MECHANIC_TASK_SUCCESS;
}
#endif

/**
 * @function
 * Implements module_task_prepare().
 *
 * We prepare here the initial condition for the task. The length of the `inidata->res`
 * vector is allocated according to the `md->input_length` variable. We use the default,
 * two-dimensional mapping of the coordinates of the current task (out->coords).
 */
int arnoldweb_task_prepare(int node, TaskInfo *info, TaskConfig *config, TaskData *in, TaskData *out) {
  double xmin, xmax, ymin, ymax;

  /* Global map range (equivalent of frequencies space) */
  xmin  = 0.8;
  xmax  = 1.2;
  ymin  = 0.8;
  ymax  = 1.2;         

#ifdef LRC
  xmin = LRC_option2double("arnold", "xmin", info->moptions);
  xmax = LRC_option2double("arnold", "xmax", info->moptions);
  ymin = LRC_option2double("arnold", "ymin", info->moptions);
  ymax = LRC_option2double("arnold", "ymax", info->moptions);
#endif

  /* Initial condition - angles */
  in->data[0] = 0.131;
  in->data[1] = 0.132;
  in->data[2] = 0.212;

  /* Map coordinates */  
  in->data[3] = xmin + out->coords[0]*(xmax-xmin)/(1.0*config->xres);
  in->data[4] = ymin + out->coords[1]*(ymax-ymin)/(1.0*config->yres);
  in->data[5] = 0.01;  

  return MECHANIC_TASK_SUCCESS;
}

/**
 * @function
 * Implements module_task_process().
 *
 * This function is the heart of the simulation. You connect your software to the
 * Mechanic here. It is called in a task loop, until all tasks are processed.
 *
 * The Arnold web: We take the prepared initial condition and run the smegno() function.
 * After the computations are finished, we return the final result to the master node.
 */
int arnoldweb_task_process(int node, TaskInfo *info, TaskConfig *config, TaskData *in, TaskData *out) {
  double err, xv[6], tend, step, eps, result;
#ifdef LRC
  int driver;
#endif

  step  = 0.25*(pow(5,0.5)-1)/2.0;
  tend  = 20000.0;
  eps   = 0.01;

#ifdef LRC  
  step = LRC_option2double("arnold", "step", info->moptions);
  step  = step*(pow(5,0.5)-1)/2.0;
  tend = LRC_option2double("arnold", "tend", info->moptions);
  eps  = LRC_option2double("arnold", "eps", info->moptions);
  driver = LRC_option2int("arnold", "driver", info->moptions);
#endif

  /* Initial data */  
  xv[0] = in->data[0];
  xv[1] = in->data[1];
  xv[2] = in->data[2];
  xv[3] = in->data[3];
  xv[4] = in->data[4];
  xv[5] = in->data[5];  

  /* Numerical integration goes here */
#ifdef LRC
  if (driver == 1) result = smegno2(xv, step, tend, eps, &err);
  if (driver == 2) result = smegno3(xv, step, tend, eps, &err);
#else  
  result = smegno2(xv, step, tend, eps, &err);
#endif

  /* Assign the master result */
  out->data[0] = xv[3];
  out->data[1] = xv[4];
  out->data[2] = result;
  out->data[3] = err;

  return MECHANIC_TASK_SUCCESS;
}
 
