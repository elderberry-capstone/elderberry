#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdarg.h>
#include "betterprint.h"

#define ERROR "[ERROR]\t%s"
#define DEBUG "[DEBUG]\t%s"
#define VERBOSE "[VERBOSE]\t%s"
#define WARN "[WARN]\t%s"
#define INFO "[INFO]\t%s"

#define BP_ERR 0
#define BP_DBG 1
#define BP_VER 2
#define BP_WAR 3
#define BP_INF 4

int bp_err = 0;
int bp_dbg = 0;
int bp_ver = 0;
int bp_war = 0;
int bp_inf = 0;

int turn_on(int type){
    if (type == BP_ERR) bp_err = 1;
    else if (type == BP_DBG) bp_dbg = 1;
    else if (type == BP_VER) bp_ver = 1;
    else if (type == BP_WAR) bp_war = 1;
    else if (type == BP_INF) bp_inf = 1;
    else return 0;
}

int turn_off(const int type){
    if (type == BP_ERR) bp_err = 0;
    else if (type == BP_DBG) bp_dbg = 0;
    else if (type == BP_VER) bp_ver = 0;
    else if (type == BP_WAR) bp_war = 0;
    else if (type == BP_INF) bp_inf = 0;
    else return 0;
}




/*
 * 
 * name: print_e
 * @param const char *format The format for printf.
 * @param ... Any other arguments
 * @return
 * 
 */

int print_e(const char *format, ...){
    fflush(stdout);
    if (!bp_err) return 0;
    char str[BUFSIZ];
    memset(&str, 0, sizeof str);
    printf(str, ERROR, format);
    va_list vl;
    va_start(vl, format);
    vprintf(str, vl);
    va_end(vl);
    printf("\n");
}

int print_dbg(const char *format, ...){
    fflush(stdout);
    if (!bp_dbg) return 0;
    char str[BUFSIZ];
    memset(&str, 0, sizeof str);
    sprintf(str, DEBUG, format);
    va_list vl;
    va_start(vl, format);
    vprintf(str, vl);
    va_end(vl);
    printf("\n");
}

int print_v(const char *format, ...){
    fflush(stdout);
    if (!bp_ver) return 0;
    char str[BUFSIZ];
    memset(&str, 0, sizeof str);
    sprintf(str, VERBOSE, format);
    va_list vl;
    va_start(vl, format);
    vprintf(str, vl);
    va_end(vl);
    printf("\n");
}

int print_w(const char *format, ...){
    fflush(stdout);
    if (!bp_war) return 0;
    char str[BUFSIZ];
    memset(&str, 0, sizeof str);
    sprintf(str, WARN, format);
    va_list vl;
    va_start(vl, format);
    vprintf(str, vl);
    va_end(vl);
    printf("\n");
}

int print_i(const char *format, ...){
    fflush(stdout);
    if (!bp_inf) return 0;
    char str[BUFSIZ];
    memset(&str, 0, sizeof str);
    sprintf(str, ERROR, format);
    va_list vl;
    va_start(vl, format);
    vprintf(INFO, vl);
    va_end(vl);
    printf("\n");
}
