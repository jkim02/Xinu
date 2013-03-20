/**
 * @file   xsh_printprocstks.c
 * @provides printprocstks
 *
 * @author kmj5336
 */

#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <printprocstks.h>

/**
 * shell command to print process stacks for three test processes.
 */

shellcmd xsh_printprocstks(int nargs, char *args[])
{
 printprocstks();
 return OK;
 }
