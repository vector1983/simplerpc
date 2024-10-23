#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#define SERVERPORT "5005"
#define SERVER "localhost"

#include "is_prime_rpc_client.h"

// Packs  an int, we used to convert it from host order to network orders
int pack(int input) {
  return htons(input);
}

int unpack(int packed_input) {
  return ntohs(packed_input);
}

// Gets the IPv4 or IPv6 sockaddr.
void *get_in_addr(struct sockaddr *sa) {
  if (sa->sa_family == AF_INET) {
    return &(((struct sockaddr_in*)sa)->sin_addr);
  } else {
    return &(((struct sockaddr_in6*)sa)->sin6_addr);
  }
}

// Gets a socket to connect with
int get_socket() {
  int sockfd;
  struct addrinfo hints, *server_info, *p;
  int number_of_bytes;

  memset(&hints, 0, sizeof hints);
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM; // tcp
  int return_value = getaddrinfo(SERVER, SERVERPORT, &hints, &server_info);
  if (return_value != 0) {
    fprintf(stderr, "getaddinfo: %s\n", gai_strerror(return_value));
    exit(1);
  }

  // end up with a linked-list of addresses, and we want to connect to the
  // first one we can
  for (p = server_info; p != NULL; p = p->ai_next) {
    // Try to make a socket with this one
    if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
      // something went wrong getting this socket, so we can try the next one.
      perror("client: socket");
      continue;
    }
    // Try to connect to that socket
    if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
      close(sockfd);
      perror("client: connect");
      continue;
    }

    break;
  }

  // if we haven't gotten a valid sockaddr here, that means we can't connect.
  if (p == NULL) {
    fprintf(stderr, "client: failed to connect\n");
    exit(2);
  }

  return sockfd;
}


bool is_prime_rpc(int number) {

  // first , we pack the arguments
  int packed_number = pack(number);

  // now get socket we can connect
  int sockfd = get_socket();

  // send just the packed number.
  if (send(sockfd, &packed_number, sizeof packed_number, 0) == -1) {
    perror("send");
    close(sockfd);
    exit(0);
  }

  // Now, wait to receive the answer.
  int buf;
  int bytes_received = recv(sockfd, &buf, sizeof buf, 0);
  if(bytes_received == -1) {
    perror("recv");
    exit(1);
  }

  // since we just have the one byte, don't really need to do anything while
  // unpacking it.
  int unpacked_result;
  unpacked_result = unpack(buf);
  printf("received response: %d\n", unpacked_result);
  bool result = unpacked_result != 0;

  // done, close socket 
  close(sockfd);
  return result;

}
