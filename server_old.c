/* --- server_old.c ---
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
  sockfd      -
  newsockfd   -   File descriptors that store the values returned by the
                  socket system call and the accept system call.
                  [ http://www.linuxhowtos.org/data/6/fd.txt ] contains info on
                  the file descriptor table.
  portno      -   Port number where server accepts connections.
  n           -   Return value for read() and write() calls. Basically contains
                  the number of characters read or written.
  */
  int sockfd, newsockfd, portno, n;
  /* clilen stores the size of the address of the client. Needed for the
  accept system call. Data type socklen_t is basically another form of int, but
  since the size of int can vary between machines, socklen_t is used to hold
  the address size for consistency. */
  socklen_t clilen;

  // contains the characters from the socket connection
  char buffer[256];
  // structures for server and client address, defined in <netinet/in.h>
  struct sockaddr_in serv_addr, cli_addr;

  // check for sufficient number of arguments
  if (argc < 2)
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
  sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sockfd < 0)
    error("ERROR opening socket");

  // sets all values in a buffer to 0
  // first argument is pointer to buffer, and second is the size of buffer
  bzero((char *) &serv_addr, sizeof(serv_addr));

  // convert command-line argument for port # to a usable integer data type
  portno = atoi(argv[1]);

  /* configure the serv_addr struct */

  /* set address family to symbolic constant AF_INET (for two hosts across the
  internet) */
  serv_addr.sin_family = AF_INET;
  /* Set port # to value obtained from command line argument.
   However, the integer must be converted to network byte order using htons().
   This is because computer networks are BIG ENDIAN but the host computers are
   LITTLE ENDIAN. */
  serv_addr.sin_port = htons(portno);
  /* Sets the IP address of the host, where s_addr is a value within sin_adder,
  which is a struct within serv_addr. INADDR_ANY is a constant that is equal to
  the IP address of the machine on which the server is running. */
  serv_addr.sin_addr.s_addr = INADDR_ANY;
  /* bind() system call binds a socket to an address, which is the address of
  the current host, and the port number the server is being run on.
  Three arguments: socket file descriptor, address to which it is bound, and
  size of the address which it is bound.
  Second argument is a pointer to a structure of type sockaddr, and since what's
  being passed in is of type sockaddr_in, it needs to be casted.
  Common reason of failure is that the socket chosen is already being used. */
  if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
    error("ERROR on binding");

  /* The system call listen() allows the process to listen on the socket for
  connections, indicated by the first argument, the socket file descriptor. The
  second argument is the backlog queue, which is the number of connections that
  can be waiting while the process is processing a connection. If the first
  argument is a valid socket, the call won't fail, and therefore, there are no
  error checks. */
  listen(sockfd, 5);
  printf("Socket binded. Listening for an inbound connection...\n");

  // Get the size of the client address, so that it can be passed to accept().
  clilen = sizeof(cli_addr);
  /* The accept() system call causes the process to stop until a client
  establishes a connection to the server. When there's a connection established,
  the server wakes up. The call returns a new file descriptor, which will be
  assigned to 'newsockfd', and all communication will be handled through this
  new file descriptor.
  Second argument is a reference pointer to the address of the client, and the
  third argument is the size of the client address. */
  newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
  char ip4[INET_ADDRSTRLEN];
  inet_ntop(AF_INET, &(cli_addr.sin_addr), ip4, INET_ADDRSTRLEN);
  printf("Connection has been accepted.\nClient IP: %s\n", ip4);
  if (newsockfd < 0)
    error("ERROR on accept");

  /* This part of the code is reached after a connection is accepted. The buffer
  is zeroed out by the bzero() call, and then reads from the socket. Notice that
  from this point, the new file descriptor is being used. read() will read all
  characters or up until 255, which is less, and return the number of characters
  read, which will be set to 'n'. */
  bzero(buffer, 256);
  n = read(newsockfd, buffer, 255);
  printf("Here is the message: %s", buffer);

  // Send something to the client.
  /* First argument is file descriptor, second is the message, and third is the
  size of the message. */
  n = write(newsockfd, "I got your message!", 19);
  if (n < 0)
    error("ERROR writing to socket");

  return 0;
}
