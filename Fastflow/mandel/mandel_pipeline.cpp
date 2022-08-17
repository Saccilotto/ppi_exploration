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

#include "ff/pipeline.hpp"

#define DIM 800
#define ITERATION 1024

using namespace ff;
//using namespace std;

void runPipe(int dim,double init_b,double step,double init_a,int niter);
double diffmsecnew(struct timeval  a,  struct timeval  b);

struct Item{
    int i;
    double im;
    unsigned char *M;
};

struct Database {
    int dim, niter;
    double init_a, init_b, step;
};

Database db;

struct Stage1: ff_node_t<Item> {
    
    Item * svc(Item * task) { 
        
        for(int i=0;i<=db.dim;i++) { 
            //cout << i << "\t";
            Item *item = new Item;    
            item->i = i; 
            item->im = db.init_b+(db.step*i);             
            item->M = (unsigned char *) malloc(db.dim);
            ff_send_out(item);
        }   
        return EOS;     
    }
};

struct Stage2: ff_node_t<Item> { 

    Item * svc(Item * task) {
        //cout << task->i << "\t";
        int j;
        for (j=0;j < db.dim; j++) {
            double a, b, cr;
            a = cr = db.init_a+db.step*j;
            b = task->im;
            double a2,b2;
            int k;
            for (k=0; k<db.niter; k++) {
                a2 = a*a;
                b2 = b*b;
                if ((a2+b2)>4.0) break;
                b = 2*a*b+task->im;
                a = a2-b2+cr;
            }
            task->M[j] = (unsigned char)(255-(k*255/db.niter));
        }     
        //cout << task->i << "\t";  
        return task; 
    }
};

struct Stage3: ff_node_t<Item> { 

    Item * svc(Item * task)  {
        //cout << task->i << endl;
        #if !defined(NO_DISPLAY)
            ShowLine(task->M,db.dim,task->i);
        #endif 
        free(task->M);
        delete(task);
        return GO_ON;
    }
};

void runPipe(){  
    //Stage1 * s1 = new Stage1(db);
    ff_Pipe<Item> pipe(new Stage1(), new Stage2(), new Stage3());

    if (pipe.run_and_wait_end()<0) error("running pipe"); 
}

double diffmsecnew(struct timeval  a,  struct timeval  b) {
    long sec  = (a.tv_sec  - b.tv_sec);
    long usec = (a.tv_usec - b.tv_usec);

    if(usec < 0) {
        --sec;
        usec += 1000000;
    }
    return ((double)(sec*1000) + (double)usec/1000.0);
}

int main(int argc, char **argv) {
    double init_a=-2.125,init_b=-1.5,range=3.0;
    double step;
    int dim = DIM, niter = ITERATION;
    //stats
    struct timeval t1,t2;
    int r, retries=1;
    double avg=0, var, * runs;

    if (argc<3) { 
        printf("Usage: seq size niterations\n\n\n");
    }
    else {
        dim = atoi(argv[1]);
        niter = atoi(argv[2]);
        step = range/((double) dim);
        retries = atoi(argv[3]);
    }

    runs = (double *) malloc(retries*sizeof(double));

    printf("Mandelbroot set from (%g+I %g) to (%g+I %g)\n",
           init_a,init_b,init_a+range,init_b+range);
    printf("resolution %d pixel, Max. n. of iterations %d\n",dim*dim,niter);

    step = range/((double) dim);

    #if !defined(NO_DISPLAY)
        SetupXWindows(dim,dim,1,NULL,"Pipeline Mandelbroot");
    #endif

    db.dim = dim;
    db.niter = niter;
    db.init_a = init_a;
    db.init_b = init_b;
    db.step = step;

    for (r=0; r<retries; r++) {
        //Start time
        gettimeofday(&t1,NULL);
        runPipe();
        //Stop time
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
