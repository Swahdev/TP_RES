#include <arpa/inet.h>
#include <poll.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

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

    // Set of pollfd structures to monitor for incoming connections and data
    struct pollfd pollfds[MAX_CLIENTS + 1];

    // Add the server socket to the set of pollfds to monitor
    pollfds[0].fd = server_socket;
    pollfds[0].events = POLLIN;
    max_sd = server_socket;

    printf("Server started. Waiting for incoming connections...\n");

    // Main server loop
    while (1)
    {
        // Wait for activity on any of the monitored pollfds
        activity = poll(pollfds, max_clients + 1, -1);

        if (activity < 0)
        {
            perror("poll failed");
            exit(EXIT_FAILURE);
        }

        // Check for new incoming connection
        if (pollfds[0].revents & POLLIN)
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

            // Add new socket to the set of monitored pollfds
            pollfds[i + 1].fd = client_socket[i];
            pollfds[i + 1].events = POLLIN;

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
                exit(EXIT_FAILURE);
            }
            printf("Welcome message sent to client %d\n", i);

            // Increment the index for the next client socket
            i++;
        }

        // Check for incoming data from any of the client sockets
        for (int j = 1; j <= max_clients; j++)
        {
            if (pollfds[j].revents & POLLIN)
            {
                // Receive incoming data
                valread = read(pollfds[j].fd, buffer, BUFFER_SIZE);

                if (valread == 0)
                {
                    // Disconnection detected, remove socket from pollfds
                    close(pollfds[j].fd);
                    pollfds[j].fd = -1;
                    printf("Client %d disconnected\n", j);
                }
                else
                {
                    // Echo received data back to the client
                    printf("Received data from client %d: %s\n", j, buffer);
                    if (send(pollfds[j].fd, buffer, strlen(buffer), 0) !=
                        strlen(buffer))
                    {
                        perror("send failed");
                        exit(EXIT_FAILURE);
                    }
                }
            }
        }
    }

    return 0;
}
