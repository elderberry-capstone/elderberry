#include <arpa/inet.h>
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
#include "betterprint.c"

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


// Define functions.
int send_tests(FILE *handle, int socket, struct addrinfo *hint);
char *fam2str(int fam);
long get_time_diff(const int *socket, const char *message);


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
    return 1;
}

int get_current_sys_time_str(char *buff){
    struct timespec tp;
    clock_t clock_id = clock();
    if (clock_gettime(clock_id, &tp) == -1){
        perror("[ERROR]\t\tGetting time");
        return -1;
    }else{
                printf("Time is %d.%ld.\n", (int) tp.tv_sec, tp.tv_nsec);
    }
    memset(buff, 0, sizeof buff); 
    sprintf(buff, "%d.%ld", (int) tp.tv_sec, tp.tv_nsec);
    return 1;
}

int print_socket_info(const int sock_fd, const struct sockaddr_in *sin, short protocol){
    print_i("No socket info currently.");
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
    switch(pid_receive_test){
        case 0:
	    print_dbg("I am the child!");
        return 1;
	case -1:
	    print_e("I couldn't spawn a child.");
        return -1;
	default:
	    print_dbg("I am the parent.");
        return 1;
    }
    return 1;
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
    char word[_STR_LEN], mesg[_STR_LEN];
    long td_test1=0, td_test2=0, td_test3=0;
    memset(word, 0, sizeof word);
    memset(mesg, 0, sizeof mesg);
    int i = -1;
    char sentTime[BUFSIZ];
    while (1){
        c = getchar();
        if ((char) c == '\n'){  // if we've reached a new line.
            printf("Read '%s'.\n", word);
            
            //construct the message from the word and the systime string.
            get_current_sys_time_str(sentTime);
            sprintf(mesg, "%s:%s", word, sentTime);
            print_dbg("Sending message '%s'.\t", mesg);

            // Send the message (exit if we error).
            td_test1 = get_time_diff(sock, "");    //message of length 1
            td_test2 = get_time_diff(sock, "\0");  //message of length 2
            td_test3 = get_time_diff(sock, "\0\0");//message of length 3.

            print_i("Sending length 0 took %d nanoseconds.", td_test1);
            print_i("Sending length 1 took %d nanoseconds.", td_test2);
            print_i("Sending length 2 took %d nanoseconds.", td_test3);
            
            i = -1;
            memset(word, 0, sizeof word);
            memset(mesg, 0, sizeof mesg);
        }else{
            word[++i] = (char) c;
        }
    }
    dup(pip);
    exit(EXIT_SUCCESS);
    return 1;
}

long get_time_diff(const int *socket, const char *message){
    if(send(*socket, &message, _STR_LEN, MSG_DONTROUTE) == -1){
        print_e("File descriptor: %d", *socket);
        perror("");
        exit(EXIT_FAILURE);
    }else{
        print_l("Message '%s' successfully sent to server.", message);
    }
    return 10;
}

int main(int argc, char *argv[]){

    turn_on(BP_ERR|BP_DBG|BP_VER|BP_WAR|BP_INF);
    
    struct addrinfo *info;

    int opt;
    int res = 0;
    int protocol = AF_UNSPEC;
    
    char *target_host;
    char *target_port;
    char *goal;
    
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
        print_dbg("Opt is %c and optarg is %s.\n", opt, optarg);
        switch(opt){
            case 'H':
                print_dbg("Host is %s\n", optarg);
            	target_host = (char *) malloc(INET_ADDRSTRLEN);
                memset(target_host, 0, sizeof(target_host));
            	strncpy(target_host, optarg, INET_ADDRSTRLEN);
                break;
            case 'p':
                print_dbg("Port is %s\n", optarg);
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
                    print_w("Please specify either protocol 4 or 6.\n");
                    return -1;
                }
                print_dbg("Setting protocol to inet%d\n", p);
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
    	print_e("!** Creating socket failed%s", strerror(errno));
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
        print_e("%s does not contain a character string representing", target_host);
        print_e("a valid network address in the specified address family.");
        exit(EXIT_FAILURE);
        return -1;
    }else if (res == -1){
        print_e("Error setting host.");
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
        print_e("Getting address info");
        print_e("gai reported: %s.\n", gai_strerror(res));
        exit(EXIT_FAILURE);
        return -1;
    }
    
    // Get the size of the list
    struct addrinfo *rp;
    for (rp = info; rp != NULL; rp = rp->ai_next){
        print_dbg("==> Another element.\n");
        print_socket_info(sock, (struct sockaddr_in *) rp->ai_addr, protocol);
    }
    
    res = connect(sock, info->ai_addr, INET_ADDRSTRLEN);
    if (res == -1){
        print_e("Connecting to socket", strerror(errno));
        exit(EXIT_FAILURE);
        return -1;
    }else{
        print_i("Connected to socket.\n");
    }
    
    pipe_in_loop(&sock);
    
    //close(sock);
    return 1;
} 
