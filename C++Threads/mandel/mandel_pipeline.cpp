#include <stdio.h>
#include <sys/time.h>
#include <math.h>
#include "marX2.h"
#include "concurrentQueue.hpp"

#include <iostream> 
#include <chrono>

#include <mutex>
#include <thread>   

using namespace concurrent::queue;
using namespace std;

#define DIM 800
#define ITERATION 1024

struct Item {

private:
    static int id;

public:
    int CURRENT_ID;
    bool isLast;
    double im;
    unsigned char *M;

    Item() {
        im = 0.0;
        M = {nullptr};
        isLast = true;
    }

    Item(int dim) {
        CURRENT_ID = id;
        im = 0.0;
        M = {nullptr};
        setIsLast(dim);
        nextId();
    }

    void nulify() {
        im = 0.0;
        M = {nullptr};
        isLast = true;
    }

    void setIsLast(int _dim) {
        CURRENT_ID >= _dim ? isLast = true:isLast = false;
    }

    void reset() {
        id = 0;
    }
 
    void nextId() {
        id += 1;
    }

    void insertIm(double _im) {
        im = _im;
    }

    void insertM(unsigned char *_M) {
        M = _M;
    }

    int getCurrentId() {
        return CURRENT_ID;
    }

    bool getIsLast() { 
        return isLast;
    }  
};

int Item::id = 0;
// queues declaration
blocking_queue<Item> queue1;
blocking_queue<Item> queue2;

class source {
public:   
    int dim;
    double init_b, step;

    // ~source() = default;
    // source(const source&) = default;// copy constructor
    // source(source&& other) noexcept  = default;
    // source& operator=(const source& other) = default; // copy assignment
    // source& operator=(source&& other) noexcept  = default;// move assignment

    source(int _dim, double _init_b, double _step):
        dim(_dim),
        init_b(_init_b),
        step(_step)
    {}

    void operator()() {
        cout << "Source\n";  
        while(1){  
            Item it(dim);  
            if(it.getIsLast()){
                it.nulify();                         
                queue1.enqueue(it);     
                break;
            }
            double im = init_b+(step*it.getCurrentId());           
            it.insertIm(im);                    
            queue1.enqueue(it);    
        }
        return;
    };
};


class computation {
public:   
    int dim, niter;
    double init_a, step;

    // ~computation() = default;
    // computation(const computation&) = default;// copy constructor
    // computation(computation&& other) noexcept  = default;
    // computation& operator=(const computation& other) = default; // copy assignment
    // computation& operator=(computation&& other) noexcept  = default;// move

    computation(int _dim, int _niter, double _init_a, double _step):
        dim(_dim),
        niter(_niter),
        init_a(_init_a),
        step(_step)
    {}

    void operator()() { 
        cout << "worker\n";
        while(1) {
            Item it = queue1.dequeue(); 
            if(it.getIsLast()){
                queue2.enqueue(it);
                break;
            }
            unsigned char * M = (unsigned char *) malloc(dim);
            int j;
            for (j = 0;j < dim;j++) {
                double a, cr;
                a = cr = init_a+step*j;
                double b = it.im;
                double a2, b2;
                int k;
                for (k = 0;k < niter;k++) { 
                    a2 = a*a;
                    b2 = b*b;
                    if ((a2+b2)>4.0) break;
                    b = 2*a*b+it.im;
                    a = a2-b2+cr;
                }
            M[j] = (unsigned char)(255-(k*255/niter));
            }
            it.insertM(M); 
            queue2.enqueue(it); 
        } 
        return;
    };
};

class sink {
public:   
    int dim;

    // ~sink() = default;
    // sink(const sink&) = default;// copy constructor
    // sink(sink&& other) noexcept  = default;
    // sink& operator=(const sink& other) = default; // copy assignment
    // sink& operator=(sink&& other) noexcept  = default;// move assignment

    sink(int _dim):
        dim(_dim)
    {}

    void operator()() {
        cout << "Sink\n";
        while(1) { 
            // cout << "Item received\n";
            Item it = queue2.dequeue();
            if(it.getIsLast()){
                it.reset();
                break;
            } 
            #if !defined(NO_DISPLAY)
                ShowLine(it.M, dim, it.getCurrentId());
            #endif            
        }
        return;
    }; 
};

double diffmsecnew(struct timeval  a,  struct timeval  b) {
    long sec  = (a.tv_sec  - b.tv_sec);
    long usec = (a.tv_usec - b.tv_usec);

    if(usec < 0) {
        --sec;
        usec += 1000000;
    }
    return ((double)(sec*1000)+ (double)usec/1000.0);
}

void management(int dim, int niter, double init_a, double init_b, double step) {
    cout << "Management\n" ;

    source src(dim, init_b, step);
    computation cmp(dim, niter, init_a, step);
    sink snk(dim);

    thread source(src);
    thread computation(cmp);
    thread sink(snk);
    
    int counter = 0;
    while(counter < 3) {
        if(source.joinable()) {
            counter++;
            source.join();
        }else {
            cout << "non joinable source" << endl;
        }
        if(computation.joinable()) {
            counter++;
            computation.join();
        }else {
            cout << "non joinable computation" << endl;
        }
        if(sink.joinable()) {
            counter++;
            sink.join();
        }else {
            cout << "non joinable sink" << endl;
        }
    }  
}

int main(int argc, char** argv) {
    cout << "\n";
    double init_a=-2.125,init_b=-1.5,range=3.0;
    double step;
    int dim = DIM, niter = ITERATION;
    // stats
    struct timeval t1,t2;
    int r,retries = 1;
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
    printf("resolution %d pixel, Max. n. of iterations %d\n",dim*dim,ITERATION);

    step = range/((double) dim);

    #if !defined(NO_DISPLAY)
        SetupXWindows(dim,dim,1,NULL,"C++ Threads Pipeline Mandelbroot");
    #endif

    for (r = 0;r < retries;r++) {
        cout << "********************\n";

        // Start time
        gettimeofday(&t1,NULL);

        management(dim, niter, init_a, init_b, step); 
         
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
