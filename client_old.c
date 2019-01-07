/* --- client_old.c ---
  Created using guide at:
  http://www.linuxhowtos.org/C_C++/socket.htm
  Simple program that creates a socket and allows two hosts to talk to each
  other over the internet.
  Considered "old-styled" since some function calls like gethostbyname()
  are no longer used in favor of calls like getaddrinfo().
*/

// declarations for input and output
#include <stdio.h>
#include <stdlib.h>
// needed for string functions like atoi()
#include <strings.h>
// needed for read() and write() calls
#include <unistd.h>
// contains definitions for data types used in system calls
#include <sys/types.h>
// includes structs needed for sockets
#include <sys/socket.h>
// contains structs and constants for internet domain addresses
#include <netinet/in.h>

// defines the struct 'hostent'
#include <netdb.h>

// for inet_ntop() call
#include <arpa/inet.h>

/*
  Error message used when a system call fails. The argument passed into the
  failure function will be displayed before the program is aborted.
*/
void error(const char *msg)
{
  perror(msg);
  exit(1);
}

int main(int argc, char *argv[])
{
  /*
  sockfd    -   Our file descriptor
  portno    -   Port number to connect to.
  n         -   Return value for read() and write() calls
  */
  int sockfd, portno, n;
  // struct that will contain the address of the server to connect to
  struct sockaddr_in serv_addr;
  // struct 'hostent' that defines a host computer on the internet
  struct hostent *server;

  /* below code is same as server */

  // contains the characters from the socket connection
  char buffer[256];

  // check for sufficient number of arguments
  if (argc < 3)
  {
    fprintf(stderr, "ERROR, port number required\n");
    exit(1);
  }

  /*
  socket() call creates a new socket. If the socket call fails, it returns
  a value of -1.
  - There are two possible adddress domains: AF_UNIX and AF_INET.
  AF_UNIX is for any two process that share a common file system, while AF_INET
  is for any two hosts on the Internet.
  - Since data is being read on a stream, we use SOCK_STREAM. SOCK_DGRAM is for
  cases where data is read in chunks.
  - Last argument is for protocol. TCP is for stream sockets, and UDP is for
  datagram sockets. Usually set to 0.
   */
  portno = atoi(argv[2]);
  sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sockfd < 0)
    error("ERROR opening socket");

  /*    */

  /* 'argv[1]' contains the name of a host on the internet, and the call
  gethostbyname() expects a char* (i.e. a string) of the hostname. The call
  queries large databases all around the country. */
  server = gethostbyname(argv[1]);
  if (server == NULL)
  {
    fprintf(stderr, "ERROR, no such host");
    exit(0);
  }

  // zero out buffer of the server_addr structure
  bzero((char *) &serv_addr, sizeof(serv_addr));
  // set family to AF_INET
  serv_addr.sin_family = AF_INET;
  /* Copy from server->h_addr to serv_addr.sin_addr.s_addr.
  Basically copies the address from the server's h_addr into client's
  server_addr.sin_addr.s_addr. The variables are both being casted into char*.
  */
  bcopy((char *) server->h_addr, (char *) &serv_addr.sin_addr.s_addr,
  server->h_length);
  // Little and big endian stuff
  serv_addr.sin_port = htons(portno);

  /* connect() is called by client to establish a connection to the server.
  The three arguments are the socket file descriptor, the address of the host
  that it wants to connect to (which includes the port number), and the size
  of the address.
  Returns 0 on success, -1 if it fails. */
  if (connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
    error("ERROR connecting");

  char ip4[INET_ADDRSTRLEN];
  inet_ntop(AF_INET, &(serv_addr.sin_addr), ip4, INET_ADDRSTRLEN);
  printf("Succesfully connected to host.\nServer IP: %s\n", ip4);

  printf("Please enter the message: ");
  bzero(buffer, 256);
  // char *fgets(char *str, int n, FILE *stream)
  // 'n' is the maximum number of characters to read, including null character
  fgets(buffer, 255, stdin);

  // once input is received, write it to the file descriptor
  // in short, it sends the contents of the buffer to the server.
  n = write(sockfd, buffer, strlen(buffer));
  if (n < 0)
    error("ERROR writing to socket");

  /* read response from the server */
  // first zero out the buffer
  bzero(buffer, 256);
  // read contents from file descriptor, which will be copied into buffer
  n = read(sockfd, buffer, 255);
  if (n < 0)
    error("ERROR reading from socket");

  // finally, print response from the server, which was copied into buffer
  printf("Response from server:\n%s\n", buffer);

  return 0;
}
