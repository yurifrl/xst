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

char *
unix_socket_name(void)
{
  char name[PATH_MAX];		     /* PATH_MAX is in limits.h */
  char *path = getenv("XST_SOCKET"); /* if launched with a configured socket */

  if(path == NULL) { 		/* Only if the environment has no socket defined */
    struct utsname u;
    uname(&u);

    path = getenv("HOME");
    if(path == NULL) {
      path = "/tmp";
    }

    snprintf(			/* print formatted into a string maximally n len */
	     name,		/* string to print into */
	     PATH_MAX,		/* maximal len */
	     "%s/.xst",		/* format to print */
	     path);		/* replace first format variable */
    
    mkdir(			/* make a new directory */
	  name,			/* name of the new directory */
	  0777);		/* chmod argument */

    snprintf(name,
	     PATH_MAX,
	     "%s/.xst/xstd-%s",
	     path,
	     u.nodename);

    path = name;
  }

  return strdup(		/* return a duplicated string (malloc) */
		path);		/* string to duplicate */
}
