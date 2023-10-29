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
    const char *FIFO = "MY_FIFO";
    char currCommand[100];
    int c2s = open(FIFO, O_WRONLY); // client to server, write only

    while (cin >> currCommand)
    {
        write(c2s, currCommand, sizeof(currCommand));
    }
    /* Close the FIFO */
    close(c2s);
}