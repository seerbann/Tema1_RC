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

const char *FIFO1 = "MY_FIFO_c2s";
const char *FIFO2 = "MY_FIFO_s2c";
int c2s;
int s2c;
void init()
{
    c2s = open(FIFO1, O_WRONLY); // client to server, write only
    s2c = open(FIFO2, O_RDONLY);
}

int main()
{
    init();
    char currCommand[100];
    int ok = 1;
    char textToReceive[100];
    int bytes_read_from_s = 0;
    while (ok == 1)
    {
        cin >> currCommand;
        if (strcmp(currCommand, "quit") == 0)
            ok = 0;
        write(c2s, currCommand, sizeof(currCommand));
    }
}