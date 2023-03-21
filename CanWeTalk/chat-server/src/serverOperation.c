// FILE          : chat-client.c
// PROJECT       : CanWeTalk
// programmer    : Eunyoung Kim, Raj Dudhat, Yujin Jeong, Yujung Park
// FIRST VERSION : 2023-03-18
// DESCRIPTION   : This is an internet server application that will respond
// to requests on port 5000

#include "../inc/chat-server.h"
#include <pthread.h>

// FUNCTION   : startServer()
// DESCRIPTION: This function initiate the server and accept clients
// PARAMETERS : NONE
// RETURN     : Nothing
int startServer()
{
  int server_socket = 0;
  int client_socket;
  int queue_pending = MAX_CLIENTS;
  int client_length = 0;
  int whichClient = 0;
  int counter = 0;
  struct sockaddr_in server_addr;
  struct sockaddr_in client_addr;
  pthread_t tid[MAX_CLIENTS];
  MasterList* clientsMasterList;

  // create socket
  server_socket = socket (AF_INET, SOCK_STREAM, 0);
  if (server_socket < 0)
  {
    printf ("[SERVER ERROR] server socket creation\n");
    return 1;
  }

  // initiate the server address info
  memset (&server_addr, 0, sizeof (server_addr));
  server_addr.sin_family = AF_INET;
  server_addr.sin_addr.s_addr = htonl (INADDR_ANY);
  server_addr.sin_port = htons (PORT);

  // bind the socket and the server address
  if(bind(server_socket, (struct sockaddr *)&server_addr, sizeof (server_addr)) < 0)
  {
    printf ("[SERVER ERROR] Binding of server socket\n");
    close (server_socket);
    return 2;
  }

  // Listen socket
  if(listen(server_socket, queue_pending) < 0)
  {
    printf("[SERVER ERROR] Listen on Socket\n");
    close(server_socket);
    return 3;
  }

  // allocate memory and set vaules
  clientsMasterList = (MasterList*)malloc(sizeof(MasterList));
  clientsMasterList->client_connections = 0;
  for(counter=0; counter<MAX_CLIENTS; counter++)
  {
    // use 0 (stdin file descriptor) for default value
    clientsMasterList->clients[counter].socket = 0;
  }

  // Accept client
  while(nClients <= MAX_CLIENTS)
  {
    // accept a packet from client
    client_length = sizeof(client_addr);
    client_socket = accept(server_socket,(struct sockaddr *)&client_addr, &client_length);
    if(client_socket < 0)
    {
      printf("[SERVER ERROR] Accept Packet from Client\n");
      close(server_socket);
      return 4;
    }
    // get IP
    getpeername(client_socket, (struct sockaddr *)&client_addr, &client_length);

    // maintain clientsMasterList
    pthread_mutex_lock(&mtx);
    nNoConnections[nClients] = client_socket;
    nClients++;

    clientsMasterList->clients[clientsMasterList->client_connections].socket = client_socket;
    clientsMasterList->clients[clientsMasterList->client_connections].ipAddress = inet_ntoa(client_addr.sin_addr);
    clientsMasterList->client_connections ++;

    pthread_mutex_unlock(&mtx);



    //create thread to be responsible for incoming  message the user and broadcasting to all users
    if (pthread_create(&(tid[(nClients-1)]), NULL, clientThread, (void *)&client_socket))
  	{
  	  printf ("[SERVER ERROR] pthread_create()\n");
  	  return 5;
  	}

    //
    for(counter = 0; counter <= MAX_CLIENTS; counter++)
    {
      int joinStatus = pthread_join(tid[counter], (void *)(&whichClient));
      if(joinStatus == 0)
      {
        printf("\n[SERVER] : received QUIT command from CLIENT-%02d (joinStatus=%d)\n", whichClient, joinStatus);
        break;
      }
    }
  }

  close(server_socket);
  return 0;
}

// FUNCTION   : maintainDCInfo()
// DESCRIPTION: This function updates struct DCInfo of struct MasterList
// for given index with message queue information and current time
// PARAMETERS : msgDC* msg
//              MasterLIst* mlist
//              int index
// RETURN     : Nothing
void addMasterList(MasterList* clist, int clientSocket)
{
}

//
//
//
//
void* clientThread(void *socket)
{
  int clientSocket = *((int*)socket);
  int numBytesRead = 0;
  int counter = 0;
  int iAmClient = 0;
  char msg[40];
  MESSAGE message_received;

  iAmClient = nClients;

  // receive message from client
  // numBytesRead = recv(clientSocket, &message_received, sizeof(message_received), FLAG);
  while((numBytesRead= recv(clientSocket, &message_received, sizeof(message_received), FLAG)) > ID_SIZE)
  {
    printf("%s\n", message_received.chat);

    // if message is >>bye<<, do not braodcast, break the loop
    if (strcmp(message_received.chat, "quit") == 0)
    {
      break;
    }
    // Broadcast message
    broadcast(&message_received);
  }

  // end the client thread,
  // collapse
  pthread_mutex_lock(&mtx);
  for(counter = 0; counter < nClients; counter++)
  {
    if(clientSocket == nNoConnections[counter])
    {
      while(counter++ < nClients-1)
      {
        nNoConnections[counter] = nNoConnections[counter+1];
      }
      break;
    }
  }

  close(clientSocket);
  nClients--;
  pthread_mutex_unlock(&mtx);
  pthread_exit((void *)iAmClient);


  return NULL;
}

//
//
//
//
void broadcast(MESSAGE* message)
{
  int counter = 0;
  printf("Broadcast message: %s\n", message->chat);
}
