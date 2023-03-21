// FILE          : chat-server.h
// PROJECT       : CanWeTalk
// programmer    : Euyoung Kim, Raj Dudhat, Yujin Jeong, Yujung Park
// FIRST VERSION : 2023-03-18
// DESCRIPTION   :

#ifndef __chat_server_h__
#define __chat_server_h__

#include "../../Common/inc/common.h"

#include <signal.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <pthread.h>

#define MAX_CLIENTS 10

typedef struct client_info
{
  int socket; // file descriptor
  char ipAddress[ID_SIZE * 4];
  char userID[ID_SIZE];
} ClientInfo;

typedef struct master_list
{
  int client_connections;
	ClientInfo clients[MAX_CLIENTS];
} MasterList;

static int nClients = 0;
static int nNoConnections[MAX_CLIENTS];

pthread_mutex_t mtx;

// function prototype
int startServer();
void* clientThread(void *clientSocket);
void broadcast(MESSAGE *message);
void addMasterList(MasterList* clist, int clientSocket);

#endif
