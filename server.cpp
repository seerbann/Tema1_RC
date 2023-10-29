#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <vector>
#include <string>
using namespace std;
vector<string> commands;
int main()
{
    /* Create the FIFO or die trying */
    const char *FIFO1 = "MY_FIFO_c2s";
    mknod(FIFO1, S_IFIFO | 0666, 0);
    int c2s = open(FIFO1, O_RDONLY); // client to server , read only

    const char *FIFO2 = "MY_FIFO_s2c";
    mknod(FIFO2, S_IFIFO | 0666, 0);
    int s2c = open(FIFO2, O_WRONLY); // server to client , write only

    /* Try to open the FIFO. Delete FIFO if open() fails */

    if (c2s == -1)
    {
        fprintf(stderr, "Failed to open FIFO\n");
        unlink(FIFO1);
        return 1;
    }

    /* Main server loop */

    char currCommand[1024];
    int bytes_read;
    char textToSend[100];

    while (1)
    {
        bytes_read = read(c2s, currCommand, sizeof(currCommand));
        if (bytes_read <= 0)
            break;

        /* If we read quit, quit; otherwise print valid comand */
        if (strcasecmp(currCommand, "quit") == 0)
            break;
        else if (strcasecmp(currCommand, "login") == 0)
        {
            write(s2c, textToSend, sizeof(textToSend));
        }

        else if (strcasecmp(currCommand, "get-logged-users") == 0)
            printf("get-logged-users\n");
        else if (strcasecmp(currCommand, "get-proc-info") == 0)
            printf("get-proc-info\n");
        else if (strcasecmp(currCommand, "logout") == 0)
            printf("logout\n");
        else
            printf("invalid command\n");
    }

    /* close and delete the FIFOs */
    close(c2s);
    close(s2c);
    printf("Deleting FIFOs\n");
    unlink(FIFO1);
    unlink(FIFO2);
}