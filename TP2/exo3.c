#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>


int main(int argc, char const *argv[])
{
    struct sockaddr_in adrso;
    memset(&adrso, 0, sizeof(adrso));
    adrso.sin_family = AF_INET;
    adrso.sin_port = htons(2121);
    adrso.sin_addr.s_addr = htonl(INADDR_ANY);
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1)
    {
        perror("socket");
        exit(1);
    }
    if (connect(sock, (struct sockaddr *)&adrso, sizeof(adrso)) == -1)
    {
        perror("connect");
        exit(1);
    }
    char buf[1024];
    int n = read(sock, buf, sizeof(buf));
    if (n == -1)
    {
        perror("read");
        exit(1);
    }
    buf[n] = '\0';
    printf("%s", buf);
    close(sock);
    
    return 0;
}
