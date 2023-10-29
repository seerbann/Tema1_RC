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
    const char *FIFO1 = "MY_FIFO";
    mknod(FIFO1, S_IFIFO | 0666, 0);
    int c2s = open(FIFO1, O_RDONLY); // client to server , read only
        
    /* Try to open the FIFO. Delete FIFO if open() fails */

    if (c2s == -1)
    {
        fprintf(stderr, "Failed to open FIFO\n");
        unlink(FIFO1);
        return 1;
    }

    /* Main server loop */ 
    while (1)
    {
        char currCommand[1024];
        int bytes_read;
        bytes_read = read(c2s, currCommand, sizeof(currCommand));
        if(bytes_read<=0)
            break;    
        
        /* If we read quit, quit; otherwise print valid comand */
        if (strcmp(currCommand,"quit")==0)
            break;
        printf("valid command\n");
    }

    /* Read QUIT from the FIFO, so close and delete the FIFO */
    close(c2s);
    printf("Deleting FIFO\n");
    unlink(FIFO1);
}