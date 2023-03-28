// FILE          : chat-client.c
// PROJECT       : CanWeTalk
// programmer    : Eunyoung Kim, Raj Dudhat, Yujin Jeong, Yujung Park
// FIRST VERSION : 2023-03-18
// DESCRIPTION   : This is an internet server application that will respond
// to requests on port 5000
// REFERENCE     : ncurses - splitWin.c is a simple example to show how to deal with split screens. Sam Hsu (11/17/10)


#include "../inc/chat-server.h"



// FUNCTION   : startServer()
// DESCRIPTION: This function operates server by initiating the server,
// and accepting clients, receiving message from customers,
// and broadcasting messages to connected clients until all clients are disconnected.
// PARAMETERS : NONE
// RETURN     : int retvalue
int startServer()
{
    int server_socket = 0;
    int client_socket = 0;
    int queue_pending = MAX_CLIENTS;
    int client_length = 0;
    int counter = 0;
    int thread_index = 0;
    char logText[LOG_BUFFER_SIZE] = { 0 };
    struct sockaddr_in server_addr;
    struct sockaddr_in client_addr;
    pthread_t tid[MAX_CLIENTS];

    // create socket
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket < 0)
    {
        sprintf(logText, "[SERVER ERROR-1] server socket creation\n");
        return -1;
    }

    // initiate the server address info
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(PORT);

    // bind the socket and the server address
    if (bind(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0)
    {
        sprintf(logText, "[SERVER ERROR-2] server socket binding\n");
        writeLogFile(SERVER, logText);
        close(server_socket);
        return -2;
    }

    // Listen socket
    if (listen(server_socket, queue_pending) < 0)
    {
        sprintf(logText, "[SERVER ERROR-3] server socket listening\n");
        close(server_socket);
        return -3;
    }

    // allocate memory and set vaules
    clientsMasterList = (MasterList*)malloc(sizeof(MasterList));
    clientsMasterList->client_connections = 0;
    for (counter = 0; counter < MAX_CLIENTS; counter++)
    {
        // use 0 (stdin file descriptor) for default value
        clientsMasterList->clients[counter].socket = 0;
    }

    // Accept client
    while (server_run == TRUE)
    {
        // accept a packet from client
        client_length = sizeof(client_addr);
        client_socket = accept(server_socket, (struct sockaddr*)&client_addr, &client_length);
        printf("client_socket: %d\n", client_socket);
        if (client_socket < 0)
        {
            sprintf(logText, "[SERVER ERROR-4] accept packet from the client\n");
            close(server_socket);
            return -4;
        }

        // maintain clientsMasterList
        pthread_mutex_lock(&mtx);
        clientsMasterList->clients[clientsMasterList->client_connections].socket = client_socket;
        strcpy(clientsMasterList->clients[clientsMasterList->client_connections].ipAddress, inet_ntoa(client_addr.sin_addr));
        clientsMasterList->clients[clientsMasterList->client_connections].port = ntohs(client_addr.sin_port);
        thread_index = clientsMasterList->client_connections;

        printf("client_connections: %d, socket: %d, port: %d, ipAddress: %s\n",
            clientsMasterList->client_connections,
            clientsMasterList->clients[clientsMasterList->client_connections].socket,
            clientsMasterList->clients[clientsMasterList->client_connections].port,
            clientsMasterList->clients[clientsMasterList->client_connections].ipAddress);

        clientsMasterList->client_connections++;
        pthread_mutex_unlock(&mtx);



        // create thread to be responsible for incoming  message the user and broadcasting to all users
        if (pthread_create(&(tid[thread_index]), NULL, clientThread, (void*)&client_socket))
        {
            sprintf(logText, "[SERVER ERROR-5] thread creation\n");
            return -5;
        }
        printf("thread creation for client %d\n", thread_index + 1);
        // release resources, detach does not requires the main thread join() with child thread. Server does not wait for each thread completion. so server handles the next client connection.
        pthread_detach(tid[thread_index]);

    }

    free(clientsMasterList);
    close(server_socket);
    return 1;
}


// FUNCTION   : clientThread()
// DESCRIPTION: This function
//
// PARAMETERS : void *socket
// RETURN     : NULL
void* clientThread(void* socket)
{
    int clientSocket = *((int*)socket);
    int numBytesRead = 0;
    int counter = 0;
    int threadIndex = 0;
    char msg[40];
    char quit[] = ">>bye<<";
    MESSAGE message_received;
    pthread_t receive_thread[MAX_CLIENTS];
    pthread_t broadcast_thread[MAX_CLIENTS];

    while (1)
    {
        // receive message from client and broadcast the message to clients
        numBytesRead = recv(clientSocket, &message_received, sizeof(MESSAGE), FLAG);
        // find the ip address for the socket and update message struct. IP add


        if (numBytesRead == 0 || strcmp(message_received.chat, quit) == 0)
        {
            // collapse MasterList, remove the client
            pthread_mutex_lock(&mtx);
            collapseMasterList(clientSocket);
            close(clientSocket);
            clientsMasterList->client_connections--;
            if (clientsMasterList->client_connections == 0)
            {
                server_run = FALSE;
                printf("serverrun=false\n");
            }
            printf("%d socket is closed and after collapse client no is %d\n", clientSocket, clientsMasterList->client_connections);
            pthread_mutex_unlock(&mtx);
            break;
        }
        else {
            // printf("byteread: %d\t %s\n", numBytesRead, message_received.chat);
            broadcast(&message_received);
        }

    }
    printf("thread exit\n");
    pthread_exit(NULL);
}


// FUNCTION   : collapseMasterList()
// DESCRIPTION: This function updates struct mlist of struct MasterList
// with received message queue information
// PARAMETERS : msgDC* msg
//              MasterLIst* mlist
//              int index
// RETURN     : Nothing
void collapseMasterList(int clientSocket)
{
    int counter = 0;
    for (counter = 0; counter < clientsMasterList->client_connections; counter++)
    {
        if (clientSocket == clientsMasterList->clients[counter].socket)
        {
            while (counter++ < clientsMasterList->client_connections - 1)
            {
                clientsMasterList->clients[counter].socket =
                    clientsMasterList->clients[counter + 1].socket;
                clientsMasterList->clients[counter].port =
                    clientsMasterList->clients[counter + 1].port;
                strcpy(clientsMasterList->clients[counter].ipAddress,
                    clientsMasterList->clients[counter + 1].ipAddress);
                strcpy(clientsMasterList->clients[counter].userID,
                    clientsMasterList->clients[counter + 1].userID);
                // debug
                printf("CollapseML index:%d socket:%d\n", counter, clientsMasterList->clients[counter].socket);
                // debug
                printf("CollapseML index:%d socket:%d\n", counter + 1, clientsMasterList->clients[counter + 1].socket);
            }
            break;
        }
    }

}


// FUNCTION   : broadcast()
// DESCRIPTION: This function updates struct mlist of struct MasterList
// with received message queue information
// PARAMETERS : msgDC* msg
//              MasterLIst* mlist
//              int index
// RETURN     : Nothing
void broadcast(MESSAGE* message)
{
    int counter = 0;

    for (counter = 0; counter < clientsMasterList->client_connections; counter++)
    {
        send(clientsMasterList->clients[counter].socket, message, sizeof(MESSAGE), FLAG);
    }
    printf("Broadcast ipAdd:%s id:%s message:%s\n", message->ipAddress, message->id, message->chat);
}
