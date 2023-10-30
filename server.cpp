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
const char *FIFOc2s = "FIFOc2s";
const char *FIFOs2c = "FIFOs2c";
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

  mknod(FIFOc2s, S_IFIFO | 0666, 0);
  mknod(FIFOs2c, S_IFIFO | 0666, 0);
  printf("Establishing connection to client. \n");
  c2s = open(FIFOc2s, O_RDONLY); // client -> server   =>    read only for server
  s2c = open(FIFOs2c, O_WRONLY); // server -> client   =>    write only for server
  printf("Connection to client established. \n");
}

int main()
{
    init();
    if (c2s == -1)
    {
        printf("Failed to open FIFO1\n");
        unlink(FIFOc2s);
        return 1;
    }
    if (s2c == -1)
    {
        printf("Failed to open FIFO2\n");
        unlink(FIFOs2c);
        return 1;
    }
    printf("Connection enabled\n");
    /* Main server loop */

    char currCommand[] = "default";
    int bytes_read;
    while (strcmp(currCommand, "quit") != 0)
    {
        bytes_read = read(c2s, currCommand, 20);
        if (bytes_read <= 0)
            break;

        /* If we read quit, quit; otherwise print valid comand */
        if (strcasecmp(currCommand, "login") == 0)
        {
            printf("loggin in");
            char textToSend[100];
            strcpy(textToSend, "default message");
            int nOfChars = strlen(textToSend);
            // write(s2c, textToSend, nOfChars);
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
    printf("quiting\n");
    /* close and delete the c2s FIFO */
    close(s2c);
    unlink(FIFOs2c);
    close(c2s);
    unlink(FIFOc2s);
}