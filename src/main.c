/* $Id: main.c,v 1.1 2015/03/19 22:20:21 sentient Exp sentient $ 
 *
 * Author: Dan Wilder
 *
 * REVISION HISTORY
 *   $Log: main.c,v $
 *   Revision 1.1  2015/03/19 22:20:21  sentient
 *   Initial revision
 *
 *
 */

#include <stdlib.h> 
#include <stdio.h>   
#include <ctype.h>    
#include <unistd.h>   

void DisplayUsage(char *);
void serverMode();
void clientMode();

/*****************************************************************************
 *    main()                                                                 *
 *****************************************************************************/

int main(int argc, char **argv) {

  int c; 

  // Presence of option character will set corresponding flag
  char sflag = 0;
  char cflag = 0;

  opterr = 0; // prevents the default error messages of getopt() 
  
  while ((c = getopt (argc, argv, "sc")) != -1) { // Set option flags 
    switch (c) {

    case 's':
      sflag = 1;
      break;

    case 'c':
      cflag = 1;
      break;

    case '?': 
      if (isprint (optopt))
        fprintf (stderr, "Unknown option `-%c'.\n", optopt);
      else
        fprintf (stderr, "Unknown option character `\\x%x'.\n", optopt);

      return 1; 

    default:
      abort (); 
    }
  }

  if (argc == 1)
    DisplayUsage(argv[0]);

  else {
    if (sflag) {
      serverMode();
    }

    else if (cflag) {
      clientMode();
    }
  }

  return 0;
}

/*****************************************************************************
 *    DisplayUsage()                                                         *
 *****************************************************************************/

void DisplayUsage(char *programName) {
  printf("\nSYNOPSIS\n    %s [OPTION]\n", programName);   
   
  printf("\nDESCRIPTION\n\tCommand Line Instant Messenger for communication");
  printf(" between users\n\ton the same system. Should work on POSIX-compliant");
  printf(" systems.\n\tAlthough this has yet to be extensively tested...");
  printf("\n\t\t***CURRENTLY UNDER DEVELOPMENT***\n");

  printf("\nOPTIONS\n    \t-s\n\t    serverMode: Starts Server\n");   
  printf("\n\t-c\n\t    clientMode: Connect to Server\n\n");
}
