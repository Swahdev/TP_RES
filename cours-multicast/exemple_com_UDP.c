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
{ // Début de la fonction main avec des arguments d'entrée
    int sock = socket(PF_INET6, SOCK_DGRAM,
                      0); // Création d'un socket IPv6 de type datagramme (UDP)
    if (sock < 0)
        return -1; // En cas d'erreur lors de la création du socket, on quitte
                   // la fonction main avec une valeur de retour -1
    // adresse de destination
    struct sockaddr_in6 adr; // Déclaration d'une structure sockaddr_in6 qui
                             // contient l'adresse IPv6 de destination
    memset(&adr, 0, sizeof(adr)); // Initialisation de la structure à 0
    adr.sin6_family =
        AF_INET6; // On spécifie que la famille d'adresses utilisée est IPv6
    inet_pton(
        AF_INET6, argv[1],
        &adr.sin6_addr); // On convertit l'adresse IPv6 donnée en argument en un
                         // format binaire utilisable par le programme
    adr.sin6_port =
        htons(atoi(argv[2])); // On spécifie le port de destination en le
                              // convertissant depuis une chaîne de caractères
                              // en un entier court en réseau
    socklen_t len =
        sizeof(adr);       // On calcule la taille de la structure sockaddr_in6
    char buffer[BUF_SIZE]; // On déclare un tampon de lecture/écriture pour
                           // stocker les données à envoyer
    memset(buffer, 0, BUF_SIZE); // On initialise le tampon à 0
    sprintf(buffer,
            "et voila un message\n"); // On écrit un message dans le tampon
    int env = sendto(sock, buffer, strlen(buffer), 0, (struct sockaddr *)&adr,
                     len); // On envoie les données contenues dans le tampon
                           // vers l'adresse de destination spécifiée
    if (env < 0)
    { // Si l'envoi a échoué, on affiche un message d'erreur
        perror("echec de sendto.");
        return -1;
    }
} // Fin de la fonction main
