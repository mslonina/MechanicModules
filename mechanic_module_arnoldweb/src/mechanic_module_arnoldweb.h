#ifndef MECHANIC_MODULE_ARNOLDWEB_H
#define MECHANIC_MODULE_ARNOLDWEB_H

#ifndef MECHANIC_TASK_SUCCESS
#define MECHANIC_TASK_SUCCESS 0
#endif

double smegno_saba3(double *xv, double step, double tend, double eps, int smod, double *enerr);
double smegno_leapfrog(double *xv, double step, double tend, double eps, int smod, double *enerr);

#endif 
