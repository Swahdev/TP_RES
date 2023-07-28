#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <string.h>

#define PORT 12345
#define MAX_LEN 256

int exo1_part1(int port, int source)
{
    int server_socket, client_socket;
    struct sockaddr_in server_address, client_address;
    socklen_t client_len = sizeof(client_address);
    char buffer[MAX_LEN];
    // Creation de la socket_serv
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket > 0)
    {
        memset(&server_address, 0, sizeof(server_address));

        server_address.sin_family = AF_INET;
        server_address.sin_port = htons(port);
        server_address.sin_addr.s_addr = htonl(source);
        // Bind
        if (bind(server_socket, (struct sockaddr *)&server_address, sizeof(server_address)) == 0)
        {
            // Listen
            if (listen(server_socket, 5) == 0)
            {
                // Accept
                client_socket = accept(server_socket, (struct sockaddr *)&client_address, &client_len);
                if (client_socket > 0)
                {
                    int n;
                    // Reception
                    while ((n = recv(client_socket, buffer, MAX_LEN - 1, 0)) > 0)
                    {
                        buffer[n] = '\0';
                        // Envoie
                        send(client_socket, buffer, strlen(buffer), 0);
                    }
                    close(client_socket);
                    close(server_socket);
                }
            }
        }
    }
    return 0;
}

int exo1_part2(int port, int source)
{
    int server_socket, client_socket;
    struct sockaddr_in server_address, client_address;
    socklen_t client_len = sizeof(client_address);
    char buffer[MAX_LEN];
    // Creation de la socket_serv
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket > 0)
    {
        memset(&server_address, 0, sizeof(server_address));

        server_address.sin_family = AF_INET;
        server_address.sin_port = htons(port);
        server_address.sin_addr.s_addr = htonl(source);
        // Bind
        if (bind(server_socket, (struct sockaddr *)&server_address, sizeof(server_address)) == 0)
        {
            // Listen
            if (listen(server_socket, 5) == 0)
            {
                // Accept
                while(client_socket = accept(server_socket, (struct sockaddr *)&client_address, &client_len) > 0)
                {
                    int n;
                    // Reception
                    while ((n = recv(client_socket, buffer, MAX_LEN - 1, 0)) > 0)
                    {
                        buffer[n] = '\0';
                        // Envoie
                        send(client_socket, buffer, strlen(buffer), 0);
                    }
                    close(client_socket);
                }
                    close(server_socket);
                }
            }
        }
            return 0;
    }

int main(int argc, char const *argv[])
{
    // 1ere partie
    exo1_part1(PORT, INADDR_ANY);

    // 2eme partie
    exo1_part2(PORT, INADDR_ANY);

    return 0;
}
