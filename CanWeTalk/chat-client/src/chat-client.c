// FILE          : chat-client.c
// PROJECT       : CanWeTalk
// programmer    : Euyoung Kim, Raj Dudhat, Yujin Jeong, Yujung Park
// FIRST VERSION : 2023-03-18
// DESCRIPTION   : This is a chat-client.c file for chat-client application of CanWeTalk System.
// chat-client application uses ncurese UI and receives user message input,
// and send the message to the server.

#include "../inc/chat-client.h"

int main(int argc, char* argv[])
{
    struct hostent* host;
    struct in_addr ip_address;

    char ipAdd[IP_SIZE];
    char usage[] = "USAGE : chat-client -user<userID> -server<server name>\n";
    char sanityBuffer[25];
    int argv_server_length = 7;
    int argv_length = 0;
    int ret_val = 0;

    client_message = (MESSAGE*)malloc(sizeof(MESSAGE));
    pthread_mutex_init(&mtx, NULL);

    // cmd args sanity check
    if (argc != 3)
    {
        printf("1 %s\n", usage);
        return -1;
    }
    // check argv[1]
    else
    {
        argv_length = strlen(argv[1]);
        if (argv_length == 5 || argv_length > 10)
        {
            printf("2 %s\n", usage);
            return -1;
        }
    }

    // check -user
    strncpy(sanityBuffer, argv[1], ID_SIZE);
    sanityBuffer[ID_SIZE] = '\0';
    if (strcmp(sanityBuffer, "-user") != 0)
    {
        printf("3 %s\n", usage);
        return -1;
    }
    else
    {
        strcpy(sanityBuffer, argv[1]);
        strncpy(user, sanityBuffer + ID_SIZE, strlen(sanityBuffer) - ID_SIZE + 1);
    }
    // check argv[2]
    strncpy(sanityBuffer, argv[2], argv_server_length);
    sanityBuffer[argv_server_length] = '\0';
    if (strcmp(sanityBuffer, "-server") != 0)
    {
        printf("4 %s\n", usage);
        return -1;
    }
    else
    {
        strcpy(sanityBuffer, argv[2]);
        strncpy(ipAdd, sanityBuffer + argv_server_length, strlen(sanityBuffer) - argv_server_length + 1);
    }

    // get hostent structure with the hostname
    if ((host = gethostbyname(ipAdd)) == NULL)
    {
        // convert server IPaddress to the hostname
        inet_aton(ipAdd, &ip_address);
        if ((host = gethostbyaddr(&ip_address, sizeof(ip_address), AF_INET)) == NULL)
        {
            printf("5 %s\n", usage);
            return -2;
        }
    }
    else
    {
        pthread_mutex_lock(&mtx);
        strcpy(client_message->id, user);
        pthread_mutex_unlock(&mtx);
        // start client
        startClient(host);
    }

    printf("bye\n");
    free(client_message);
    pthread_mutex_destroy(&mtx);

    return 0;
}
