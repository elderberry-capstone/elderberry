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
#define _SOCK_LEN 1000
#define _FILE_LEN 10000
#define _MAX_PIDS 100
#define _DEV 0

// Define functions.
int send_tests(FILE *handle, int socket, struct addrinfo *hint);

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
    char *time_str = (char *) malloc(_STR_LEN * sizeof (char *));
    time_t the_time;
    (void) time(&the_time);
    struct tm *tmp_ptr = localtime(&the_time);
    strftime(time_str, _STR_LEN, format, tmp_ptr);
    return time_str;
}

int send_tests(FILE *handle, int socket, struct addrinfo *hint){
    
    // Initialize the result variables.
    int res = 0;
    char *res_str = (char *) malloc(_STR_LEN);
    memset(res_str, 0, _STR_LEN);
    
    // Variables related to the file.
    char *line = (char *) calloc(_FILE_LEN, sizeof (char));
        
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
    res = connect(socket, hint->ai_addr, (size_t) 14);
    
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
                hint->ai_canonname, hint->ai_canonname);
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

int print_socket_info(int sock_fd, struct sockaddr_in *sin){
        char dbg[_STR_LEN];
        memset(dbg, 0, sizeof(dbg));
        inet_ntop(AF_UNSPEC, &(sin->sin_addr.s_addr), &dbg, _STR_LEN);
        printf("============ SOCKET INFORMATION =============\n");
        printf("!** sock: %d\n", sock_fd);
        printf("!** info->ai_addr: sockaddr_in(\n");
        printf("!**     sin_family:    %d\n", sin->sin_family);
        printf("!**     sin_port:      %d\n", sin->sin_port);
        printf("!**     sin_addr: in_addr(\n");
        printf("!**         s_addr: '%s'\n", dbg);
        printf("!**     )\n)\n");
        printf("=============================================\n");
        return 1;
}

int main(int argc, char *argv[]){
    
    struct addrinfo *info;
    struct in_addr i_a;

    int opt;
    int res = 0;
    int use_inet6 = 0;
    
    char *target_host;
    char *target_port;
    char *input_file_path;
    char *goal;
    char *log_file_path;
    char *log_file_path_parsed;
    
    FILE *input_file;
    FILE *log_file;
    
    char *host = NULL;
    int sock = socket(AF_INET6, SOCK_STREAM, 0);
    if (sock == -1){
    	perror("!** Creating socket failed");
	    exit(EXIT_FAILURE);
    }
    int goal_us = 0;
    struct tm *log_start;
    char *log_start_str;
    
    //pthreads
    pthread_t pthread_send_tests;
    pthread_t pthread_get_response;
    pthread_t log_response;
    
    void *thread_result;
    
    // Deal with the command line paramaters
    struct option longopts[] = {
        {"host",    1,  NULL,   'H'},
        {"port",    1,  NULL,   'p'},
        {"goal-ms", 2,  NULL,   'g'},
        {"inet6",   2,  NULL,   'i'},
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
    int size = 0;
    int len = 0;
    while ((opt = getopt_long(argc, argv, ":H:p:g:ih", longopts, NULL)) != -1){
	    size = sizeof(optarg);
	    if (optarg != NULL)
            len = strlen(optarg);
        printf("Opt is %c and optarg is %s.\n", opt, optarg);
        switch(opt){
            case 'H':
                printf("Host is %s\n", optarg);
            	target_host = (char *) malloc(size);
            	strncpy(target_host, optarg, len);
                break;
            case 'p':
                printf("Port is %s\n", optarg);
            	target_port = (char *) malloc(size);
            	strncpy(target_port, optarg, len);
                break;
            case 'i':
                use_inet6 = 1;
                break;
            case 'g':
                goal = (char *) malloc(size);
                strncpy(goal, optarg, len);
                break;
            case 'h':
                print_help();
                break;
            default:
                print_usage();
                break;
        }
    }
    
    short fam = (use_inet6)? AF_INET6 : AF_UNSPEC;
    
    
    // Set sockadder_in structure
    struct sockaddr_in sa_in = {
        .sin_family =   fam,
        .sin_port   =   htons(atol(target_port)),
        .sin_addr   =   i_a
    };
    
    // Set the in_addr structure
    inet_pton(fam, target_host, &(sa_in.sin_addr), INET_ADDRSTRLEN);
    char dbg1[INET_ADDRSTRLEN];
    inet_ntop(fam, &(sa_in.sin_addr), dbg1, INET_ADDRSTRLEN);
    printf ("i_a is %s\n\n", dbg1);
    
    print_socket_info(sock, &sa_in);
    
    // Clear sockadder_in.sin_zero
    memset(sa_in.sin_zero, 0, sizeof(strlen));
    
    
    // Set the addrinfo structure.
    struct addrinfo hint = {
        .ai_flags=      AI_PASSIVE,
        .ai_family=     fam,
        .ai_socktype=   SOCK_STREAM,
        .ai_protocol=   0,
        .ai_addrlen =   _STR_LEN,
        .ai_addr =      (struct sockaddr *) (&sa_in)
    };
    
    res =  getaddrinfo(target_host, target_port, NULL, &info);
    if (res != 0){
        perror("!** Getting address info");
        printf("!** gai reported: %s.\n", gai_strerror(res));
        struct sockaddr_in *i6 = (struct sockaddr_in *) info->ai_addr;
        perror("!** Binding failed.");
        printf("!** Environment:\n");
        //print_socket_info(sock, i6);
        exit(EXIT_FAILURE);
        return -1;
    }

    if (bind(sock, (info->ai_addr), _SOCK_LEN) == -1){
        struct sockaddr_in *i6 = (struct sockaddr_in *) info->ai_addr;
        perror("!** Binding failed.");
        print_socket_info(sock, i6);
	    exit(EXIT_FAILURE);
    }

    return 1;
} 