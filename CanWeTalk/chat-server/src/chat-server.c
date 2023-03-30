// FILE          : chat-server.c
// PROJECT       : CanWeTalk
// programmer    : Euyoung Kim, Raj Dudhat, Yujin Jeong, Yujung Park
// FIRST VERSION : 2023-03-18
// DESCRIPTION   : This is chat-server.c file for chat-server application

#include "../inc/chat-server.h"


int main(void)
{
	int retValue = 0;
	// initiate mutex
	pthread_mutex_init(&mtx, NULL);
	// start server
	retValue = startServer();

	// Destroy mutex
	pthread_mutex_destroy(&mtx);

	return 0;
}
