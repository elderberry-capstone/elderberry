PROFILER README
===============

The gmon output of the program is [here](./gprof_output_1).

As shown in the chart, the program was run 1000000 (1 million) times and has a
total runtime of 20.22 seconds:

> time   seconds   seconds    calls  ms/call  ms/call  name    
101.10      0.02     0.02        1    20.22    20.22  loop
  0.00      0.02     0.00  1000000     0.00     0.00  hello
  
Simple mathematics will show that the hello() function ran in 2.022 x 10^-5
seconds or about 20.22 microseconds.

RUNNING THE PROGRAM
-------------------

In whichever file you have a function you want to test, include _looper.h_ in
with the rest of the imports:

>\#include "looper.h"

And, finally, for the function _func()_ that you want to test, add it as a
callback to the looper function, e.g.

>int f()\{
>    ...
>\}

>int g()\{
>    loop(f, _\[times\]_);
>\}

It would be helpful to have _time_ be a command line argument, as in the test
program below.

To run the program, compile as

>make _\[option\]_

Where option is either _all_, _debug_, or _profile_. (_all_ can be taken off).

Then run the program like the following:

>./a.out _\[number\]_

Where _\[number\]_ is the number of times you want the function to run.