
#include <arpa/inet.h>
#include <net/if.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#define BUF_SIZE 123

int main(int argc, char *argv[])
{
    int sock = socket(
        PF_INET6, SOCK_DGRAM,
        0); // Crée un socket IPv6 pour envoyer et recevoir des datagrammes
    if (sock < 0)
        return -1; // Si la création du socket a échoué, retourne -1

    struct sockaddr_in6
        servadr; // Crée une structure pour stocker l'adresse du serveur
    memset(&servadr, 0, sizeof(servadr)); // Initialise la structure à 0
    servadr.sin6_family = AF_INET6;       // Spécifie que l'adresse est IPv6
    servadr.sin6_addr =
        in6addr_any; // Spécifie que le serveur peut recevoir des datagrammes de
                     // n'importe quelle adresse IPv6
    servadr.sin6_port = htons(atoi(
        argv[1])); // Spécifie le numéro de port que le serveur va utiliser pour
                   // recevoir des datagrammes. Le numéro de port est obtenu à
                   // partir des arguments de la ligne de commande.

    if (bind(sock, (struct sockaddr *)&servadr, sizeof(servadr)) < 0)
        return -1; // Associe le socket à l'adresse du serveur. Si cela échoue,
                   // retourne -1.

    char buffer[BUF_SIZE + 1]; // Crée un tampon pour stocker les données reçues
    struct sockaddr_in6 cliadr; // Crée une structure pour stocker l'adresse du
                                // client qui a envoyé le datagramme
    socklen_t len =
        sizeof(cliadr); // Initialise la taille de la structure qui stocke
                        // l'adresse du client à sizeof(cliadr)
    memset(buffer, 0, BUF_SIZE + 1); // Initialise le tampon à 0

    int r =
        recvfrom(sock, buffer, BUF_SIZE, 0, (struct sockaddr *)&cliadr,
                 &len); // Reçoit un datagramme sur le socket, stocke les
                        // données dans le tampon et l'adresse du client qui a
                        // envoyé le datagramme dans la structure cliadr. Stocke
                        // également la taille de la structure dans len. La
                        // fonction recvfrom renvoie le nombre d'octets reçus.

    if (r < 0)
        return -1; // Si la réception du datagramme a échoué, retourne -1

    printf("message recu - %d octets : %s\n", r,
           buffer); // Affiche le message reçu et le nombre d'octets reçus
    close(sock);    // Ferme le socket
    return 0;       // Retourne 0 pour indiquer que tout s'est bien passé
}
