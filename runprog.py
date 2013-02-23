#!/usr/bin/env python
import logging
import sys
import os
import time

def print_usage():
    print(
        """
Usage: ./runprog time program_name [program_arguments ...]
        
time                The amount of time to run the program. Use the format
                    HH:MM:SS where "HH" is hours, "MM" is minutes, and "SS"
                    is seconds. For eample, five hours, six seconds will be
                    written as "05:00:06"
        
program-name        The name of the program to run

program_arguments   Arguments to supply to the program program_name.
"""
    )

def main():
    if (argc != 3):
        print_usage()
        sys.exit()
        
    prog_name = argv[2]
    time_s = argv[1]
    prog_args = argv[3:]
    
    time_a = ':'.split(time_s)
    sleep_time = int(time[2]) + (int(time[1]) * 60) + (int(time[0]) * 60
        * 60) # number of seconds to sleep
    
    logger.debug("Will sleep for %d seconds." % (sleep_time))
    
    pid = os.fork()
    if (pid == 0): # child
        execvp(prog_name, prog_args)
    else:   # parent
        time.sleep(sleep_time)
        os.kill(pid)
        logger.info("Killed process %d" % pid)
    
if __name__=="__main__":
    main()