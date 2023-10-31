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

    s2c = open(FIFOs2c, O_RDONLY); // client to server, read only
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
        int numberOfChars_sent = strlen(currCommand);
        int numberOfChars_rec = -1;
        if (strcmp(currCommand, "quit") == 0)
            ok = 0;

        if (write(c2s, &numberOfChars_sent, sizeof(numberOfChars_sent)) == -1)
            printf("problem line 44 in client\n");

        if (write(c2s, currCommand, numberOfChars_sent) != -1)
        {
            // reading from server
            int messageLength = 0, bytes = -1;
            if (read(s2c, &messageLength, 4) == -1)
                printf("problem reading messageLength\n");
                printf("read1 client\n");
            // Read the message
            char message[messageLength + 1]; // +1 for null-terminator
            if (read(s2c, message, messageLength) == -1)
                printf("error line 57\n");
                printf("read2 in client\n");
            message[messageLength] = '\0'; // Null-terminate the received message
            printf("%s\n", message);
        }
    }
    close(c2s);
    close(s2c);
}