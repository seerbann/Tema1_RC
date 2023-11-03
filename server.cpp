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
#include <utmp.h>
#include <time.h>
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
void sendMessageToClient(const char *message)
{
    printf("sending a message to client\n");
    int messageLength = strlen(message);
    if (write(s2c, &messageLength, 4) == -1)
        printf("error line 70\n");

    if (write(s2c, message, messageLength) == -1)
        printf("error line 72");
}

void login()
{
    int ok = 1;
    char message[] = "Enter your username:";
    int messageLength = strlen(message);
    if (write(s2c, &messageLength, 4) == -1)
        printf("error line 70\n");

    if (write(s2c, message, sizeof(message)) == -1)
        printf("error line 72");

    pid_t childProcess;
    int pipefd[2];
    if (pipe(pipefd) == -1)
        exit(errno);
    // 1 write
    // 0 read
    switch (childProcess = fork())
    {
    case -1:
    {
        printf("eroare la fork");
        break;
    }
    case 0: // copil
    {
        char userChild[] = "default";
        close(pipefd[1]); // doar citeste
        memset(userChild, 0, sizeof(userChild));
        read(pipefd[0], userChild, 10);
        userChild[strlen(userChild)] = '\0';
        printf("citit de copil %s\n ", userChild);
        // verificare user
        readUsersFromFile();
        if (isUser(userChild) == true)
        {
            sendMessageToClient("you logged in succesfully.\n");
            logged_in = true;
        }
        else
            sendMessageToClient("invalid username!\n");
        close(pipefd[0]);
        break;
    }
    default:
    {                     // parinte
        close(pipefd[0]); // doar scriem

        char userTyped[] = "default";
        int numberOfChars_rec;
        printf("IM in PARENT proc\n");
        while (ok == 1)
        {

            int bytes_read = 0;
            // citire user scris de client
            read(c2s, &numberOfChars_rec, sizeof(numberOfChars_rec));

            memset(userTyped, 0, sizeof(userTyped));
            bytes_read = read(c2s, userTyped, numberOfChars_rec);
            if (bytes_read != 0)
            {
                printf("user entered: ");
                userTyped[strlen(userTyped)] = '\0';
                printf("%s", userTyped);
                ok = 0;
                // scriere user catre proces copil prin pipe
                write(pipefd[1], userTyped, numberOfChars_rec);
            }
        }
        close(pipefd[1]);
        break;
    }
    }
}

void logout()
{
    if (logged_in == true)
    {
        logged_in = false;
        sendMessageToClient("Succesfully logged out\n");
    }
    else
        sendMessageToClient("You are not logged in\n");
}

void getLoggedUsers()
{
    struct utmp *myUtmp;
    int ok = 1;
    while ((myUtmp = getutent()) != nullptr && ok == 1)
    {
        if (myUtmp->ut_type == USER_PROCESS)
        {
            printf("User: %s\n", myUtmp->ut_user);
            ok = 0;
        }
    }
}

void getProcInfo(){

}
int ok = 1;
int main()
{
    init();

    char currCommand[50] = "";
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

        if ((strcmp(currCommand, "login") == 0))
        {
            if (logged_in == false)
                login();
            else
                sendMessageToClient("already logged. please log out first\n");
        }

        else if (strcmp(currCommand, "get-logged-users") == 0)
            getLoggedUsers();
        else if (strcasecmp(currCommand, "get-proc-info") == 0)
            sendMessageToClient("[server] getting info..\n");
        else if (strcasecmp(currCommand, "logout") == 0)
            logout();
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