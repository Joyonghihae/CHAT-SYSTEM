

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>


#define PORT 5000
#define BUFFER_SIZE 80
#define ID_SIZE 5
#define TRUE 1
#define FLAG 0

//
typedef struct message
{
  char id[ID_SIZE];
	char chat[BUFFER_SIZE];
} MESSAGE;
