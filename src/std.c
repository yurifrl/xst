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

/* WARNING > This function will NOT send data larger than 65535 bytes, and */
/*         > will NOT tell you what it did not send. It just wont send it. */
/* data - byte data to send */
/* len - length of the byte array */
void
send_to_std(struct st_connection *connection, char *data, int len)
{
  uint8_t s[2];                 /* unsigned char */

  if(len > 65535) {		/* cap the length */
    len = 65535;
  }

  s[0] = len >> 8;              /* these two lines are putting the length */
  s[1] = len;                   /* into an unsigned char array for writing */
                                /* I think this is to enforce endian-ness */
  write(connection->fd, s, 2);  /* write the length array */
  write(connection->fd, data, len); /* write the actual data */
}

void
start_listener(char *socket_name, struct st_connection *connection)
{
  int fcntl_return = 0;
  struct sockaddr_un socket_address;   /* sockaddr_un - unix socket address struct */

  if(socket_name == NULL ||
     strlen(socket_name) >= sizeof(socket_address.sun_path)) {
    fprintf(stderr, "Socket name too long\n");
    exit(EXIT_FAILURE);
  }

  /* AF_UNIX - used to communicate between processes on the same machine */
  /* see man 7 unix */
  connection->fd = socket(AF_UNIX,     /* interprocess same machine */
                          SOCK_STREAM, /* stream oriented socket */
                          0);          /* standard protocol */

  if(connection->fd < 0) {             /* -1 returned for error */
    perror("Bad socket descriptor");
    exit(EXIT_FAILURE);
  }

  fcntl_return = fcntl(         /* manipulate open file descriptor */
                       connection->fd, /* file descriptor to manipulate */
                       F_SETFD,        /* apply file status flag */
                       FD_CLOEXEC);    /* close on exec (close when execve called) */

  if(fcntl_return < -1) {
    perror("Unable to set close on exec flag for socket.");
    exit(EXIT_FAILURE);
  }

  fcntl_return = fcntl(connection->fd,
                       F_SETFL,     /* set a flag */
                       O_NONBLOCK); /* fail reads with EAGAIN error if no data. NON BLOCKING */

  if(fcntl_return < -1) {
    perror("Unable to set close on non blocking read flag for socket.");
    exit(EXIT_FAILURE);
  }

  socket_address.sun_family = AF_UNIX; /* set the socket unix family to match socket */
  strcpy(socket_address.sun_path,      /* copy the socket name into the address path */
         socket_name);

  unlink(socket_name);          /* deletes the socket file ... TODO > necessary? */

  mode_t original_mask = umask(0077);   /* TODO > necessary? */

  /* assigns a name to the socket so we can accept connections */
  int bind_success = bind(      /* assigns socket_address to the created fd */
                          connection->fd,          /* file descriptor to bind to */
                          (struct sockaddr *)&socket_address, /* socket_address to bind */
                          sizeof(socket_address));     /* length of the UNIX_AF 
                                                          socket address type */

  if(bind_success < 0) {            /* returns -1 on error */
    perror("unable to assign a name to our socket.");
    exit(EXIT_FAILURE);
  }

  umask(original_mask);         /* reset the mask to before our bind */

  /* non blocking. allows accepts. */
  int listen_result = listen(   /* enable listening for connections to our socket */
                             connection->fd, /* file descriptor of listening socket */
                             5);             /* number of pending connections allowed */

  if(listen_result < 0) {       /* returns -1 on error */
    perror("listen failed on our socket");
    exit(EXIT_FAILURE);
  }

  printf("accepting new connections on socket '%s'\n", socket_name);

  socklen_t peer_addr_size;
  while(1) {
    peer_addr_size = sizeof(struct sockaddr_un);
    int accept_result = accept(connection->fd,
                               (struct sockaddr *)&socket_address,
                               &peer_addr_size);
    if(accept_result < 0) {     /* error on -1 */
      if(errno == EAGAIN) {
        printf("no connection on non blocking socket. sleeping.\n");
        sleep(6);
      }
      else {
        perror("unhandled error.");
        exit(EXIT_FAILURE);
      }
    }
    else {
      printf("connection received...\n");
    }
  }
}

int
main(int argc, char **argv)
{
  char *socket_name = unix_socket_name();
  struct st_connection connection;
  start_listener(socket_name, &connection);
  
  free(socket_name);
  
  return EXIT_SUCCESS;
}
