#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <errno.h>
#include <pthread.h>
#include <signal.h>

#define _GNU_SOURCE
#include <getopt.h>

#define _STR_LEN 100
#define _FILE_LEN 10000
#define _MAX_PIDS 100
#define _DEV 0

// Define functions.
int send_tests(FILE *handle, int socket, struct addrinfo *address);

// Define global structs

struct struct_args_send {
    FILE *file_handle;
};

// define global variables

int client_received = 1;

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
    "\n"
    "                           isn't given, the default will be to output to\n"
    "                           the terminal. e.g. /home/user/log.txt";
    printf("%s\n", help);
    return 1;
}

int print_usage(){
    print_help();
}

char *get_current_time_str(const char *format){
    char *time_str = (char *) malloc(_STR_LEN);
    time_t the_time;
    (void) time(&the_time);
    struct tm *tmp_ptr = localtime(&the_time);
    strftime(time_str, _STR_LEN, format, tmp_ptr);
    return time_str;
}

int send_tests(FILE *handle, int socket, struct addrinfo *address){
    
    // Initialize the result variables.
    int res = 0;
    char *res_str = (char *) malloc(_STR_LEN);
    
    // Variables related to the file.
    char *line = (char *) malloc(_FILE_LEN);
        
    if (handle == NULL){
        printf("Handle is null.");
        exit(EXIT_FAILURE);
    }
    
    // Time variables.
    struct timespec *time_sent = (struct timespec *)
        malloc(sizeof (struct timespec));
    struct timespec *tp = (struct timespec *)
        malloc(sizeof (struct timespec ));
    
    int c = fgetc(handle);
    int l = -1;
    int p = -1;
    
    // Try connecting to the socket.
    res = connect(socket, address->ai_addr, (size_t) 14);
    
    if (res == -1){
        perror("Connecting to socket");
        return -1;
    }
    
    char *message = (char *) malloc(_STR_LEN);
    while(c != EOF){
        if (c == '\n'){
            printf("Line is %s.\n", line);
            // get the current timestamp
            clock_gettime(CLOCK_REALTIME, tp);
            sprintf(message, "%s:%d.%ld", line, (int) tp->tv_sec, tp->tv_nsec);
            printf ("Sending %s to %s:%s\n", message,
                address->ai_canonname, address->ai_canonname);
            res = send(socket, message, _STR_LEN, MSG_DONTROUTE|MSG_DONTWAIT);
            if (res == -1){
                perror(message);
                exit(EXIT_FAILURE);
            }
            line = NULL;
            line = (char *) malloc(_FILE_LEN);
            l = -1;
        }else{
            line[++l] = c;
        }
        c = fgetc(handle);
    }
    printf("File completed.");
    client_received = 0;
    exit(EXIT_SUCCESS);
}

int main(int argc, char *argv[]){
    
    struct addrinfo address;
    struct addrinfo *info;
    struct sockaddr_in6 *addr6;

    int opt;
    
    char *target_host;
    char *target_port;
    char *input_file_path;
    char *log_file_path;
    char *log_file_path_parsed;
    
    FILE *input_file;
    FILE *log_file;
    
    char *host = NULL;
    int sock = socket(AF_UNIX, SOCK_STREAM, 0);
    int goal_us = 0;
    struct tm *log_start;
    char *log_start_str;
    
    //pthreads
    pthread_t pthread_send_tests;
    pthread_t pthread_get_response;
    pthread_t log_response;
    
    void *thread_result;
    
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
    
    int i;
    for (i = 0; i < argc; ++i){
       if (strcmp("-h", argv[i]) == 0 || strcmp("--help", argv[i]) == 0){
           print_help();
           return 1;
       }
    }
    
    opt = -1;
    
    // Deal with the command line paramaters
    while ((opt = getopt_long(argc, argv, ":H:p:i:g:nho:",
        longopts, NULL)) != -1){
        switch(opt){
            case 'H':
                target_host = (char *) malloc(strlen(optarg));
                strncpy(target_host, optarg, strlen(optarg));
                break;
            case 'p':
                target_port = (char *) malloc(strlen(optarg));
                strncpy(target_port, optarg, strlen(optarg));
                break;
            case 'i':
                input_file_path = (char *) malloc(strlen(optarg)*4);
                strncpy(input_file_path, optarg, strlen(optarg));
                break;
            case 'o':
                log_file_path = (char *) malloc(strlen(optarg)*4);
                strncpy(input_file_path, optarg, strlen(optarg));
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
    
    addr6 = ((struct sockaddr_in6 *) malloc(sizeof (struct sockaddr_in6)));
    memset(&address, 0, sizeof address);
    memset(&info, 0, sizeof info);
    memset(&addr6, 0, sizeof addr6);
    
    long prt = atol(target_port);
    addr6->sin6_port = *(( int long *) malloc (sizeof prt));
    addr6->sin6_port = prt;
    addr6->sin6_addr = *((struct in6_addr *)malloc(sizeof (struct in6_addr)));
    strncpy(addr6->sin6_addr.s6_addr, target_host, _STR_LEN);
    
    address.ai_addr = (struct sockaddr *) addr6;
    address.ai_socktype = SOCK_STREAM;
    address.ai_flags = AI_PASSIVE;
    address.ai_family = AF_INET6;
    
    // Get the info of the address.
    int res = 0;
    res =  getaddrinfo(target_host, target_port, &address, &info);
    if (res != 0){
        perror("! Getting address info");
        printf("gai reported: %s.\n", gai_strerror(res));
        return -1;
    }
    printf("AI Info: %s\n", info->ai_addr->sa_data);
    
    // Get a string representing the time the log file was started.
    log_start_str = get_current_time_str("%Y-%m-%d %I:%M:%S %p");
    printf("Staring log file at %s.\n", log_start_str);
    
    // Get the files from the file path.
    FILE *infile = fopen(input_file_path, "rb");
    if (infile == NULL){
        perror(input_file_path);
        exit(EXIT_FAILURE);
    }
    send_tests(infile, sock, info);
}
