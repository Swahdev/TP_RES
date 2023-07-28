/*
Exercice 1 : Utiliser select pour un serveur echo
Écrivez un serveur TCP qui écoute sur un port et agit en echo pour retourner les
messages reçus (une fois un client connecté sur ce port, si un message arrive du
client, il lui renvoie). Les messages auront taille maximale 512 octets et
termineront par \n. Ce serveur doit : – permettre la connexion et l’echo de
plusieurs client en parallèle – ne pas utiliser des threads mais utiliser select
Le serveur devra donc créer et utiliser deux fd_set :
– un fd_set a_surveiller
— il contiendra au départ le file descriptor correspondant au socket du serveur
(pour surveiller les nouvelles connexions) — on lui ajoutera à chaque nouvelle
connexion les file descriptor des sockets clients – un fd_set activite — au
démarrage de la boucle recevra une copie de a_surveiller — sera mis à jour par
select pour détecter les nouveaux événements sur les sockets clients et faire
l’echo On testera le serveur avec netcat sur le port choisi.
*/
// -------------------------------------------------------------------------- //

#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <unistd.h>
#include <errno.h>

#define MAX_CLIENTS 10
#define BUFFER_SIZE 512

int main(int argc, char *argv[])
{
    int server_socket, client_socket[MAX_CLIENTS], max_clients = MAX_CLIENTS,
                                                   activity, i, j, valread;
    int max_sd, sd;
    struct sockaddr_in server_addr, client_addr;
    char buffer[BUFFER_SIZE];

    // Create server socket
    if ((server_socket = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // Set server socket to allow multiple connections
    if (setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, (char *)&i,
                   sizeof(i)) < 0)
    {
        perror("setsockopt failed");
        exit(EXIT_FAILURE);
    }

    // Bind server socket to local address and port
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(8888);

    if (bind(server_socket, (struct sockaddr *)&server_addr,
             sizeof(server_addr)) < 0)
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    // Listen for incoming connections
    if (listen(server_socket, 3) < 0)
    {
        perror("listen failed");
        exit(EXIT_FAILURE);
    }

    // Initialize the list of client sockets to 0
    for (i = 0; i < max_clients; i++)
    {
        client_socket[i] = 0;
    }

    // Set of sockets to monitor for incoming connections and data
    fd_set read_fds;

    // Add the server socket to the set of sockets to monitor
    FD_ZERO(&read_fds);
    FD_SET(server_socket, &read_fds);
    max_sd = server_socket;

    printf("Server started. Waiting for incoming connections...\n");

    // Main server loop
    while (1)
    {
        // Copy the set of sockets to monitor to the set of active sockets
        fd_set active_fds = read_fds;

        // Wait for activity on any of the monitored sockets
        activity = select(max_sd + 1, &active_fds, NULL, NULL, NULL);

        if ((activity < 0) && (errno != EINTR))
        {
            perror("select failed");
            exit(EXIT_FAILURE);
        }

        // Check for new incoming connection
        if (FD_ISSET(server_socket, &active_fds))
        {
            if ((client_socket[i] =
                     accept(server_socket, (struct sockaddr *)&client_addr,
                            (socklen_t *)&j)) < 0)
            {
                perror("accept failed");
                exit(EXIT_FAILURE);
            }

            printf("New connection, socket fd is %d, ip is : %s, port : %d\n",
                   client_socket[i], inet_ntoa(client_addr.sin_addr),
                   ntohs(client_addr.sin_port));

            // Add new socket to the set of monitored sockets
            FD_SET(client_socket[i], &read_fds);

            // Update the maximum file descriptor
            if (client_socket[i] > max_sd)
            {
                max_sd = client_socket[i];
            }

            // Send welcome message to the new client
            char welcome_message[] = "Welcome to the server\n";
            if (send(client_socket[i], welcome_message, strlen(welcome_message),
                     0) != strlen(welcome_message))
            {
                perror("send failed");
            }
        }
        // Check for activity on the client sockets
        for (i = 0; i < max_clients; i++)
        {
            sd = client_socket[i];

            if (FD_ISSET(sd, &active_fds))
            {
                // Check if the client socket has closed the connection
                if ((valread = read(sd, buffer, BUFFER_SIZE)) == 0)
                {
                    // Client disconnected
                    getpeername(sd, (struct sockaddr *)&client_addr,
                                (socklen_t *)&j);
                    printf("Host disconnected, ip %s, port %d\n",
                           inet_ntoa(client_addr.sin_addr),
                           ntohs(client_addr.sin_port));

                    // Close the socket and remove it from the set of monitored
                    // sockets
                    close(sd);
                    client_socket[i] = 0;
                    FD_CLR(sd, &read_fds);
                }
                else
                {
                    // Echo the received message back to the client
                    buffer[valread] = '\0';
                    printf("Received message: %s", buffer);
                    send(sd, buffer, strlen(buffer), 0);
                }
            }
        }
    }

    return 0;
}