#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <net/if.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>

int main(int argc, char const *argv[]) {
  int sock;

  /* créer la socket */
  if((sock = socket(AF_INET6, SOCK_DGRAM, 0)) < 0) {
    perror("erreur socket");
    return 1;
  }

  /* Initialisation de l'adresse d'abonnement */
  struct sockaddr_in6 grsock;
  memset(&grsock, 0, sizeof(grsock));
  grsock.sin6_family = AF_INET6;
  inet_pton(AF_INET6, "ff12::1:2:3", &grsock.sin6_addr);
  grsock.sin6_port = htons(4321);

  /* initialisation de l'interface locale autorisant le multicast IPv6 */
  int ifindex = if_nametoindex("eth0");
  
  if(setsockopt(sock, IPPROTO_IPV6, IPV6_MULTICAST_IF, &ifindex, sizeof(ifindex))) {
    perror("erreur initialisation de l'interface locale");
    return 1;
  }

  char buf[10];
  sprintf(buf, "bonjour");
  
  if (sendto(sock, buf, strlen(buf), 0, (struct sockaddr*)&grsock, sizeof(grsock)) < 0) 
    printf("erreurr send\n");

  close(sock);
  return 0;
}

