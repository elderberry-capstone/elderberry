#!/usr/bin/env python
#import logger
import logging, sys, os, time, shutil, pexpect, subprocess, re

sys_reserved = ["sudo"]
SIG_PROF_KILL = 2929
SIGQUIT = 3
SIGINT = 2
STDOUT = 1
PIPE_FD = 3

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
    
    
class GProfOut:
    def __init__(self):
        self.time_run = 0
        self.functions = {}
        
    def tostring(self):
        str = "\n"
        for (name, f) in self.functions.iteritems():
            str += f.tostring()
        str += ">> Run time for %s: %s seconds" % (name, self.time_run) 
        return str
        
        
class CFunc:
    def set_func(self, perc_time, cumu_sec, self_sec, calls, self_sec_per_call,
                 tot_sec_per_call, name):
        
        self.perc_time = perc_time
        self.cumu_sec = cumu_sec
        self.self_sec = self_sec
        self.calls = calls
        self.self_sec_per_call = self_sec_per_call
        self.tot_sec_per_call = tot_sec_per_call
        self.name = name
       
    def __init__(self, gprof_list=None):
        if (gprof_list != None):
            self.set_func(float(gprof_list[0]),
                          float(gprof_list[1]),
                          float(gprof_list[2]),
                          float(gprof_list[3]),
                          gprof_list[4],
                          gprof_list[5],
                          gprof_list[6])
        
    def tostring(self):
        return"""
===== Function: %s ======
Percentage of time:         %d
Cumulative seconds:         %d
Self seconds:               %d
Number of calls:            %d
Seconds per call (self):    %s
Seconds per call (total):   %s
        """ % (self.name,
               self.perc_time,
               self.cumu_sec,
               self.self_sec,
               self.calls,
               self.self_sec_per_call,
               self.tot_sec_per_call)
##
# Run the program for the specified number of times.
# @param t: how long to run the program.
# @param args: The program arguments.
#
def run_for_time(t, args):
    pid = os.fork()
    outfd = 0
    sleep_time = t
    if (pid == 0):   # We're the child
        logging.info("The child prcess is %d" % (os.getpid()))
        logging.info("Will sleep for %d seconds" % (sleep_time))
        os.dup2(outfd, STDOUT)
        os.execvp(args[0], args)
    else:           # We're the parent.
        time.sleep(sleep_time)
        
        # Send the SIGINT signal
        os.kill(pid, SIGINT)
        
        os.dup2(STDOUT, outfd)
        
#
# Human-readable utility function for declaring which test
# is running
# @param list arg_time A list consisting of
#   <tt>(hours, minutes, seconds)</tt>
# @param int iter The iteration number.
def print_test_header(arg_time, iter):
    t = secs_to_arr(arg_time)
    init_header = "## TEST (%d) - Time %s h, %s m, %s s ##" % (iter,
        t[0], t[1], t[2])
    border = "#" * len(init_header)
    
    logging.info(border)
    logging.info(init_header)    
    logging.info(border)
    
def choose_new_gmon(i = 0):
    """
    if (not os.path.exists("gmon.out")):
        pexpect.run('touch gmon.out')
    elif (os.path.exists("gmon." + str(i) + ".out")):
        return choose_new_gmon(i+1)
    else:
        return "gmon." + str(i) + ".out"
    """
    if (os.path.exists("gmon." + str(i) + ".out")):
        return choose_new_gmon(i+1)
    else:
        return "gmon." + str(i) + ".out"
    
    
##
# Given a filename in gprof format, parse its information
# for analysis.
# @param filename: The filename in gprof format.
# @return A GProfOut object representing the information.
def parse_output_file(filename):
    line_regex = '((\d)?\s*\d+\.?\d+)+\s+\w*'
    split_regex = '(\s){1,7}'
    ws_regex = '(\s)+'
    
    re_line = re.compile(line_regex)
    re_split = re.compile(split_regex)
    re_space = re.compile(ws_regex)
    
    p = subprocess.Popen("gprof -b --flat-profile ./fc %s" % (filename), shell=True,
                    stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
    lines = []
    line = ""
    lineno = 0
    cont = True
    while (True):
        line = p.stdout.readline()
        if re.match(re_line, line) != None:
            lines.append(line)
        if line == "" and p.poll() != None:
            break
            
    print("\n")
    funclist= []
    for l in lines:
        lines_list = re.split(re_split, l)
        lines_list2 = []
        for ll1 in lines_list:
            if ll1 not in ['', ' ', '  ', '\n', '\t']:
                lines_list2.append(ll1)
        funclist.append(lines_list2)
                
    gpo = GProfOut()
    for fl in funclist:
        #print fl
        gpo.functions[fl[6]] = CFunc(fl)
    
    return gpo

def arr_to_secs(time):
    return (int(time[0]) * 3600) + (int(time[1]) * 60) + int(time[2])

def secs_to_arr(time):
    return [
        (time / 3600) % 3600,
        (time / 60 ) % 60,
        (time % 60) % 60
    ]

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
    times = []
    gmon_times = {}
    gmon_files = []
    for t in arg_time.split(","):
        tim = arr_to_secs(t.split(":"))
        times.append(tim)
    print (times)
    
        
    for ta in times:
        new_gmon = choose_new_gmon()
        gmon_times[new_gmon] = times
        gmon_files.append(new_gmon)
        loop += 1
        print_test_header(ta, loop)
        run_for_time(ta, arg_args)
        if (new_gmon != None):
            logging.debug("Copying 'gmon.out' to '" + new_gmon + "'")
            shutil.copy("gmon.out", new_gmon)
            
    # Analyze the program.
    # The order of the files is gmon.0.out, gmon.1.out, ..., gmon.n.out
    # 
    logging.info("\n\n###### Analysis stage #######")
    logging.debug("Files: %s" % (gmon_files))
    
    testno = -1
    gresults = []
    for gfile in gmon_files:
        testno = testno+1
        gout = parse_output_file(gfile)
        try:
            gout.time_run = gmon_times[gfile][testno]
        except:
            pass
        
        print('='*10 + " TEST " + str(testno+1) + " " + '='*10)
        print(gout.tostring())
        gresults.append(gout)
            
    logging.debug("%d tests run." % (len(gresults)))
    sys.exit()
    
if __name__=="__main__":
    logging.basicConfig(
        format="%(levelname)s\t[%(asctime)s]\t%(msg)s",
        level=logging.DEBUG)
    main()
