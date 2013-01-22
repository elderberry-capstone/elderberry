#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <errno.h>

#define _GNU_SOURCE
#include <getopt.h>

#define _STR_LEN 100
#define _DEV 1

int print_help(){
    const char *help = " TestRun \n\n"
    "Usage: ./testrun {options}, where {options} is the following:\n\n"
    "   -H, --host=[name]       (REQUIRED) The host name of the intended \n"
    "                           server. e.g., localhost\n\n"
    "   -p, --port=[number]     (REQUIRED) The port number of the intended\n" 
    "                           server. e.g. 1818\n\n"
    "   -i, --input=[path]      (REQUIRED) A newline-deliniated file containing"
    "\n"
    "                           input for the test. e.g. /home/user/test001.txt"
    "\n\n"
    "   -u, --goal-us [number]  (optional) The microsecond goal.  Any\n"
    "                           connection that takes longer than this will \n"
    "                           automaticically be a FAIL. e.g. 12000\n\n"
    "   -n, --inet6             (optional) Whether we're using inet6 protocol."
    "\n\n"
    "   --logfile -o [path]     (optional) Output to this file.  If this option"
    "\n\n"
    "                           isn't given, the default will be to output to\n"
    "                           the terminal. e.g. /home/user/log.txt";
    printf("%s\n", help);
    return 1;
}

int print_usage(){
    print_help();
}

char *get_current_time_str(const char *format){
    time_t the_time;
    the_time = time((time_t *)0);
    struct tm *timeptr = gmtime(&(the_time));
    char *time_str = (char *) malloc(_STR_LEN);
    strftime(time_str, _STR_LEN, format, timeptr);
    return time_str;
}

int main(int argc, char *argv[]){
    //printf("You provided %d arguments.\n", (argc-1));
    struct sockaddr_in  address;
    struct sockaddr_in6 sai6;
    int opt;
    
    char *input_file_path;
    char *log_file_path;
    char *log_file_path_parsed;
    
    FILE *input_file;
    FILE *log_file;
    
    char *host = NULL;
    int sock = socket(AF_UNIX, SOCK_STREAM, 0);
    int goal_us = 0;
    struct timespec *time_sent = (struct timespec *) malloc(sizeof (struct timespec));
    struct tm *log_start;
    char *log_start_str;
    
    if ((argc < 4 || argc > 7) && _DEV == 0){
        print_usage();
        return -1;
    }
    
    // Options
    struct option longopts[] = {
        {"host",    1,  NULL,   'H'},
        {"port",    1,  NULL,   'p'},
        {"input",   1,  NULL,   'i'},
        {"goal-ms", 2,  NULL,   'g'},
        {"inet6",   2,  NULL,   'n'},
        {"logfile", 2,  NULL,   'o'},
        {"help",    2,  NULL,   'h'},
        {0,         0,  0,      0}
    };
    
    while ((opt = getopt_long(argc, argv, ":H:p:i:g:nho:",
        longopts, NULL)) != -1){
        if (opt == 'h'){
            print_help();
            return 1;
        }
        if (!(opt == 'H' || opt == 'p' || opt == 'i' || opt == 'g' || opt == 'n'
            || opt == 'n' || opt == 'h' || opt == '0' )){
                print_usage();
                return -1;
            }
    }
    
    opt = 0;
    
    if (_DEV == 0){
    // Deal with the command line paramaters
    while ((opt = getopt_long(argc, argv, ":H:p:i:g:nho:",
        longopts, NULL)) != -1){
        switch(opt){
            case 'H':
                inet_pton(AF_INET,   optarg, &(address.sin_addr));
                break;
            case 'p':
                inet_pton(AF_INET,  optarg, &(address.sin_port));
                break;
            case 'i':
                input_file_path = (char *) malloc(strlen(optarg)*4);
                strncpy(input_file_path, optarg, _STR_LEN);
                break;
            case 'o':
                log_file_path = (char *) malloc(strlen(optarg)*4);
                strncpy(input_file_path, optarg, _STR_LEN);
                break;
            case 'g':
                goal_us = atoi(optarg);
                break;
            case 'h':
                print_help();
                return 1;
            default:
                print_usage();
                break;
        }
    }
    }else{
        //print_usage();
        inet_pton(AF_INET,   "127.0.0.1", &(address.sin_addr));
        inet_pton(AF_INET,  "8081", &(address.sin_port));
        input_file_path = "testin.txt";
        log_file_path = "testlog.%Y-%m-%d.%H:%M:%S.txt";
    }
    
    // Get a string representing the time the log file was started.
    log_start_str = get_current_time_str("%Y-%m-%d %h:%M:%S %p");
    printf("Staring log file at %s.\n", log_start_str);
    
    // Get the exact time the message was sent.
    if(clock_gettime(CLOCK_REALTIME, time_sent) == -1)
        perror("clock_gettime reported: ");
    printf("Socket message sent: seconds: %s, micro-seconds: %d\n",
        ctime(&(time_sent->tv_sec)), (int) time_sent->tv_nsec
    );
    
    input_file=fopen(input_file_path, "r");
    return 1;
}
