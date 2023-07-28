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
  if((sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
    perror("erreur socket");
    return 1;
  }

  /* Initialisation de l'adresse d'abonnement */
  struct sockaddr_in grsock;
  memset(&grsock, 0, sizeof(grsock));
  grsock.sin_family = AF_INET;
  inet_pton(AF_INET, "225.1.2.3", &grsock.sin_addr);
  grsock.sin_port = htons(4321);

  struct ip_mreqn group;
  memset(&group, 0, sizeof(group));
  group.imr_multiaddr.s_addr = htonl(INADDR_ANY);
  group.imr_ifindex = if_nametoindex("eth0");
  
  if(setsockopt(sock, IPPROTO_IP, IP_MULTICAST_IF, &group, sizeof(group))){
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

