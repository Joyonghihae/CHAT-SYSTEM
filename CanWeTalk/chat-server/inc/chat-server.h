// FILE          : chat-server.h
// PROJECT       : CanWeTalk
// programmer    : Euyoung Kim, Raj Dudhat, Yujin Jeong, Yujung Park
// FIRST VERSION : 2023-03-18
// DESCRIPTION   : This is a header file for chat-server

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
	int port;
	char ipAddress[ID_SIZE * 4];
	char userID[ID_SIZE];
	// char message[BUFFER_SIZE];
} ClientInfo;

typedef struct master_list
{
	int client_connections;
	ClientInfo clients[MAX_CLIENTS];
} MasterList;

MasterList* clientsMasterList;
pthread_mutex_t mtx;
static int server_run = TRUE;
static int server_socket = 0;

// function prototype
void serverSignalHandler(int signal_number);
void serverAlarmHandler(int signal_number);
int startServer();
void* clientThread(void* socket);
void collapseMasterList(int clientSocket);
void broadcast(MESSAGE* message);

#endif
