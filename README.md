The repository's organization is defined as follows:

Spar folder
TBB folder
Fastflow folder
C++Threads folder

Each one consists of the Mandelbrot implementation, performance data and its graphs and personal report of what I had learnt about each one after using them at the time of GMAP Labs' Training route. (started this route at the end of 02/2021 - until 07/2021).

Parallel Programming Interfaces(PPIs) Used:

Spar makes use of simple annotations composed of the parallel pattern visualized by the programmer on a purely sequential program. With that information it transforms the code to parallel behind the scenes using the specified charateristics that one might prefer. However that advantage doesn't come without penalties. Its speedup is often lower than other lower level interfaces

TBB is an optimized parallel programming interface which uses its own syntax in order to implement structured parallel programming, however its abdstracts some of the resource usage from the user and optimezes it by different ways regarding the various directives that may not be specified by the programmer. Thus it may not implement exactly what an adavanced programmer may want to perform in lower layers of exceution.

In the same way Fastflow offers an abstraction for programmers that can implement structured parallel programming. However, differently from Intel TBB, it is modeled in a way to explicitily represent the design patterns' algorithms. That way it becomes easier to tweek different aspects of parallel execution, but at the price of needing more effort and knowledge to extract its full performance.

The native C++ threads library basically offers the thread abstraction on which the developer should explicitily write all the needed code such as thread safe queues, stages communication and others. It offers high performance and customization, but turns out to require a high programmability effort.

