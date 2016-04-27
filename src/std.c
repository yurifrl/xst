/* Source file for the st client */
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/utsname.h>
#include <sys/stat.h>
#include <sys/types.h>

char *
unix_sockname()
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

void
send(data, len)
     char *data;		/* byte data to send */
     int len;			/* length of the byte array */
{
  uint8_t s[s];

  if(len > 65535) {		/* cap the length */
    len = 65535;
  }

  s[0] = len >> 8;
  s[1] = len;
}

int
main(argc, argv)
     int argc; 			/* argument count */
     char **argv;		/* argument vector */
{
  char *mer = unix_sockname();

  printf("%s\n", mer);

  free(mer);
  
  return EXIT_SUCCESS;
}
