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
#include <fstream>
using namespace std;
bool logged_in = false;
const char *FIFO1 = "MY_FIFO_c2s";
const char *FIFO2 = "MY_FIFO_s2c";
int c2s;
int s2c;
vector<string> readUsersFromFile()
{
    vector<string> users;
    ifstream usersFile;

    if (usersFile.is_open())
    {
        string user;
        while (getline(usersFile, user))
        {
            users.push_back(user);
        }
        usersFile.close();
    }
    else
    {
        printf("Unable to open file");
    }

    return users;
}

void init()
{
    mknod(FIFO1, S_IFIFO | 0666, 0);
    c2s = open(FIFO1, O_RDONLY); // client to server , read only
    mknod(FIFO2, S_IFIFO | 0666, 0);
    s2c = open(FIFO2, O_WRONLY); // server to client , write only
}

int main()
{
    init();
    if (c2s == -1)
    {
        fprintf(stderr, "Failed to open FIFO1\n");
        unlink(FIFO1);
        return 1;
    }
    if (s2c == -1)
    {
        fprintf(stderr, "Failed to open FIFO2\n");
        unlink(FIFO2);
        return 1;
    }
    printf("Connection enabled\n");
    /* Main server loop */

    char currCommand[1024];
    int bytes_read;

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
            printf("loggin in");
            char textToSend[100];
            strcpy(textToSend, "default message");
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

    /* close and delete the c2s FIFO */
}