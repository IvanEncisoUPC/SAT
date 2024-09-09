/* 
 *  A forked echo server
 */


#include <string.h> /* memset() */
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h> /* exit() */
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/wait.h>
#include <netdb.h>
#include <sys/types.h>


#define PORT    1500 /* Port to listen on */
#define BACKLOG     10  /* Passed to listen() */
#define MAX_LEN	    1024


int handle(int newsock)
{
    /* recv(), send(), close() */
    char msg[40],buffer[MAX_LEN];
    pid_t pid;
    int n, sv;

    pid = getpid();
    sprintf(msg,"You are served by process %d\n", pid);
    write(newsock, msg, strlen(msg));


    while ((n = recv(newsock, buffer, MAX_LEN, 0 )) != 0) {
         if (n < 0) continue;
         send(newsock, buffer, n, 0);

    }

    close(newsock);
}

int main(void)
{
    int sock;
    struct sockaddr_in servaddr;
    struct sigaction sa;

    /* Get the address info */

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

    /* Set up the signal handler */
	signal(SIGCHLD, SIG_IGN);


    /* Main loop */
    while (1) {
        struct sockaddr_in their_addr;
        socklen_t size = sizeof(struct sockaddr_in);
        int newsock = accept(sock, (struct sockaddr*)&their_addr, &size);
        int pid;

        if (newsock == -1) {
            perror("accept");
            return 0;
        }

        printf("Got a connection from %s on port %d\n", inet_ntoa(their_addr.sin_addr), htons(their_addr.sin_port));

        pid = fork();
        if (pid == 0) {
            /* In child process */
            close(sock);
            handle(newsock);
            return 0;
        }
        else {
            /* Parent process */
            if (pid == -1) {
                perror("fork");
                return 1;
            }
            else {
                close(newsock);
            }
        }
    }

    close(sock);

    return 0;
}