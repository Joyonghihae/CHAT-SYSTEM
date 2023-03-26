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
    char user[ID_SIZE + 1];
    char ipAdd[IP_SIZE];
    char usage[] = "USAGE : chat-client -user<userID> -server<server name>\n";
    char sanityBuffer[25];
    int argv_server_length = 7;
    int argv_length = 0;
    int ret_val = 0;

    MESSAGE client_message = { 0 };

    // time stamps
    char timestamp[11] = { 0 };
    time_t rawtime;
    struct tm* timeinfo;
    time(&rawtime);

    timeinfo = localtime(&rawtime);
    sprintf(timestamp, "(%02d:%02d:%02d)", timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec);

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
        printf("user: %s\n", user);
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
        printf("ipAdd: %s\n", ipAdd);
    }

    // get hostent structure with the hostname
    if ((host = gethostbyname(ipAdd)) == NULL)
    {
        // convert IPaddress to the hostname
        inet_aton(ipAdd, &ip_address);
        if ((host = gethostbyaddr(&ip_address, sizeof(ip_address), AF_INET)) == NULL)
        {
            printf("[CLIENT ERROR] : Host/IP Address information\n");
            return 2;
        }
    }
    else
    {
        strcpy(client_message.id, user);
        strcpy(client_message.ipAddress, ipAdd);
        printf("start server user: %s, ipAdd %s\n", client_message.id, client_message.ipAddress);

        ret_val = startClient(host, &client_message);
        printf("startclient return: %d\n", ret_val);
    }



    return 0;
}
