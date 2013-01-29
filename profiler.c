#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/wait.h>
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
#define _SHOW_ERR 1
#define _BE_VERBOSE 1
#define _DEBUG 1
// Debugging macros
#define bDBG if (_DEBUG) printf("[DEBUG]\t" 
#define eDBG if );
#define bERR if (_SHOW_ERR) printf("[ERROR]\t" 
#define eERR );
#define bVER if (_BE_VERBOSE) printf("[VERBOSE]\t" 
#define eVER );


// Define functions.
int send_tests(FILE *handle, int socket, struct addrinfo *hint);
char *fam2str(int fam);
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
    "                           input for the test. e.g. /home/user/test001.txt"
    "\n\n"
    "   -u, --goal-us=[number]  (optional) The microsecond goal.  Any\n"
    "                           connection that takes longer than this will \n"
    "                           automaticically be a FAIL. e.g. 12000\n\n"
    "   -i, --inet=[4|6]        (optional) Protocol we're using."
    "\n\n";
    printf("%s\n", help);
    return 1;
}

int print_usage(){
    print_help();
}

int debug(){
    printf("\n[DEBUG]\t\t");
    return 1;
}

int error(){
    printf("\n[ERROR]\t\t");
    return 1;
}

int verb(){
    printf("\n[VERBOSE]\t\t");
    return 1;
}

int warn(){
    printf("\n[WARNING]\t\t");
    return 1;
}

char *get_current_time_str(const char *format){
    char *time_str = (char *) malloc(_STR_LEN * sizeof (char *));
    time_t the_time;
    (void) time(&the_time);
    struct tm *tmp_ptr = localtime(&the_time);
    strftime(time_str, _STR_LEN, format, tmp_ptr);
    return time_str;
}

int print_socket_info(const int sock_fd, const struct sockaddr_in *sin, short protocol){
    char dbg[INET_ADDRSTRLEN];
    char *famstr;
    inet_ntop(protocol, &(sin->sin_addr), dbg, INET_ADDRSTRLEN);
    debug();
    printf("============ SOCKET INFORMATION =============" );
    debug();
    printf("socket: %d", sock_fd );
    debug();
    printf("info->ai_addr: sockaddr_in(" );
    debug();
    famstr = fam2str(sin->sin_family);
    debug();
    printf("    sin_family:    %s", famstr );
    debug();
    printf("    sin_port:      %d", ntohs(sin->sin_port) );
    debug();
    printf("    sin_addr:      in_addr( s_addr : '%s' )", dbg );
    debug();
    printf(")");
    debug();
    printf("=============================================" );
    return 1;
}

char *fam2str(int fam){
    switch (fam){
        case AF_INET:
            return "AF_INET";
        case AF_INET6:
            return "AF_INET6";
        case AF_UNSPEC:
            return "AF_UNSPEC";
        default:
            return "(UNKNOWN)";
    }
    return "(UNKNOWN)";
}

int pipe_in_loop(const int *sock){
    // Start a new thread.
    int pid_receive_test = fork();
    int status = 0;
    switch(pid_receive_test){
        case -1:
            perror("Forking error");
            exit(EXIT_FAILURE);
            return -1;
        case 0: // child
            debug();
            printf("Starting child process %d\n", pid_receive_test);
            get_test_input(sock);
            break;
        case 1: // parent
            if (waitpid(pid_receive_test, &status, 0)){
                debug(); printf("Child %d exited.\n", pid_receive_test);
            }else{
                error(); printf("Child did not exit!\n");
            }
            return 1;
    }
    if (waitpid(pid_receive_test, &status, 0)){
        debug();
        printf("Child %d exited.\n", pid_receive_test);
        return 1;
    }
    else
        error();printf("Child did not exit!\n");
}

/**
 * Read in one character at a time.  If we come to
 * a newline character, add that as a word and print it
 * out to the console.
 * Otherwise, keep on reading.
 **/
int get_test_input(const int *sock){
    int pip = dup(0);
    int c = 0;
    char word[_STR_LEN];
    memset(word, 0, sizeof word);
    int i = -1;
    int res = 0;
    while (1){
        c = getchar();
        if ((char) c == '\n'){  // if we've reached a new line.
            printf("Read '%s'.\n", word);
            char *now = get_current_time_str("%Y-%m-%d %I:%M:%S %p");
            res = send(*sock, &word, _STR_LEN, MSG_DONTROUTE);
            if (res == -1){
                printf("[%s]\tERROR: File descriptor: %d\n", now, *sock);
                printf("[%s]\tERROR:", now);
                perror("");
            }else{
                printf("[%s]\tMessage '%s' successfully sent to server.\n", now,
                    word);
            }
            
            i = -1;
            memset(word, 0, sizeof word);
        }else{
            word[++i] = (char) c;
        }
    }
    dup(pip);
    exit(EXIT_SUCCESS);
    return 1;
}

int main(int argc, char *argv[]){
    
    struct addrinfo *info;
    struct in_addr i_a;

    int opt;
    int res = 0;
    int use_inet6 = 0;
    int protocol = AF_UNSPEC;
    
    char *target_host;
    char *target_port;
    char *input_file_path;
    char *goal;
    char *log_file_path;
    char *log_file_path_parsed;
    
    FILE *input_file;
    FILE *log_file;
    
    char *host = NULL;
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
        {"inet",    2,  NULL,   'i'},
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
    int p = 0;
    while ((opt = getopt_long(argc, argv, ":H:p:g:i:h", longopts, NULL)) != -1){
	    size = sizeof(optarg) + sizeof(char);
	    if (optarg != NULL)
            len = strlen(optarg);
        printf("Opt is %c and optarg is %s.\n", opt, optarg);
        switch(opt){
            case 'H':
                printf("Host is %s\n", optarg);
            	target_host = (char *) malloc(INET_ADDRSTRLEN);
                memset(target_host, 0, sizeof(target_host));
            	strncpy(target_host, optarg, INET_ADDRSTRLEN);
                break;
            case 'p':
                printf("Port is %s\n", optarg);
            	target_port = (char *) malloc(size);
                memset(target_port, 0, sizeof(target_port));
            	strncpy(target_port, optarg, len+10);
                break;
            case 'i':
                p = atoi(optarg);
                if (4 == p){
                    protocol = AF_INET;
                }else if (p){
                    protocol = AF_INET6;
                }else{
                    printf("Please specify either protocol 4 or 6.\n");
                    return -1;
                }
                printf("Setting protocol to inet%d\n", p);
                break;
            case 'g':
                goal = (char *) malloc(size);
                strncpy(goal, optarg, len);
                break;
            case 'h':
                print_help();
                return -1;
                break;
            default:
                print_usage();
                return -1;
                break;
        }
    }
    
    int sock = socket(protocol, SOCK_STREAM, 0);
    if (sock == -1){
    	perror("!** Creating socket failed");
	    exit(EXIT_FAILURE);
    }
    
    struct in_addr ia = { .s_addr = 0};
    
    // Set sockadder_in structure
    struct sockaddr_in sa_in = {
        .sin_family =   protocol,
        .sin_port   =   htons(atol(target_port)),
        .sin_addr   =   ia
    };
    
    // Set the in_addr structure
    res = inet_pton(protocol, target_host, &(sa_in.sin_addr));
    
    if (res == 0){
        printf("!** %s does not contain a character string representing\n"
        "!** a valid network address in the specified address family.\n",
        target_host);
        exit(EXIT_FAILURE);
        return -1;
    }else if (res == -1){
        perror("Error setting host.");
        exit(EXIT_FAILURE);
    }
    
    // Clear sockadder_in.sin_zero
    memset(sa_in.sin_zero, 0, sizeof(strlen));
    
    struct addrinfo hint;
    memset(&hint, 0, sizeof hint);
    memset(&info, 0, sizeof info);
    // Set the addrinfo structure.
    hint.ai_flags=      AI_PASSIVE;
    hint.ai_family=     protocol;
    hint.ai_socktype=   SOCK_STREAM;
    hint.ai_protocol=   0;
    hint.ai_addr =      (struct sockaddr *) (&sa_in);
    hint.ai_addrlen =   INET_ADDRSTRLEN;
    
    res =  getaddrinfo(target_host, target_port, &hint, &info);
    if (res != 0){
        perror("!** Getting address info");
        printf("!** gai reported: %s.\n", gai_strerror(res));
        exit(EXIT_FAILURE);
        return -1;
    }
    
    // Get the size of the list
    struct addrinfo *rp;
    for (rp = info; rp != NULL; rp = rp->ai_next){
        printf("==> Another element.\n");
        print_socket_info(sock, (struct sockaddr_in *) rp->ai_addr, protocol);
    }
    
    /**
    if (bind(sock, (info->ai_addr), INET_ADDRSTRLEN) == -1){
    //if (bind(sock, (hint.ai_addr), INET_ADDRSTRLEN) == -1){
        perror("!** Binding failed");
        print_socket_info(sock, (struct sockaddr_in *) info->ai_addr, protocol);
	    exit(EXIT_FAILURE);
    }else{
        printf("# Binding to host %s on port %s succeeded.\n",
         target_host, target_port);
    }**/
    res = connect(sock, info->ai_addr, INET_ADDRSTRLEN);
    if (res == -1){
        error(); perror("Connecting to socket");
        exit(EXIT_FAILURE);
        return -1;
    }else{
        debug(); printf("Connected to socket.\n");
    }
    
    pipe_in_loop(&sock);
    
    //close(sock);
    return 1;
} 