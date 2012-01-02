/**
 * @file
 * The Arnold Web module for Mechanic: The Arnold Web part
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "mechanic_module_arnoldweb.h"

/**
 * @function
 * The right hand sides + variational equations of the Hamiltonian model of the Arnold web, 
 * see Guzzo+ Science 289 (2000)
 */
void vinteraction(double *y,  double *a, double *dy, double *v, double eps) {
  double I1, I2, I3, f1, f2, f3, sf1, sf2;
  double sf3, cf1, cf2, cf3, dif, dif2, dif3, sum; 

  f1   = y[0];
  f2   = y[1];
  f3   = y[2]; 
  I1   = y[3];
  I2   = y[4];
  I3   = y[5];
  
  sf1  = sin(f1);
  sf2  = sin(f2);
  sf3  = sin(f3);
  cf1  = cos(f1);
  cf2  = cos(f2);
  cf3  = cos(f3);
  
  dif  = cf1 + cf2 + cf3 + 4;
  dif2 = eps/(dif*dif);
  dif3 = dif2/dif;
  
  // right hand sides  
  a[0] =  I1;
  a[1] =  I2;
  a[2] =  I3; 
  a[3] = -sf1*dif2;
  a[4] = -sf2*dif2;
  a[5] = -sf3*dif2;
  
  // variational equations
  v[0] =  1;
  v[1] =  1;
  v[2] =  0;
  
  sum  = 2*(sf1*dy[0] + sf2*dy[1] + sf3*dy[2])*dif3;
  
  v[3] = -cf1*dif2*dy[0] - sum*sf1;
  v[4] = -cf2*dif2*dy[1] - sum*sf2;
  v[5] = -cf3*dif2*dy[2] - sum*sf3;  
          
}

/**
 * @function
 * The energy integral
 */
double energy(double *y, double eps) {
  double I1, I2, I3, f1, f2, f3, cf1, cf2, cf3, dif, en; 

  f1   = y[0];
  f2   = y[1];
  f3   = y[2];
  I1   = y[3];
  I2   = y[4];
  I3   = y[5];
  
  cf1  = cos(f1);
  cf2  = cos(f2);
  cf3  = cos(f3);
  
  dif  = eps/(cf1 + cf2 + cf3 + 4);
  en   = I1*I1/2.0L + I2*I2/2.0L + I3 + dif;
  
  return en;  
}

/**
 * @function
 * The variational integral
 */
double variat(double *xv, double *dy, double eps) {
  double acc[6], var[6], vint;
 
  vinteraction(xv, acc, dy, var, eps);
 
  vint = -acc[3]*dy[0] -acc[4]*dy[1] -acc[5]*dy[2] + 
         +acc[0]*dy[3] +acc[1]*dy[4] +acc[2]*dy[5];

  return vint;
}

/**
 * @function
 * Evaluates the norm of a vector
 */
double checknorm(int dim, double *a) {
  double tmp = 0.0L;
  int i;

  for(i=1; i<=dim; i++) tmp += a[i]*a[i];

  return sqrtl(tmp);
}

/**
 * @function
 * Normalizes the variational vector
 */
double norm(int dim, double *a) {
  double tmp = 0.0L;
  int i;
  
  for(i=1; i<=dim; i++) tmp += a[i]*a[i];
  tmp = sqrtl(tmp);
  for(i=1; i<=dim; i++) a[i]= a[i]/tmp;  

  return tmp;
}

/**
 * @function
 * Symplectic MEGNO with the classical Leapfrog integrator
 */
double smegno_leapfrog(double *xv, double step, double tend, double eps, int smod, double *enerr) {
  double c1, c2;
  double Y0, mY0, Y1, mY1, maxe;   
  double acc[6], dy[6], var[6], t, dt, en, en0, h, delta, delta0;
  long int ks;
  int i;
  //double v0;
   
  c1    = 0.5L;
  c2    = 1.0L;
                  
  t     = 0.0L;
  maxe  = 0.0L;
  dt    = step;
  Y0    = mY0   = Y1    = mY1   = 0.0;
  for (i=1; i<=6; i++) dy[i] = rand()/(RAND_MAX+1.0);
  
  delta0= norm(6, dy); 
  en0   = energy(xv, eps);
  //  v0    = variat(xv, dy, eps);
  
  // integrator + variations Leapfrog
  
  ks    = 0;
  
  while (t <= tend) { 
  
    // 1st kick
    vinteraction(xv, acc, dy, var, eps);
    h     = c1*dt;    
    xv[3] = xv[3] + acc[3]*h;
    xv[4] = xv[4] + acc[4]*h;
    xv[5] = xv[5] + acc[5]*h; 
    
    dy[3] = dy[3] + var[3]*h;    
    dy[4] = dy[4] + var[4]*h;        
    dy[5] = dy[5] + var[5]*h;            
    
    // 1st drift
    h     = c2*dt;    
    xv[0] = xv[0] + xv[3]*h;
    xv[1] = xv[1] + xv[4]*h;
    xv[2] = xv[2] + 1.00*h;
    
    dy[0] = dy[0] + dy[3]*h;    
    dy[1] = dy[1] + dy[4]*h;        
    dy[2] = dy[2];        
    
    // 2nd kick
    vinteraction(xv, acc, dy, var, eps);
    h     = c1*dt;    
    xv[3] = xv[3] + acc[3]*h;
    xv[4] = xv[4] + acc[4]*h;
    xv[5] = xv[5] + acc[5]*h; 
    
    dy[3] = dy[3] + var[3]*h;    
    dy[4] = dy[4] + var[4]*h;        
    dy[5] = dy[5] + var[5]*h;            
 
    // integrator step done
    
    t = (ks++)*dt;

    // MEGNO
    delta   = checknorm(6, dy);    
    Y1      =  Y0*(ks-1.0L)/(1.0L*ks) + 2.0L*logl(delta/delta0);
    mY1     = mY0*(ks-1.0L)/(1.0L*ks) + Y1/(1.0L*ks);
    Y0      = Y1;
    mY0     = mY1;
    delta0  = delta;
    
    // relative errors of the energy and the variational integrator
    if (ks%smod == 0) {  
      en = fabs((energy(xv,eps)-en0)/en0);
      if (en>maxe) maxe = en;
    }
  }    

  *enerr = maxe;
  return mY1;
}      

/**
 * @function
 * Symplectic MEGNO with the SABA3 integrator by Robutel & Laskar
 */
double smegno_saba3(double *xv, double step, double tend, double eps, int smod, double *enerr) {
  double c1, c2, d1, d2;
  double Y0, mY0, Y1, mY1, maxe;   
  double acc[6], dy[6], var[6], t, dt, en, en0, h, delta, delta0;
  long int ks;
  int i;
  //double v0;
   
  // SABA3 coefficients
  c1    = 1.0/2.0L - sqrtl(15.0L)/10.0L;
  c2    = sqrtl(15.0L)/10.0L;
  d1    = 5.0L/18.0L;
  d2    = 4.0L/9.0L; 
                  
  t     = 0.0L;
  maxe  = 0.0L;
  dt    = step;
  Y0    = mY0   = Y1    = mY1   = 0.0;
  for (i=1; i<=6; i++) dy[i] = rand()/(RAND_MAX+1.0);
  
  delta0= norm(6, dy); 
  en0   = energy(xv, eps);
  //  v0    = variat(xv, dy, eps);
  
  // integrator + variations SABA3
  
  ks    = 0;
  
  while (t <= tend) { 
  
    // 1st drift
    h     = c1*dt;    
    xv[0] = xv[0] + xv[3]*h;
    xv[1] = xv[1] + xv[4]*h;
    xv[2] = xv[2] + 1.00*h;
    
    dy[0] = dy[0] + dy[3]*h;    
    dy[1] = dy[1] + dy[4]*h;        
    dy[2] = dy[2];        
    
    // 1st kick
    vinteraction(xv, acc, dy, var, eps);
    h     = d1*dt;    
    xv[3] = xv[3] + acc[3]*h;
    xv[4] = xv[4] + acc[4]*h;
    xv[5] = xv[5] + acc[5]*h; 
    
    dy[3] = dy[3] + var[3]*h;    
    dy[4] = dy[4] + var[4]*h;        
    dy[5] = dy[5] + var[5]*h;            
    
    // 2nd drift
    h     = c2*dt;    
    xv[0] = xv[0] + xv[3]*h;
    xv[1] = xv[1] + xv[4]*h;
    xv[2] = xv[2] + 1.00*h;
    
    dy[0] = dy[0] + dy[3]*h;    
    dy[1] = dy[1] + dy[4]*h;        
    dy[2] = dy[2];        
    
    // 2nd kick
    vinteraction(xv, acc, dy, var, eps);
    h     = d2*dt;    
    xv[3] = xv[3] + acc[3]*h;
    xv[4] = xv[4] + acc[4]*h;
    xv[5] = xv[5] + acc[5]*h; 
    
    dy[3] = dy[3] + var[3]*h;    
    dy[4] = dy[4] + var[4]*h;        
    dy[5] = dy[5] + var[5]*h;            
 
    // 3rd drift
    h     = c2*dt;
    xv[0] = xv[0] + xv[3]*h;
    xv[1] = xv[1] + xv[4]*h;
    xv[2] = xv[2] + 1.0*h;
    
    dy[0] = dy[0] + dy[3]*h;    
    dy[1] = dy[1] + dy[4]*h;        
    dy[2] = dy[2];            

    // 3rd kick
    vinteraction(xv, acc, dy, var, eps);
    h     = d1*dt;    
    xv[3] = xv[3] + acc[3]*h;
    xv[4] = xv[4] + acc[4]*h;
    xv[5] = xv[5] + acc[5]*h; 
    
    dy[3] = dy[3] + var[3]*h;    
    dy[4] = dy[4] + var[4]*h;        
    dy[5] = dy[5] + var[5]*h;            

    // final drift
    h     = c1*dt;
    xv[0] = xv[0] + xv[3]*h;
    xv[1] = xv[1] + xv[4]*h;
    xv[2] = xv[2] + 1.0*h;
    
    dy[0] = dy[0] + dy[3]*h;    
    dy[1] = dy[1] + dy[4]*h;        
    dy[2] = dy[2];            
    
    // integrator step done
    
    t = (ks++)*dt;

    // MEGNO
    delta   = checknorm(6, dy);    
    Y1      =  Y0*(ks-1.0L)/(1.0L*ks) + 2.0L*logl(delta/delta0);
    mY1     = mY0*(ks-1.0L)/(1.0L*ks) + Y1/(1.0L*ks);
    Y0      = Y1;
    mY0     = mY1;
    delta0  = delta;
    
    // relative errors of the energy and the variational integrator
    if (ks%smod == 0) {  
      en = fabs((energy(xv,eps)-en0)/en0);
      if (en>maxe) maxe = en;
    }
  }    

  *enerr = maxe;
  return mY1;
}      

