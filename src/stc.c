/* Source file for the st client */
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>             /* uint8_t */

#include <unistd.h>
#include <limits.h>
#include <string.h>

#include <sys/utsname.h>
#include <sys/stat.h>
#include <sys/types.h>

#include <sys/socket.h>
#include <sys/un.h>
#include <fcntl.h>

#include <errno.h>

#include "st_shared.h"

void
connect_std(void)
{
  struct sockaddr_un socket_address;
  char *socket_name = unix_socket_name();
  struct st_connection connection;
  
  if(socket_name == NULL ||
     strlen(socket_name) >= sizeof(socket_address.sun_path)) {
    fprintf(stderr, "Socket name too long\n");
    exit(EXIT_FAILURE);
  }

  /* AF_UNIX - used to communicate between processes on the same machine */
  /* see man 7 unix */
  connection.fd = socket(AF_UNIX,     /* interprocess same machine */
                         SOCK_STREAM, /* stream oriented socket */
                         0);          /* standard protocol */

  if(connection.fd < 0) {             /* -1 returned for error */
    perror("Bad socket descriptor");
    exit(EXIT_FAILURE);
  }

  socket_address.sun_family = AF_UNIX;
  strcpy(socket_address.sun_path, socket_name);
  free(socket_name);

  int connect_success = connect(connection.fd,
                                (struct sockaddr *)&socket_address,
                                sizeof(socket_address));

  if(connect_success < 0) {            /* returns -1 on error */
    if(errno == ECONNREFUSED) {
      fprintf(stderr, "std not accepting connections.\n");
      return;
    }
    else {
      perror("unable to connect to std.");
      exit(EXIT_FAILURE);
    }
  }

  printf("connected to std. sleeping.\n");

  close(connection.fd);
}

int
main(argc, argv)
     int argc; 			/* argument count */
     char **argv;		/* argument vector */
{
  while(1) {
    connect_std();   
    sleep(1);
  }
  
  fprintf(stderr, "Could not connect to std. Starting st instead.");

  /* execvp looks for program on path */
  execvp("st", argv);
  return EXIT_SUCCESS;
}
