#ifndef MECHANIC_MODULE_ARNOLDWEB_H
#define MECHANIC_MODULE_ARNOLDWEB_H

#ifndef MECHANIC_TASK_SUCCESS
#define MECHANIC_TASK_SUCCESS 0
#endif

double smegno2(double *xv, double step, double tend, double eps, double *err);
double smegno3(double *xv, double step, double tend, double eps, double *err);

#endif 
