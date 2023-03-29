// FILE          : common.c
// PROJECT       : CanWeTalk
// programmer    : Eunyoung Kim, Raj Dudhat, Yujin Jeong, Yujung Park
// FIRST VERSION : 2023-03-18
// DESCRIPTION   : This is an internet server application that will respond
// to requests on port 5000

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include <time.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>


#define PORT 1024
#define BUFFER_SIZE 80
#define LOG_BUFFER_SIZE 256
#define ID_SIZE 5
#define IP_SIZE 16
#define TRUE 1
#define FALSE 0
#define FLAG 0
#define SERVER 's'

//
typedef struct message
{
	char ipAddress[IP_SIZE];
	char id[ID_SIZE + 1];
	char chat[BUFFER_SIZE];
} MESSAGE;

int writeLogFile(char appType, char* logText);
