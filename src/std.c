/* Source file for the st client */
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>             /* uint8_t */

#include <limits.h>
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

/* WARNING > This function will NOT send data larger than 65535 bytes, and */
/*         > will NOT tell you what it did not send. It just wont send it. */
void
send(data, len)
     char *data;		/* byte data to send */
     int len;			/* length of the byte array */
{
  uint8_t s[2];                 /* unsigned char */

  if(len > 65535) {		/* cap the length */
    len = 65535;
  }

  s[0] = len >> 8;              /* these two lines are putting the length */
  s[1] = len;                   /* into an unsigned char array for writing */
                                /* I think this is to enforce endian-ness */
  write(fd, s, 2);              /* write the length array */
  write(fd, data, len);         /* write the actual data */
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
