#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <time.h>
#include <unistd.h>

#define MAX_CLIENTS 1
#define BUFFER_SIZE 512
#define QUESTION_TIME 10
#define NUM_QUESTIONS 5
#define MAX_NUM 20

int main(int argc, char *argv[])
{
    int server_socket, client_socket, activity, i, j, valread;
    struct sockaddr_in server_addr, client_addr;
    char buffer[BUFFER_SIZE];
    int num_clients = 0;
    int score = 0;

    // Create server socket
    if ((server_socket = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // Set server socket to allow multiple connections
    int opt = 1;
    if (setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) <
        0)
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

    printf("Server started. Waiting for incoming connections...\n");

    // Wait for incoming connections and handle them
    while (1)
    {
        // Accept incoming connection
        if ((client_socket =
                 accept(server_socket, (struct sockaddr *)&client_addr,
                        (socklen_t *)&j)) < 0)
        {
            perror("accept failed");
            exit(EXIT_FAILURE);
        }

        printf("New connection, socket fd is %d, ip is : %s, port : %d\n",
               client_socket, inet_ntoa(client_addr.sin_addr),
               ntohs(client_addr.sin_port));

        // Send welcome message to the new client
        char welcome_message[] = "Welcome to the multiplication game! You have "
                                 "10 seconds to answer each question.\n";
        if (send(client_socket, welcome_message, strlen(welcome_message), 0) !=
            strlen(welcome_message))
        {
            perror("send failed");
            exit(EXIT_FAILURE);
        }

        // Play the game with the client
        srand(time(NULL));
        int num1, num2, answer, result;
        for (i = 0; i < NUM_QUESTIONS; i++)
        {
            // Generate random multiplication question
            num1 = rand() % MAX_NUM + 1;
            num2 = rand() % MAX_NUM + 1;
            result = num1 * num2;
            sprintf(buffer, "Question %d: %d x %d = ?\n", i + 1, num1, num2);
            // Send question to client
            if (send(client_socket, buffer, strlen(buffer), 0) !=
                strlen(buffer))
            {
                perror("send failed");
                exit(EXIT_FAILURE);
            }

            // Set up timer
            int time_left = QUESTION_TIME;
            while (time_left > 0)
            {
                // Send countdown to client
                sprintf(buffer, "%d\n", time_left);
                if (send(client_socket, buffer, strlen(buffer), 0) !=
                    strlen(buffer))
                {
                    perror("send failed");
                    exit(EXIT_FAILURE);
                }
                sleep(1);
                time_left--;
            }

            // Receive client's answer
            memset(buffer, 0, BUFFER_SIZE);
            valread = read(client_socket, buffer, BUFFER_SIZE);
            if (valread < 0)
            {
                perror("read failed");
                exit(EXIT_FAILURE);
            }

            // Check client's answer
            answer = atoi(buffer);
            if (answer == result)
            {
                score++;
            }
            else
            {
                // Send "TROP LENT !" message
                char too_slow[] = "TROP LENT !\n";
                if (send(client_socket, too_slow, strlen(too_slow), 0) !=
                    strlen(too_slow))
                {
                    perror("send failed");
                    exit(EXIT_FAILURE);
                }
            }
        }

        // Send final score to client
        sprintf(buffer, "Your final score is: %d\n", score);
        if (send(client_socket, buffer, strlen(buffer), 0) != strlen(buffer))
        {
            perror("send failed");
            exit(EXIT_FAILURE);
        }

        // Close client socket
        close(client_socket);
        printf("Client disconnected, socket fd is %d, ip is : %s, port : %d\n",
               client_socket, inet_ntoa(client_addr.sin_addr),
               ntohs(client_addr.sin_port));
    }

    return 0;
}
