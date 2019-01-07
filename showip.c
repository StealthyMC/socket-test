/*  showip.c

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
// for inet_ntop(), converts network to presentation format (for IPs)
#include <arpa/inet.h>
// structs and constants for internet domain addresses
#include <netinet/in.h>

int main(int argc, char *argv[])
{
  // first check if portno supplied
  if (argc != 2)
  {
    fprintf(stderr, "usage: showip hostname\n");
    return 1;
  }

  int status;
  struct addrinfo hints, *res, *p;
  char ipstr[INET6_ADDRSTRLEN];

  memset(&hints, 0, sizeof hints);
  hints.ai_family = AF_UNSPEC;      // no specification for ipv4 or ipv6
  hints.ai_socktype = SOCK_STREAM;  // TCP all the way baby

  /*
  arg0  -   hostname on internet
  arg1  -   port to listen on host's IP (not needed in this example)
  arg2  -   'struct addrinfo' filled with relevant info (above)
  arg3  -   linked list pointed to by *res
  */
  if ((status = getaddrinfo(argv[1], NULL, &hints, &res)) != 0)
  {
    fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(status));
    return 2;
  }

  printf("\nIP addresses for %s:\n\n", argv[1]);

  for (p = res; p != NULL; p = p->ai_next)
  {
    void *addr;
    char *ipver;

    // obtain pointer to address (for inet_ntop() call)

    // for ipv4:
    if (p->ai_family == AF_INET)
    {
      struct sockaddr_in *ipv4 = (struct sockaddr_in *) p->ai_addr;
      addr = &(ipv4->sin_addr);
      ipver = "IPv4";
    }
    else
    // for ipv6:
    {
      struct sockaddr_in6 *ipv6 = (struct sockaddr_in6 *) p->ai_addr;
      addr = &(ipv6->sin6_addr);
      ipver = "IPv6";
    }

    // convert IP from network to presentation format
    inet_ntop(p->ai_family, addr, ipstr, sizeof ipstr);
    printf(" %s: %s\n\n", ipver, ipstr);
  }

  freeaddrinfo(res);

  return 0;
}
