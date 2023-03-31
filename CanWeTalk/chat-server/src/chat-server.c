// FILE          : chat-server.c
// PROJECT       : CanWeTalk
// programmer    : Euyoung Kim, Raj Dudhat, Yujin Jeong, Yujung Park
// FIRST VERSION : 2023-03-18
// DESCRIPTION   : This is chat-server.c file for chat-server application

#include "../inc/chat-server.h"


int main(void)
{
	// register the signal handlers
	signal(SIGINT, serverSignalHandler);
	signal(SIGALRM, serverAlarmHandler);
	alarm(5);

	// start server
	startServer();

	return 0;
}
