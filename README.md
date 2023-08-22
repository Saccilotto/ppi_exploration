This repository consists on Mandelbrot set's calculation and image generation application. Its output produces the Mandelbrot's visuzalization. 
The original application was changed in order to leverage parallel desing patterns' strategies. 
These implementations were made with parallel programming interfaces' usage and their performance was reported on each one. 

The repository's organization is defined as follows:
Spar folder: SPar Mandelbrot implementation and its report inside mandel folder.
TBB folder: Intel TBB Mandelbrot implementation and its report inside mandel folder.
Fastflow folder: FastFlow Mandelbrot implementation and its report inside mandel folder.

C++Threads folder: C++Threads Mandelbrot implementation and its report inside mandel folder.

Parallel Programming Interfaces(PPIs) Used:

Spar makes use of simple annotations composed of the parallel pattern visualized by the programmer on a purely sequential program. With that information it transforms the code to parallel behind the scenes using the specified charateristics that one might prefer.

TBB is an optimized parallel programming interface which uses its own syntax in order to implement structure parallel programming, however abstracting some of the resource usage from the user and optimizing it by different ways regarding the various directives that may be specified by the programmer.

In the same way Fastflow offers an abstraction for programmers that can implement structured parallel programming. However, differntly from Intel TBB, its modeled tries its best to represent the design patterns' conceptual paradigm. Making it easier to tweek different aspects of parallel execution, but at the price of needing more effort to extract its full performance.

The native C++ threads library basically offers the thread abstraction on which the developer should explicitily write all the needed code such as thread safe queues, stages communication and others. It offers high performance and customization, and for this same reason, high programmability effort.

