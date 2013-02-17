/*
 * betterprint.c
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

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <sys/time.h>
#include <time.h>
#include "betterprint.h"

#define ERROR "[ERROR]\t"
#define DEBUG "[DEBUG]\t"
#define VERBOSE "[VERBOSE]\t"
#define WARN "[WARN]\t"
#define INFO "[INFO]\t"

#define BP_ERR 0x01
#define BP_DBG 0x02
#define BP_VER 0x03
#define BP_WAR 0x04
#define BP_INF 0x05
#define BP_LOG 0x06

#define OFF 0
#define ON 1

int bp_err = 0;
int bp_dbg = 0;
int bp_ver = 0;
int bp_war = 0;
int bp_inf = 0;
int bp_log = 0;

char *get_current_time_str(const char *);

int turn_on(const int types){
    int val = ON;
    if ((types & BP_ERR) == BP_ERR) bp_err = val;
    if ((types & BP_DBG) == BP_DBG) bp_dbg = val;
    if ((types & BP_VER) == BP_VER) bp_ver = val;
    if ((types & BP_WAR) == BP_WAR) bp_war = val;
    if ((types & BP_INF) == BP_INF) bp_inf = val;
    if ((types & BP_LOG) == BP_LOG) bp_log = val;
    return val;
}

int turn_off(const int types){
    int val = OFF;
    if ((types & BP_ERR) == BP_ERR) bp_err = val;
    if ((types & BP_DBG) == BP_DBG) bp_dbg = val;
    if ((types & BP_VER) == BP_VER) bp_ver = val;
    if ((types & BP_WAR) == BP_WAR) bp_war = val;
    if ((types & BP_INF) == BP_INF) bp_inf = val;
    if ((types & BP_LOG) == BP_LOG) bp_log = val;
    return val;
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
    if (!bp_err) return 0;
    fflush(stdout);
    printf(ERROR);
    va_list vl;
    va_start(vl, format);
    vprintf(format, vl);
    va_end(vl);
    printf("\n");
    return 1;
}

int print_dbg(const char *format, ...){
    if (!bp_dbg) return 0;
    fflush(stdout);
    printf(DEBUG);
    va_list vl;
    va_start(vl, format);
    vprintf(format, vl);
    va_end(vl);
    printf("\n");
    return 1;
}

int print_v(const char *format, ...){
    if (!bp_ver) return 0;
    fflush(stdout);
    printf(VERBOSE);
    va_list vl;
    va_start(vl, format);
    vprintf(format, vl);
    va_end(vl);
    printf("\n");
    return 1;
}

int print_w(const char *format, ...){
    if (!bp_war) return 0;
    fflush(stdout);
    printf(WARN);
    va_list vl;
    va_start(vl, format);
    vprintf(format, vl);
    va_end(vl);
    printf("\n");
    return 1;
}

int print_i(const char *format, ...){
    if (!bp_inf) return 0;
    fflush(stdout);
    printf(INFO);
    va_list vl;
    va_start(vl, format);
    vprintf(format, vl);
    va_end(vl);
    printf("\n");
    return 1;
}

int print_l(const char *format, ...){
    if (!bp_log) return 0;
    fflush(stdout);
    const char *f = "[%Y-%m-d %I:%M:%S %p]\t";
    printf("%s", get_current_time_str(f));
    va_list vl;
    va_start(vl, format);
    vprintf(format, vl);
    va_end(vl);
    printf("\n");
    return 1;
}

char *get_current_time_str(const char *format){
    char *time_str = (char *) malloc(100 * sizeof (char *));
    time_t the_time;
    (void) time(&the_time);
    struct tm *tmp_ptr = localtime(&the_time);
    strftime(time_str, 100, format, tmp_ptr);
    return time_str;
}
