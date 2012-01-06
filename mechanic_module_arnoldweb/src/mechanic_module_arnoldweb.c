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
 * condition vector `irl`, which is sended to each worker node and the length of the
 * master result `mrl`, which is sended from the worker node to the master. 
 *
 * The Arnold Web: Since we are developing the dynamical map, we need to store coordinates
 * of the map and the final state of the system (MEGNO). We also prepare the initial
 * condition for each worker node -- we need here a vector of length 6. 
 */
int arnoldweb_init(int mpi_size, int node, moduleInfo *info, configData *config) {
  
  info->mrl = 4;
  info->irl = 6;
  
  return MECHANIC_TASK_SUCCESS;
}

/**
 * @function
 * Implements module_task_prepare().
 *
 * We prepare here the initial condition for the task. The length of the `inidata->res`
 * vector is allocated according to the `md->irl` variable. We use the default,
 * two-dimensional mapping of the coordinates of the current task (r->coords).
 */
int arnoldweb_task_prepare(int node, moduleInfo *info, configData *config, masterData *in, masterData *out) {
  double xmin, xmax, ymin, ymax;

  /* Global map range (equivalent of frequencies space) */
  xmin  = 0.8;
  xmax  = 1.2;
  ymin  = 0.8;
  ymax  = 1.2;         

  /* Initial condition - angles */
  in->res[0] = 0.131;
  in->res[1] = 0.132;
  in->res[2] = 0.212;

  /* Map coordinates */  
  in->res[3] = xmin + out->coords[0]*(xmax-xmin)/(1.0*config->xres);
  in->res[4] = ymin + out->coords[1]*(ymax-ymin)/(1.0*config->yres);
  in->res[5] = 0.01;  

  return MECHANIC_TASK_SUCCESS;
}

/**
 * @function
 * Implements module_task_process().
 *
 * This function is the heart of the simulation. You connect your software to the
 * Mechanic here. 
 *
 * The Arnold web: We take the prepared initial condition and run the smegno() function.
 * After the computations are finished, we return the final result to the master node.
 */
int arnoldweb_task_process(int node, moduleInfo *info, configData *config, masterData *in, masterData *out) {
  double err, xv[6], tend, step, eps, result;

  step  = 0.25*(pow(5,0.5)-1)/2.0;
  tend  = 20000.0;
  eps   = 0.01;

  /* Initial data */  
  xv[0] = in->res[0];
  xv[1] = in->res[1];
  xv[2] = in->res[2];
  xv[3] = in->res[3];
  xv[4] = in->res[4];
  xv[5] = in->res[5];  

  /* Numerical integration goes here */
  result = smegno2(xv, step, tend, eps, &err);
  
  /* Assign the master result */
  out->res[0] = xv[3];
  out->res[1] = xv[4];
  out->res[2] = result;
  out->res[3] = err;

  return MECHANIC_TASK_SUCCESS;
}

