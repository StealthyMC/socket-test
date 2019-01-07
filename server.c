/*  server.c

    Based on guide from:
    http://beej.us/guide/bgnet/html/multi/index.html

    More "IP-agnostic" version of the previous C scripts.

*/

#include <stdio.h>
#include <stdlib.h>

// definitions for data types used in syscalls
#include <sys/types.h>
// definitions for socket-related structs
#include <sys/socket.h>
#include <netdb.h>
// definitions for string functions like strcpy(), memset()
#include <string.h>

int main(int argc, char *argv[])
{
  // first check if portno supplied
  if (argc < 2)
  {
    fprintf(stderr, "usage: server port_number\n");
    exit(0);
  }

  int status;
  struct addrinfo hints;
  struct addrinfo *servinfo;

  memset(&hints, 0, sizeof hints);
  hints.ai_family = AF_UNSPEC;      // no specification for ipv4 or ipv6
  hints.ai_socktype = SOCK_STREAM;  // TCP all the way baby
  hints.ai_flags = AI_PASSIVE;      // fill IP in automatically

  if ((status = getaddrinfo(NULL, argv[1], &hints, &servinfo)) != 0)
  {
    fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(status));
    exit(1);
  }

  freeaddrinfo(servinfo); // free linked-list

  return 0;
}
