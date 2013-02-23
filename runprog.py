#!/usr/bin/env python
#import logger
import logging, sys, os, time, subprocess, signal

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
    logging.basicConfig(level=logging.DEBUG)
    if (sys.argv < 3):
        print_usage()
        sys.exit()
        
    prog_name = sys.argv[2]
    time_s = sys.argv[1]
    prog_args = sys.argv[2:]
    
    time_a = (time_s).split(":")
    if (len(time_a) != 3):
        logging.error("Time '%s' in wrong format." % (time_s))
    
    sleep_time = int(time_a[2]) + (int(time_a[1]) * 60) + (int(time_a[0])
        * 60 * 60) # number of seconds to sleep
    
    logging.debug("Will sleep for %d seconds." % (sleep_time))
    
    pid = os.fork()
    if (pid == 0): # child
        call_args = ["sudo"] + prog_args
        logging.debug("Executing arguments: %s" % (call_args))
        subprocess.call(call_args)
        
    else: #parent
        time.sleep(sleep_time)
        os.kill(pid, signal.SIG_DFL)
        logging.info("Killed process %d" % pid)
    
if __name__=="__main__":
    main()