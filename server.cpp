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
#include <sys/socket.h>
using namespace std;

bool logged_in = false;
const char *FIFOc2s = "FIFOc2s";
const char *FIFOs2c = "FIFOs2c";
int c2s;
int s2c;
char users[50][50];
int usersCount = 0;
void readUsersFromFile()
{
    FILE *file = fopen("users.txt", "r");
    if (file == NULL)
    {
        printf("error in opening file\n");
    }

    while (fscanf(file, "%s", users[usersCount]) == 1)
        usersCount++;
    
    fclose(file);
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
    printf("[server]sending a message to client\n");
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
                printf("%s\n", userTyped);
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
    int sockp[2];
    if (socketpair(AF_UNIX, SOCK_STREAM, 0, sockp) == -1)
    {
        printf("error creating socket\n");
    }
    switch (fork())
    {
    case -1:
    {
        printf("error in fork linia 174\n");
        break;
    }

    case 0: // copil
    {
        close(sockp[0]);
        struct utmp *myUtmp;
        int ok = 1;
        char user[32] = "";
        while ((myUtmp = getutent()) != nullptr && ok == 1)
        {
            if (myUtmp->ut_type == USER_PROCESS)
            {
                printf("User: %s\n", myUtmp->ut_user);
                strcpy(user, myUtmp->ut_user);
                ok = 0;
            }
        }
        if (write(sockp[1], user, 20) == -1)
            printf("errorr line 195");
        printf("[copil]  %s\n", user);
        close(sockp[1]);
    }
    default:
    {
        char user[20];
        close(sockp[1]);
        if (read(sockp[0], user, sizeof(user)) == -1)
            printf("[parinte]Err...write");
        printf("%s", user);
        sendMessageToClient((const char *)user);
        close(sockp[0]);
    }
    }
}

//not done
void getProcInfo()
{    
    int ok = 1;
    char message[] = "Enter pid:";
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
        char pidChild[] = "default";
        close(pipefd[1]); // doar citeste
        memset(pidChild, 0, sizeof(pidChild));
        read(pipefd[0], pidChild, 10);
        pidChild[strlen(pidChild)] = '\0';
        printf("citit de copil %s\n ", pidChild);
        // verificare pid
        sendMessageToClient("pid citit\n");
        
        
        
        close(pipefd[0]);
        break;
    }
    default:
    {                     // parinte
        close(pipefd[0]); // doar scriem

        char pidTyped[] = "default";
        int numberOfChars_rec;
        printf("IM in PARENT proc\n");
        while (ok == 1)
        {

            int bytes_read = 0;
            // citire pid scris de client
            read(c2s, &numberOfChars_rec, sizeof(numberOfChars_rec));

            memset(pidTyped, 0, sizeof(pidTyped));
            bytes_read = read(c2s, pidTyped, numberOfChars_rec);
            if (bytes_read != 0)
            {
                printf("pid entered: ");
                pidTyped[strlen(pidTyped)] = '\0';
                printf("%s", pidTyped);
                ok = 0;
                // scriere user catre proces copil prin pipe
                write(pipefd[1], pidTyped, numberOfChars_rec);
            }
        }
        close(pipefd[1]);
        break;
    }
    }
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
        switch (logged_in)
        {
        case false:
            if ((strcmp(currCommand, "login") == 0))
            {
                login();
            }
            else if (strcasecmp(currCommand, "quit") == 0)
                sendMessageToClient("[server]quiting\n");

            else
                sendMessageToClient("[server] invalid request\n");
            break;

        default: // true
            if ((strcmp(currCommand, "login") == 0))
            {
                sendMessageToClient("already logged. please log out first\n");
            }
            else if (strcmp(currCommand, "get-logged-users") == 0)
                getLoggedUsers();
            else if (strcasecmp(currCommand, "get-proc-info") == 0)
                getProcInfo();
            else if (strcasecmp(currCommand, "logout") == 0)
                logout();
            else if (strcasecmp(currCommand, "quit") == 0)
                sendMessageToClient("[server]quiting\n");

            else
                sendMessageToClient("[server] invalid request\n");

            break;
        }
    }
    printf("[server]quiting\n");
    /* close and delete the c2s FIFO */
    close(s2c);
    unlink(FIFOs2c);
    close(c2s);
    unlink(FIFOc2s);
}