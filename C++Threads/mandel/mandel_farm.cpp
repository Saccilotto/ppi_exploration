#include <stdio.h>
#include <sys/time.h>
#include <math.h>
#include "marX2.h"
#include "concurrentQueue.hpp"
#include "threadWrapper.hpp"

#include <iostream>
#include <chrono>

#include <future>
#include <mutex>
#include <thread> 
#include <vector>

using namespace concurrent::wrapper;
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

    Item(int dim){
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

    void nextId() {
        id += 1;
    }
    
    void reset() {
        id = 0;
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

mutex mng_lock;
int Item::id = 0;
// queues declaration
blocking_queue<Item> queue1;
blocking_queue<Item> queue2;

class source {
public:   
    int dim, threads;
    double init_b, step;
    
    ~source() = default;
    source(const source&) = default;// copy constructor
    source(source&& other) noexcept  = default;
    source& operator=(const source& other) = default; // copy assignment
    source& operator=(source&& other) noexcept  = default;// move assignment

    source(int _dim, int _threads, double _init_b, double _step):
        dim(_dim),
        threads(_threads),
        init_b(_init_b),
        step(_step)
    {}

    void operator()() {          
        while(1){  
            Item it(dim);  
            if(it.getIsLast()) {
                it.nulify();
                int i = 0;
                while(i < threads){                             
                    queue1.enqueue(it);  
                    ++i;
                    Item it();
                } 
                break;
            }
            double im = init_b+(step*it.getCurrentId());           
            it.insertIm(im);                    
            queue1.enqueue(it);    
            /*{
                lock_guard<mutex> lock(cout_lock);
                cout << "enqueue item to queue1 = " << it.i << endl;
            }*/
        }
        return;
    };
};

class computation {
public:   
    int dim, niter;
    double init_a, step;

    ~computation() = default;
    computation(const computation&) = default;// copy constructor
    computation(computation&& other) noexcept  = default;
    computation& operator=(const computation& other) = default; // copy assignment
    computation& operator=(computation&& other) noexcept  = default;// move assignment

    computation(int _dim, int _niter, double _init_a, double _step):
        dim(_dim),
        niter(_niter),
        init_a(_init_a),
        step(_step)
    {}

    void operator()() { 
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
            /*{
                lock_guard<mutex> lock(cout_lock);
                cout << "enqueue item to queue2 = " << it.i << endl;
            }*/
        } 
        return;           
    };
};

class sink {
public:   
    int dim, threads;

    ~sink() = default;
    sink(const sink&) = default;// copy constructor
    sink(sink&& other) noexcept  = default;
    sink& operator=(const sink& other) = default; // copy assignment
    sink& operator=(sink&& other) noexcept  = default;// move assignment

    sink(int _dim, int _threads):
        dim(_dim),
        threads(_threads)
    {}

    void operator()() {
        int last_itens_counter = 0;
        while(1) { 
            Item it = queue2.dequeue(); 
            if(it.getIsLast()) {
                last_itens_counter++;
                if(last_itens_counter == threads) {
                    it.reset();
                    break;
                }
            } else {
                #if !defined(NO_DISPLAY)
                    ShowLine(it.M, dim, it.getCurrentId());
                #endif
            }
        }
        return;
    }; 
};

void management(int dim, int niter, int threads, double init_a, double init_b, double step) {
    vector<threadWrapper> allThreads;

    source src = source(dim, threads, init_b, step);
    allThreads.push_back(move(threadWrapper(thread(src))));
    for(int i = 0; i < threads; ++i) {
        computation cmpt = computation(dim, niter, init_a, step);
        allThreads.push_back(move(threadWrapper(thread(cmpt))));
    }
    sink snk = sink(dim, threads);
    allThreads.push_back(move(threadWrapper(thread(snk))));
    
    // while(!allThreads.empty()) {
    //     allThreads.pop_back();
    // }

    // allThreads.clear(); // TODO: Is it the best choice for cleaning the values?
}

double diffmsecnew(struct timeval  a,  struct timeval  b) {
    long sec  = (a.tv_sec  - b.tv_sec);
    long usec = (a.tv_usec - b.tv_usec);

    if(usec < 0) {
        --sec;
        usec += 1000000;
    }
    return ((double)(sec*1000)+ (double)usec/1000.0);
}

int main(int argc, char **argv) {
    double init_a=-2.125,init_b=-1.5,range=3.0;
    double step;
    int dim = DIM, niter = ITERATION;
    int threads = 1;
    // stats
    struct timeval t1,t2;
    int r,retries = 1;
    double avg=0, var, * runs;

    if (argc<4) {
        printf("Usage: seq size niterations threads\n\n\n");
    }
    else {
        dim = atoi(argv[1]);
        niter = atoi(argv[2]);
        step = range/((double) dim);
        threads = atoi(argv[3]);
        retries = atoi(argv[4]);
    }
    runs = (double *) malloc(retries*sizeof(double));

    printf("Mandelbroot set from (%g+I %g) to (%g+I %g)\n",
           init_a,init_b,init_a+range,init_b+range);
    printf("resolution %d pixel, Max. n. of iterations %d\n",dim*dim,ITERATION);

    step = range/((double) dim);

    #if !defined(NO_DISPLAY)
        SetupXWindows(dim,dim,1,NULL,"C++ Threads Farm Mandelbrot");
    #endif

    for (r = 0;r < retries;r++) {
        // Start time
        gettimeofday(&t1,NULL);
        // future<void> await  = std::async(management, dim, niter, threads, init_a, init_b, step);
        // await.get
        management(dim, niter, threads, init_a, init_b, step);
        // mng_lock.lock();
        // future<bool> await  = std::async(std::launch::async, management, dim, niter, threads, init_a, init_b, step);
        // mng_lock.unlock();
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
