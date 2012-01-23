/*
 * MANDELBROT -- SAMPLE MECHANIC MODULE
 * Copyright (c) 2010-2011, Mariusz Slonina (Nicolaus Copernicus University)
 * All rights reserved.
 *
 * This file is part of MECHANIC code.
 *
 * MECHANIC was created to help solving many numerical problems by providing
 * tools for improving scalability and functionality of the code. MECHANIC was
 * released in belief it will be useful. If you are going to use this code, or
 * its parts, please consider referring to the authors either by the website
 * or the user guide reference.
 *
 * http://git.astri.umk.pl/project/mechanic
 *
 * Redistribution and use in source and binary forms,
 * with or without modification, are permitted provided
 * that the following conditions are met:
 *
 * - Redistributions of source code must retain the above copyright notice,
 *   this list of conditions and the following disclaimer.
 * - Redistributions in binary form must reproduce the above copyright notice,
 *   this list of conditions and the following disclaimer in the documentation
 *   and/or other materials provided with the distribution.
 * - Neither the name of the Nicolaus Copernicus University nor the names of
 *   its contributors may be used to endorse or promote products derived from
 *   this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

/* [MANDELBROT] */

/**
 * @section mandelbrot The Mandelbrot Module
 *
 * This module shows how to use basic API of Mechanic to compute any numerical
 * problem, in that case -- the Mandelbrot set.
 *
 * We use here only 3 functions: @c mandelbrot_init(), @c mandelbrot_cleanup()
 * and @c mandelbrot_task_process(). There is an additional function,
 * @c mandelbrot_generate_fractal(), which shows that you can even add external
 * functions to your module, since it is a standard C code.
 *
 * In addition, the module returns the number of node that computed the task.
 *
 * The header file:
 * @code
 * @icode modules/mandelbrot/mechanic_module_mandelbrot.h MANDELHEADER
 * @endcode
 */

/* [/MANDELBROT] */

/* [MANDELCOMPUTE] */

#include "mechanic.h"
#include "mechanic_module_mandelbrot.h"

/**
 * Implementation of module_init().
 */
int mandelbrot_init(int mpi_size, int node, TaskInfo* md, TaskConfig* d){

  md->output_length = 4;
  md->input_length = 4;

  return MECHANIC_TASK_SUCCESS;
}

/**
 * Implementation of module_cleanup().
 */
int mandelbrot_cleanup(int mpi_size, int node, TaskInfo* md, TaskConfig* d){

  return MECHANIC_TASK_SUCCESS;
}

/**
 * Implementation of module_task_process().
 */
int mandelbrot_task_process(int worker, TaskInfo *md, TaskConfig* d,
    TaskData* inidata, TaskData* r){

  double real_min, real_max, imag_min, imag_max;
  double scale_real, scale_imag;
  double c;

  real_min = -2.0;
  real_max = 2.0;
  imag_min = -2.0;
  imag_max = 2.0;
  c = 4.0;

  /* Coordinate system */
  scale_real = (real_max - real_min) / ((double) d->xres - 1.0);
  scale_imag = (imag_max - imag_min) / ((double) d->yres - 1.0);

  r->data[0] = real_min + r->coords[0] * scale_real;
  r->data[1] = imag_max - r->coords[1] * scale_imag;

  /* Mandelbrot set */
  r->data[2] = mandelbrot_generate_fractal(r->data[0], r->data[1], c);

  /* We also store information about the worker */
  r->data[3] = (double) worker;

  return MECHANIC_TASK_SUCCESS;
}

/**
 * An example of a custom function
 */
int mandelbrot_generate_fractal(double a, double b, double c){

  double temp, lengthsq;
  int max_iter = 256;
  int count = 0;
  double zr = 0.0, zi = 0.0;

  do {

    temp = zr*zr - zi*zi + a;
    zi = 2*zr*zi + b;
    zr = temp;
    lengthsq = zr*zr + zi*zi;
    count++;

  } while ((lengthsq < c) && (count < max_iter));

  return count;
}

/* [/MANDELCOMPUTE] */

