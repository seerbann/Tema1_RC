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
char users[50][50];
int usersCount = 0;
int readUsersFromFile()
{
    FILE *file = fopen("users.txt", "r");
    if (file == NULL)
    {
        printf("File could not be opened\n");
        return -1;
    }

    while (usersCount < 50 && fscanf(file, "%49s", users[usersCount]) == 1)
    {
        usersCount++;
    }

    fclose(file);
    return usersCount; // Return the number of users read
}

bool isUser(const char *username)
{
    for (int i = 0; i < usersCount; i++)
    {
        if (strcmp(username, users[i]) == 0)
            return true;
    }
    return false;
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
    int ok = 1;
    char message[] = "username:";
    int messageLength = strlen(message);
    if (write(s2c, &messageLength, 4) == -1)
        printf("error line 70\n");

    if (write(s2c, message, sizeof(message)) == -1)
        printf("error line 72");

    while (ok == 1)
    {
        char userTyped[] = "default";
        int bytes_read = 0;
        int numberOfChars_rec;
        //sleep(5);
        read(c2s, &numberOfChars_rec, sizeof(numberOfChars_rec));
        memset(userTyped, 0, sizeof(userTyped));
        bytes_read = read(c2s, userTyped, numberOfChars_rec);
        if (bytes_read != 0)
        {
            
            printf("user entered\n");
            userTyped[strlen(userTyped)] = '\0';
            printf("%s\n",userTyped);
            ok = 0;
        }
    }
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
        printf("read1\n");
        memset(currCommand, 0, sizeof(currCommand));
        bytes_read = read(c2s, &currCommand, numberOfChars_rec);
        printf("read2\n");

        if (bytes_read <= 0)
            break;
        currCommand[strlen(currCommand)] = '\0';
        /* If we read quit, quit; otherwise print valid comand */
        if (strcmp(currCommand, "login") == 0)
        {
            login();
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