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
    t2 = (int(t[0]) * 3600) + (int(t[1]) * 60) + int(t[2])
    logging.info("Running program:")
    logging.info("\t%s" % (" ".join(args)))
    logging.info("Time: %d seconds" % (t2))
    
    proc = subprocess.Popen(args)
    
    time.sleep(t2)
    
    logging.debug("Killing process '%d'" % (proc.pid))
    subprocess.call(["sudo", "kill -9", str(proc.pid)])
    for ar in args:
        if (ar not in sys_reserved):
            logging.debug("Also killing '" + ar + "'")
            subprocess.Popen("sudo killall -9 " + ar)
    
    #os.kill(proc.pid, 9)
        
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
    
def choose_new_gmon(i = 0):
    if (not os.path.exists("gmon.out")):
        pexpect.run('touch gmon.out')
    elif (os.path.exists("gmon." + str(i) + ".out")):
        return choose_new_gmon(i+1)
    else:
        return "gmon." + str(i) + ".out"
    

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
    
    # Start a loop in case we have a time list
    loop = 0
    for at in arg_time.split(","):
        new_gmon = choose_new_gmon()
        loop += 1
        print_test_header(at.split(":"), loop)
        run_for_time(at.split(":"), arg_args)
        if (new_gmon != None):
            logging.debug("Copying 'gmon.out' to '" + new_gmon + "'")
            shutil.copy("gmon.out", new_gmon)
            
    sys.exit()
    
if __name__=="__main__":
    logging.basicConfig(
        format="%(levelname)s\t[%(asctime)s]\t%(msg)s",
        level=logging.DEBUG)
    main()