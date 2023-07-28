#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <net/if.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>

void affiche_adresse(struct sockaddr_in6 adr){
    char adr_buf[INET6_ADDRSTRLEN];
    memset(adr_buf, 0, sizeof(adr_buf));
    
    inet_ntop(AF_INET6, &(adr.sin6_addr), adr_buf, sizeof(adr_buf));
    printf("adresse serveur : IP: %s port: %d\n", adr_buf, ntohs(adr.sin6_port));
}


int main(int argc, char const *argv[]) {
  int sock;

  /* créer la socket */
  if((sock = socket(AF_INET6, SOCK_DGRAM, 0)) < 0) {
    perror("echec de socket");
    return 1;
  }

  /* SO_REUSEADDR permet d'avoir plusieurs instances locales de cette application  */
  /* ecoutant sur le port multicast et recevant chacune les differents paquets       */
  int ok = 1;
  if(setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &ok, sizeof(ok)) < 0) {
    perror("echec de SO_REUSEADDR");
    close(sock);
    return 1;
  }

   /* Initialisation de l'adresse de reception */
  struct sockaddr_in6 grsock;
  memset(&grsock, 0, sizeof(grsock));
  grsock.sin6_family = AF_INET6;
  grsock.sin6_addr = in6addr_any;
  grsock.sin6_port = htons(4321);
  
  if(bind(sock, (struct sockaddr*) &grsock, sizeof(grsock))) {
    perror("echec de bind");
    close(sock);
    return 1;
  }
  
  /* initialisation de l'interface locale autorisant le multicast IPv6 */
  int ifindex = if_nametoindex ("eth0");

  /* s'abonner au groupe multicast */
  struct ipv6_mreq group;
  inet_pton (AF_INET6, "ff12::1:2:3", &group.ipv6mr_multiaddr.s6_addr);
  group.ipv6mr_interface = ifindex;

  if(setsockopt(sock, IPPROTO_IPV6, IPV6_JOIN_GROUP, &group, sizeof group) < 0) {
    perror("echec de abonnement groupe");
    close(sock);
    return 1;
  }

  //on peut lire avec read
  char buf[10];
  memset(buf, 0, sizeof(buf));
  if (read(sock, buf, sizeof(buf)-1) < 0) {
      perror("echec de read");
      return -1;
  }
  printf("Message : %s\n", buf);

  //ou on peut lire avec recvfrom
  struct sockaddr_in6 cliadr;
  int recu;
  socklen_t clilen = sizeof(cliadr);

  memset(buf, 0, sizeof(buf));
  if ((recu = recvfrom(sock, buf, sizeof(buf)-1, 0, (struct sockaddr *)&cliadr, &clilen)) < 0){
    perror("echec de recvfrom.");
    return -1;
  }
  affiche_adresse(cliadr);
  printf("Message : %s\n", buf);

  close(sock);
  return 0;
}

