/* Source file for the st client */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int
main(argc, argv)
     int argc; 			/* argument count */
     char **argv;		/* argument vector */
{
  perror("Could not connect to std. Starting st instead.");

  /* execvp looks for program on path */
  execvp("st", argv);
  return EXIT_SUCCESS;
}
