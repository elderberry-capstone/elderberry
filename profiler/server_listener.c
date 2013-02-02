/*
 * server_listener.c
 * 
 * Copyright 2013 Jordan Hewitt <jordannh@sent.com>
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301, USA.
 * 
 * 
 */
 #include <libHX/option.h>
 #include <stdio.h>
 #include <stdlib.h>
 #include <string.h>
 #include "betterprint.c"
 
 #define SPLIT1 ','
 #define SPLIT2 ':'
 
struct socket_list {
    char *host;
    char *port;
    struct socket_list *next;
};

int main(int argc, const char **argv){
    
    turn_on(BP_ERR|BP_DBG|BP_VER|BP_WAR|BP_INF);
    
    struct socket_list *socklist;
    char *portBreak;
    int len = 0;
    int len2 = 0;
    
    if (argc == 1){
        print_e("Please include arguments of hosts and ports, e.g.:");
        print_e("%s 127.0.0.1:30 127.0.0.9:82", argv[0]);
        exit(EXIT_FAILURE);
    }
    
    int hostlen, hostsize;
    
    struct socket_list *current = socklist; // head of socklist;
    
    // loop through the arguments and get the host and ports.
    for (int i = 1; i < argc; ++i){
        len2 = strlen(argv[i]);
        
        portBreak = strstr(argv[i], ":");
        //portBreak[0] = 0;   // Remove the ':'
        print_dbg("Port is '%s'.", portBreak);
        
        hostlen = strlen(argv[1]) - strlen(portBreak) + 1;
        hostsize = hostlen * sizeof (char);
        print_dbg("hostsize is %d and hostlen is %d.", hostsize, hostlen);
        current->host = malloc(hostsize);
        current->port = malloc(sizeof ((char *)portBreak));
        
        strncpy(current->host, argv[1], hostlen);
        strncpy(current->port, portBreak, strlen(portBreak));
        
        struct socket_list *nxt;
        socklist->next = nxt;
    }
}

int split_into(const char *haystack, const char *needles, ...){
    
    va_list ap;
    va_start(ap, )
    
}
