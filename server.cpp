#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <vector>
#include <strings.h>
#include <fstream>
#include <iostream>
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
    c2s = open(FIFOc2s, O_RDONLY); // client -> server,  read only for server
    s2c = open(FIFOs2c, O_WRONLY); // server -> client,   write only for server
    if (c2s == -1)
        printf("c2s not working\n");
    if (s2c == -1)
        printf("s2c not working\n");
    printf("Connection enabled. \n");
}

void login()
{
    int numberCharacters = 11;
    write(s2c, &numberCharacters, sizeof(int));
    if (write(s2c, "username: ", numberCharacters) == -1) /// writing to client to say username
        perror("[login/parent] Eroare la scrierea prompt-ului username catre client\n");
    else
        printf("[login/parent] Am scris catre client: username. \n");
}

void sendMessageToClient(const char *message)
{
    printf("sending a message to client\n");
    int messageLength = strlen(message);
    if (write(s2c, &messageLength, 4) == -1)
        printf("error line 70\n");

    if (write(s2c, message, messageLength) == -1)
        printf("error line 72");
}

int ok = 1;
int main()
{
    init();
    /* Main server loop */

    char currCommand[50] = "";
    char newCurrCommand[] = "";
    int bytes_read;
    int numberOfChars_rec = 0;
    while (strcmp(currCommand, "quit") != 0)
    {
        read(c2s, &numberOfChars_rec, sizeof(numberOfChars_rec));
        // reseting the buffer
        memset(currCommand, 0, sizeof(currCommand));
        bytes_read = read(c2s, &currCommand, numberOfChars_rec);
        if (bytes_read <= 0)
            break;
        currCommand[strlen(currCommand)] = '\0';
        /* If we read quit, quit; otherwise print valid comand */
        if (strcmp(currCommand, "login") == 0)
        {
            sendMessageToClient("[server]logging in..\n");
        }

        else if (strcmp(currCommand, "get-logged-users") == 0)
            sendMessageToClient("[server] getting info..\n");
        else if (strcasecmp(currCommand, "get-proc-info") == 0)
            sendMessageToClient("[server] getting info..\n");
        else if (strcasecmp(currCommand, "logout") == 0)
            sendMessageToClient("[server]logging out..\n");
        else if (strcasecmp(currCommand, "quit") == 0)
            sendMessageToClient("[server]quiting\n");

        else
            sendMessageToClient("[server] invalid request\n");
    }
    printf("[server]quiting\n");
    /* close and delete the c2s FIFO */
    close(s2c);
    unlink(FIFOs2c);
    close(c2s);
    unlink(FIFOc2s);
}