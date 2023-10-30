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

const char *FIFOc2s = "FIFOc2s";
const char *FIFOs2c = "FIFOs2c";
int c2s;
int s2c;
void init()
{
    c2s = open(FIFOc2s, O_WRONLY);
    if (c2s == -1)
        printf("error while opening c2s fifo\n"); // client to server, write only

    s2c = open(FIFOs2c, O_RDONLY);
    if (s2c == -1)
        printf("error while opening s2c fifo\n");
}

int main()
{
    init();
    char currCommand[] = "default";
    int ok = 1;
    char textToReceive[100];
    int bytes_read_from_s = 0;
    while (ok == 1)
    {
        cin >> currCommand;
        int numberOfChars = strlen(currCommand);
        if (strcmp(currCommand, "quit") == 0)
            ok = 0;

        write(c2s, currCommand, numberOfChars);
    }
}