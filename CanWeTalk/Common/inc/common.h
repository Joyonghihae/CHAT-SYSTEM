// FILE          : common.h
// PROJECT       : CanWeTalk
// programmer    : Eunyoung Kim, Raj Dudhat, Yujin Jeong, Yujung Park
// FIRST VERSION : 2023-03-18
// DESCRIPTION   : This is a common header file for CanWeTalk system

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <net/if.h>
#include <netdb.h>
#include <pthread.h>


#define PORT 1024
#define BUFFER_SIZE 81
#define LOG_BUFFER_SIZE 256
#define ID_SIZE 5
#define IP_SIZE 16
#define TRUE 1
#define FALSE 0
#define FLAG 0
#define RET_ERROR -1
#define SERVER 's'
#define CLIENT 'c'


typedef struct message
{
	char ipAddress[IP_SIZE];
	char id[ID_SIZE + 1];
	char chat[BUFFER_SIZE];
} MESSAGE;

int writeLogFile(char appType, char* logText);
