/* Exemple servidor TCP  */

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h> /* exit() */

#define LOCAL_SERVER_PORT 1500
#define MAX_MSG 1024
#define BACKLOG 2

#define STDOUT 1

int main(int argc, char *argv[])
{

   int listenfd,connfd,n;
   struct sockaddr_in servaddr,cliaddr;
   socklen_t clilen;
   char msg[MAX_MSG];

   /* Creació del socket per rebre connexions*/
   if ((listenfd=socket(AF_INET,SOCK_STREAM,0))<0) {
      printf("%s: Error a la creació del socket \n",argv[0]);
      exit(1);
   }

   /* Assignació de la adreça i el port de servei */
 
   servaddr.sin_family = AF_INET;
   /* INADDR_ANY : S'acceptaran connexions pel qualsevol
    * adreça IP del servidor (eth0, eth1, lo)
    */
   servaddr.sin_addr.s_addr=htonl(INADDR_ANY);
   /* Assignació del port de servei */
   servaddr.sin_port=htons(LOCAL_SERVER_PORT);

   /* bind: Es lliga el descriptor de socket listenfd amb l'adreça de servei */
   bind(listenfd,(struct sockaddr *)&servaddr,sizeof(servaddr));

   /* posem el socket en estat de "listen" a l'espera
    * de connexions de clients. backlog = 3 vol dir
    * que mentre el servidor està servint un client
    * a la cua poden haver-hi com a màxim 3 clients
    * esperant ser servits. Si arriba un quart client
    * el sistema operatiu li retornarà un error de connexió
    */
   listen(listenfd,BACKLOG);

   for(;;)
   {
      clilen=sizeof(cliaddr);
      /* Quan un client es connecta i s'accepta la seva connexió,
       * connfd és el descriptor de socket que farem servir per a
       * comunicar-nos amb aquest client
       */
      connfd = accept(listenfd,(struct sockaddr *)&cliaddr,&clilen);
      fprintf(stderr,"Connexió des de IP %s, Port %d\n",inet_ntoa(cliaddr.sin_addr),ntohs(cliaddr.sin_port));

      /* programem un bucle per rebre els missatges del client
       * si el número de bytes rebut en una lectura del socket
       * es 0, vol dir que el client ha tancat la connexió */

      while ((n = recv(connfd, msg, MAX_MSG, 0)) != 0) {
	    if (n < 0) continue;
        /* Les dades rebudes s'escriuen a l'estandard "output" */
         write(STDOUT,msg,n); 
      }
      close(connfd);
   } /* Final del bucle infinit del servidor */
   return 0;
}
