#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string>
using namespace std;

int main()
{
    const char *FIFO1 = "MY_FIFO_c2s";
    const char *FIFO2 = "MY_FIFO_s2c";
    char currCommand[100];
    int c2s = open(FIFO1, O_WRONLY); // client to server, write only
    int s2c = open(FIFO2, O_RDONLY);
    int ok = 1;
    char textToReceive[100];
    while (cin >> currCommand && ok == 1)
    {
        if (strcmp(currCommand, "quit") == 0)
            ok = 0;
        write(c2s, currCommand, sizeof(currCommand));

        if (read(s2c, textToReceive, 10) != -1)
        {
            printf("%s",textToReceive);
            printf("\n");
        }
    }
    /* Close the FIFO */
    close(c2s);
}