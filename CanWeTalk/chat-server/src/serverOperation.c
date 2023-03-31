// FILE          : serverOperation.c
// PROJECT       : CanWeTalk
// programmer    : Euyoung Kim, Raj Dudhat, Yujin Jeong, Yujung Park
// FIRST VERSION : 2023-03-18
// DESCRIPTION   : This serverOperation.c file has the following functions for chat-server operation.
// - startServer
// - clientThread
// - collapseMasterList
// - broadcast


#include "../inc/chat-server.h"



// FUNCTION   : startServer()
// DESCRIPTION: This function operates server by initiating the server,
// and accepting clients, receiving message from customers,
// and broadcasting messages to connected clients until all clients are disconnected.
// PARAMETERS : NONE
// RETURN     : int retvalue
int startServer()
{
    int client_socket = 0;
    int queue_pending = MAX_CLIENTS;
    int client_length = 0;
    int counter = 0;
    int thread_index = 0;
    char logText[LOG_BUFFER_SIZE] = { 0 };
    struct sockaddr_in server_addr;
    struct sockaddr_in client_addr;
    pthread_t tid[MAX_CLIENTS];
    pthread_mutex_init(&mtx, NULL);


    // create socket
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket < 0)
    {
        perror("server socket creation : ");
        snprintf(logText, LOG_BUFFER_SIZE, "[SERVER ERROR-1] %s\n", strerror(errno));
        writeLogFile(SERVER, logText);
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
        perror("server socket binding : ");
        snprintf(logText, LOG_BUFFER_SIZE, "[SERVER ERROR-2] %s\n", strerror(errno));
        writeLogFile(SERVER, logText);
        close(server_socket);
        return -2;
    }

    // Listen socket
    if (listen(server_socket, queue_pending) < 0)
    {
        perror("server socket listening : ");
        snprintf(logText, LOG_BUFFER_SIZE, "[SERVER ERROR-3] %s\n", strerror(errno));
        writeLogFile(SERVER, logText);
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
        if (client_socket < 0)
        {
            perror("client accepting : ");
            snprintf(logText, LOG_BUFFER_SIZE, "[SERVER ERROR-4] %s\n", strerror(errno));
            writeLogFile(SERVER, logText);
            free(clientsMasterList);
            close(server_socket);
            return -4;
        }
        // maintain clientsMasterList
        pthread_mutex_lock(&mtx);
        clientsMasterList->clients[clientsMasterList->client_connections].socket = client_socket;
        strcpy(clientsMasterList->clients[clientsMasterList->client_connections].ipAddress, inet_ntoa(client_addr.sin_addr));
        clientsMasterList->clients[clientsMasterList->client_connections].port = ntohs(client_addr.sin_port);
        thread_index = clientsMasterList->client_connections;

        clientsMasterList->client_connections++;
        pthread_mutex_unlock(&mtx);

        // create thread to be responsible for incoming  message the user and broadcasting to all users
        if (pthread_create(&(tid[thread_index]), NULL, clientThread, (void*)&client_socket))
        {
            perror("thread creation for client : ");
            snprintf(logText, LOG_BUFFER_SIZE, "[SERVER ERROR-5] %s\n", strerror(errno));
            writeLogFile(SERVER, logText);
            free(clientsMasterList);
            close(server_socket);
            return -5;
        }
        // debug printf("thread creation for client %d\n", thread_index + 1);

        // release resources, detach does not requires the main thread join() with child thread. Server does not wait for each thread completion. so server handles the next client connection.
        pthread_detach(tid[thread_index]);
    }
    // free(clientsMasterList);
    // close(server_socket);
    return 1;
}


// FUNCTION   : clientThread()
// DESCRIPTION: This function receives client message and call broadcast function
// and maintains MasterList if client is disconnected
// PARAMETERS : void *socket
// RETURN     : nothing
void* clientThread(void* socket)
{
    int clientSocket = *((int*)socket);
    int numBytesRead = 0;
    int counter = 0;
    char msg[40];
    char quit[] = ">>bye<<";
    MESSAGE message_received;
    pthread_t receive_thread[MAX_CLIENTS];
    pthread_t broadcast_thread[MAX_CLIENTS];

    while (1)
    {
        // receive message from client and broadcast the message to clients
        numBytesRead = recv(clientSocket, &message_received, sizeof(MESSAGE), FLAG);
        if (numBytesRead == RET_ERROR || strcmp(message_received.chat, quit) == 0)
        {
            // collapse MasterList, remove the client
            pthread_mutex_lock(&mtx);
            collapseMasterList(clientSocket);
            close(clientSocket);
            clientsMasterList->client_connections--;
            if (clientsMasterList->client_connections == 0)
            {
                server_run = FALSE;
                kill(getpid(), SIGINT);
                //debug printf("serverrun=%d\n", FALSE);
            }
            // debug printf("client-socket closed:%d After collapse, client#:%d\n", clientSocket, clientsMasterList->client_connections);
            pthread_mutex_unlock(&mtx);
            pthread_exit(NULL);
            break;
        }
        else
        {
            broadcast(&message_received);
        }
    }//end while
}


// FUNCTION   : collapseMasterList()
// DESCRIPTION: This function updates MasterList if client is disconnected
// PARAMETERS : int clientSocket
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
                // debug printf("CollapseML index:%d socket:%d\n", counter, clientsMasterList->clients[counter].socket);
                // debug printf("CollapseML index:%d socket:%d\n", counter + 1, clientsMasterList->clients[counter + 1].socket);
            } // end while
            break;
        } // end if
    } // end for
}


// FUNCTION   : broadcast()
// DESCRIPTION: This function sends incoming message to all clients
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
    // debug
    printf("Broadcast ipAdd:%s id:%s message:%s\n", message->ipAddress, message->id, message->chat);
}


// FUNCTION   : serverSignalHandler()
// DESCRIPTION: This function handles SIGINT to terminate the left thread,
// and close the program gracefully.
// PARAMETERS : int signal_number
// RETURN     : Nothing
void serverSignalHandler(int signal_number)
{
    if (server_run == FALSE)
    {
        // log
        free(clientsMasterList);
        close(server_socket);
        pthread_mutex_destroy(&mtx);
        //debug printf("serverSignalHandler:server_run=%d\n", server_run);
        signal(SIGINT, serverAlarmHandler);
        exit(0);
    }
    // reactivate our custom signal handler for next time
    signal(signal_number, serverSignalHandler);
}


// FUNCTION   : serverAlarmHandler()
// DESCRIPTION: This function is to handle timeout condition.
// PARAMETERS : int signal_number
// RETURN     : Nothing
void serverAlarmHandler(int signal_number)
{
    alarm(5);	// reset alarm
    signal(signal_number, serverAlarmHandler);
}
