#!/usr/bin/env python
#import logger
import logging, sys, os, time, subprocess, signal, shutil, pexpect

sys_reserved = ["sudo"]

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

def run_for_time(t, args):
    if (len(sys.argv) < 3):
        print_usage()
        sys.exit()
    
    time_a = (t).split(":")
    if (len(time_a) != 3):
        logging.error("Time '%s' in wrong format." % (time_s))
    
    sleep_time = int(time_a[2]) + (int(time_a[1]) * 60) + (int(time_a[0])
        * 60 * 60) # number of seconds to sleep
    
    logging.debug("Will sleep for %d seconds." % (sleep_time))
    
    proc = subprocess.Popen(args)
    
    time.sleep(sleep_time)
    
    try:
        proc = subprocess.Popen(['sudo', 'kill', '-9', str(proc.pid)])
        logging.debug("Successfully kill process %d." % (proc.pid))
        for pa in args:
            if (pa not in sys_reserved):
                subprocess.Popen(['sudo', 'killall', '-9', pa])
                logging.debug("Also killing process '%s'..." % (pa))
            else:
                logging.warn("Process '%s' is a system-reserved word."
                    % (pa))
    except e:
        logging.error(e)
        
##
# Human-readable utility function for declaring which test
# is running
# @param list arg_time A list consisting of
#   <tt>(hours, minutes, seconds)</tt>
# @param int iter The iteration number.
def print_test_header(arg_time, iter):
    init_header = "## TEST (%d) - Time %s h, %s m, %s s ##" % (iter,
        arg_time[0], arg_time[1], arg_time[2])
    border = "#" * len(init_header)
    
    logging.info(border)
    logging.info(init_header)    
    logging.info(border)

def main():
    # First check for the gmon.out file.
    """
    try:
        gmon_file = open('gmon.out', 'r')
    except IOError:
        logging.error("Could not find gmon.out. Did you compile your" +
            " program with the '-pg' option?")
        sys.exit()
    """
        
    # Next, check that the user has valid arguments.
    if (len(sys.argv) < 3):
        logging.error("Invalid usage.")
        print_usage()
        sys.exit()
        
    # Store the arguments in variables.
    arg_time = sys.argv[1]
    arg_args = sys.argv[2:]
    
    # Generate a new gmon.out file, bumping up the last file index.
    file_index = 1
    while (os.path.isfile("gmon.%d.out" % (file_index))):
        ++file_index
    new_gmon = "gmon.%d.out" % (file_index)
    
    # Start a loop in case we have a time list
    loop = 0
    for at in arg_time.split(","):
        print_test_header(at, ++loop)
        run_for_time(at, arg_args)
        logging.info("Moving 'gmon.out' to '%s'..." % (new_gmon))
        shutil.copyfileobj(file("gmon.out"), file(new_gmon))
    sys.exit()
    
if __name__=="__main__":
    logging.basicConfig(level=logging.DEBUG)
    main()