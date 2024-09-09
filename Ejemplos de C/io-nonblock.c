/*
** non blocking demo
*/

#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>

#define STDIN 0  // file descriptor for standard input

int main(void)
{
    char line[10];
    int n;
	fcntl(STDIN, F_SETFL, O_NONBLOCK);
        n=read(STDIN,line,10);
        if (n > 0) printf("A key was pressed! %d\n",n);
        if (n == 0) printf("Oops n=0\n");
        if (n == -1) printf("An error: %s\n", strerror(errno));
	
    return 0;
} 
