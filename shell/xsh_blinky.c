/**
 * @file   xsh_printprocstks.c
  * @provides printprocstks
   *
    * @author kmj5336
     */

     #include <stddef.h>
     #include <stdio.h>
     #include <string.h>
     #include <blinky.h>

     /**
      * shell command to print process stacks for three test processes.
       */

       shellcmd xsh_blinky(int nargs, char *args[])
       {
         run_blinky();
	 return OK;
	  }
