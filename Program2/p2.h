/* Nathan Azoulay
 * CS570, Program 2
 * Professor Carroll
 * October 2, 2020
 * p2.h: header file for p2.c 
 */

#include "getword.h"
#include <stdio.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>
#include <fcntl.h>
#include <dirent.h>
#define MAXITEM 100
#define MAXMEM 25500
void prompt();
int parse();
void myhandler();
