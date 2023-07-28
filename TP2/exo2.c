#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

int lampe_port = 13;
const char *lampe_ip = "192.168.70.237";

int main(int argc, char const *argv[])
{
    // Creation de la socket
    int sock = socket(PF_INET, SOCK_STREAM, 0);
    // Preparation de l'adresse ip + port du destinataire (le serveur)
    struct sockaddr_in adrso;
    memset(&adrso, 0, sizeof(adrso));
    adrso.sin_family = AF_INET;
    adrso.sin_port = htons(lampe_port);
    inet_pton(AF_INET, lampe_ip, &adrso.sin_addr);
    // Connexion au serveur
    if (connect(sock, (struct sockaddr *)&adrso, sizeof(adrso)) == -1)
    {
        perror("connect");
        exit(1);
    }
    // affiche la date et l’heure renvoyées par le service
    char buf[1024];
    int n = read(sock, buf, sizeof(buf));
    if (n == -1)
    {
        perror("read");
        exit(1);
    }
    buf[n] = '\0';
    printf("%s", buf);
    char buf2[1025];
    memset(buf2, 0, 1025);
    int recu = recv(sock, buf2, 1025, 0);
    close(sock);
    return 0;
}
