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