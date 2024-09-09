/*
** Blocking
*/

#include <stdio.h>
#define STDIN 0  // file descriptor for standard input

int main(void)
{
    char line[10];

        read(STDIN,line,10);
        printf("A key was pressed!\n"); 
    return 0;
} 
