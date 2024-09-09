/* 
 *  A echo server using select()
 */

#include <stdio.h>
#include <string.h> /* memset() */
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netdb.h>

#define PORT    1500 /* Port to listen on */
#define BACKLOG     10  /* Passed to listen() */
#define MAX_LEN	    1024

void handle(int newsock, fd_set *set)
{
    /* send(), recv(), close() */
    /* Call FD_CLR(newsock, set) on disconnection */
    int n;
    char buffer[MAX_LEN];

    n = recv(newsock, buffer, MAX_LEN, 0 );
    if ( n == 0) {
      close(newsock);
      FD_CLR(newsock, set);
    } 
    if ( n > 0) send(newsock, buffer, n , 0);
}

int main(void)
{
    int sock;
    fd_set socks;
    fd_set readsocks;
    int maxsock;

    struct addrinfo hints, *res;
    struct sockaddr_in servaddr;

    /* Create the socket */
    sock = socket(AF_INET,SOCK_STREAM,0);
    if (sock == -1) {
        perror("socket");
        return 1;
    }


    /* Fill the server address */
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr=htonl(INADDR_ANY);
    servaddr.sin_port=htons(PORT);

    /* Bind to the address */
    if (bind(sock, (struct sockaddr *)&servaddr, sizeof(servaddr)) == -1) {
        perror("bind");
        return 1;
    }


    /* Listen */
    if (listen(sock, BACKLOG) == -1) {
        perror("listen");
        return 1;
    }

    /* Set up the fd_set */
    FD_ZERO(&socks);
    FD_SET(sock, &socks);
    maxsock = sock;

    /* Main loop */
    while (1) {
        unsigned int s;
        readsocks = socks;
        if (select(maxsock + 1, &readsocks, NULL, NULL, NULL) == -1) {
            perror("select");
            return 1;
        }
        for (s = 0; s <= maxsock; s++) {
            if (FD_ISSET(s, &readsocks)) {
                printf("socket %d was ready\n", s);
                if (s == sock) {
                    /* New connection */
                    int newsock;
                    struct sockaddr_in their_addr;
                    socklen_t size = sizeof(struct sockaddr_in);
                    newsock = accept(sock, (struct sockaddr*)&their_addr, &size);
                    if (newsock == -1) {
                        perror("accept");
                    }
                    else {
                        printf("Got a connection from %s on port %d\n", 
                                inet_ntoa(their_addr.sin_addr), htons(their_addr.sin_port));
                        FD_SET(newsock, &socks);
                        if (newsock > maxsock) {
                            maxsock = newsock;
                        }
                    }
                }
                else {
                    /* Handle read or disconnection */
                    handle(s, &socks);
                }
            }
        }

    }

    close(sock);

    return 0;
}
