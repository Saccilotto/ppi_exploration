/* ***************************************************************************
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License version 2 as
 *  published by the Free Software Foundation.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 *
 *  As a special exception, you may use this file as part of a free software
 *  library without restriction.  Specifically, if other files instantiate
 *  templates or use macros or inline functions from this file, or you compile
 *  this file and link it with other files to produce an executable, this
 *  file does not by itself cause the resulting executable to be covered by
 *  the GNU General Public License.  This exception does not however
 *  invalidate any other reasons why the executable file might be covered by
 *  the GNU General Public License.
 *
 ****************************************************************************
 */

/*

   Author: Marco Aldinucci.
   email:  aldinuc@di.unipi.it
   marco@pisa.quadrics.com
   date :  15/11/97

Modified by:

****************************************************************************
 *  Author: Dalvan Griebler <dalvangriebler@gmail.com>
 *
 *  Copyright: GNU General Public License
 *  Description: This program simply computes the mandelbroat set.
 *  File Name: mandel.cpp
 *  Version: 1.0 (25/05/2018)
 *  Compilation Command: make
 ****************************************************************************
*/

#include <stdio.h>
#include "marX2.h"
#include <sys/time.h>
#include <math.h>

#include <iostream>
#include <chrono>

#include "tbb/parallel_for.h"
#include "tbb/blocked_range.h"
#include "tbb/task_scheduler_init.h"

#define DIM 800
#define ITERATION 1024

using namespace tbb;

double diffmsecnew(struct timeval  a,  struct timeval  b);

double diffmsecnew(struct timeval  a,  struct timeval  b) {
    long sec  = (a.tv_sec  - b.tv_sec);
    long usec = (a.tv_usec - b.tv_usec);

    if(usec < 0) {
        --sec;
        usec += 1000000;
    }
    return ((double)(sec*1000)+ (double)usec/1000.0);
}

class Mapper{
    public:
        double step, init_a, init_b;
        int nthreads, dim, niter;
        unsigned char *M;

    Mapper(double _step, double _init_a, double _init_b,int _nthreads, int _dim, int _niter){       
        step = _step;
        init_a = _init_a;
        init_b = _init_b;
        dim = _dim;
        niter = _niter;
        nthreads = _nthreads;
        M = {nullptr};
        control();
    }

    void control() {
        int i;
        for(i = 0;i < dim;i++){
            M = (unsigned char *) malloc(dim);
            double im = init_b+(step*i);
            compute(im, nthreads, i, M);
        }
    }

    void compute(double im, int nthreads, int i, unsigned char *M){
        task_scheduler_init init(nthreads);
        parallel_for(
            blocked_range<int>(0,dim),
            [=](blocked_range<int> &l){
                int j;
                for(j = l.begin();j != l.end();++j) { 
                    double a,b,cr;                
                    a = cr = init_a+step*j;                       
                    b = im; 
                    int k;
                    double a2,b2;
                    for (k=0;k<niter;k++) {
                        a2 = a*a;
                        b2 = b*b;
                        if ((a2+b2)>4.0) break;
                        b = 2*a*b+im;
                        a = a2-b2+cr;
                    }
                    M[j]= (unsigned char) 255 -((k*255/niter));
                }
            }
        );    
        display(i, M);
    }

    void display(int i, unsigned char *M){
        #if !defined(NO_DISPLAY)
            ShowLine(M,dim,i); 
        #endif
    }
};

int main(int argc, char **argv) {
    double init_a=-2.125,init_b=-1.5,range=3.0;
    double step;
    int dim = DIM, niter = ITERATION;
    // stats
    struct timeval t1,t2;
    int nthreads = 1;
    int r,retries = 1;
    double avg=0, var, * runs;

    if (argc<4) {
        printf("Usage: seq size niterations\n\n\n");
    }
    else {
        nthreads = atoi(argv[1]);
        dim = atoi(argv[2]);
        niter = atoi(argv[3]);
        step = range/((double) dim);
        retries = atoi(argv[4]);
    }
    runs = (double *) malloc(retries*sizeof(double));

    printf("Mandelbroot set from (%g+I %g) to (%g+I %g)\n",
           init_a,init_b,init_a+range,init_b+range);
    printf("resolution %d pixel, Max. n. of iterations %d\n",dim*dim,ITERATION);

    step = range/((double) dim);

    #if !defined(NO_DISPLAY)
        SetupXWindows(dim,dim,1,NULL,"Sequential Mandelbroot");
    #endif

    for (r=0; r<retries; r++) {
        gettimeofday(&t1,NULL);
        // Start time
        Mapper map(step,init_a,init_b,nthreads,dim,niter);
        // Stop time
        gettimeofday(&t2,NULL);

        avg += runs[r] = diffmsecnew(t2,t1);
        printf("Run [%d] DONE, time= %f (ms)\n",r, runs[r]);
    }

    avg = avg / (double) retries;
    var = 0;
    for (r=0; r<retries; r++) {
        var += (runs[r] - avg) * (runs[r] - avg);
    }
    
    var /= retries;
    printf("Average on %d experiments = %f (ms) Std. Dev. %f\n\nPress a key\n",retries,avg,sqrt(var));

    #if !defined(NO_DISPLAY)
        getchar();
        CloseXWindows();
    #endif
    return 0;
}