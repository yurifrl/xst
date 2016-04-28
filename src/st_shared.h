#ifndef _st_shared_H_
#define _st_shared_H_

#ifndef PATH_MAX
#define PATH_MAX 16384
#endif

struct st_connection {
  int fd;                       /* socket file descriptor */
};

char * unix_socket_name(void);

#endif
