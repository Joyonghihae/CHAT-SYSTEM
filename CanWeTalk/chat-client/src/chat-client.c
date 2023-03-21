// FILE          : chat-client.c
// PROJECT       : CanWeTalk
// programmer    : Euyoung Kim, Raj Dudhat, Yujin Jeong, Yujung Park
// FIRST VERSION : 2023-03-18
// DESCRIPTION   : This is an application that

#include "../inc/chat-client.h"

int main (int argc, char *argv[])
{
  int my_server_socket, len, done;
  struct sockaddr_in server_addr;
  MESSAGE client_message;
  struct hostent*    host;

  // char timestamp[BUFFER_SIZE] = { 0 };
  // time_t rawtime;
  // struct tm* timeinfo;
  // time(&rawtime);
  // timeinfo = localtime(&rawtime);
  //
  // sprintf(timestamp, "[%04d-%02d-%02d %02d:%02d:%02d]\n",
  // timeinfo->tm_year+1900, timeinfo->tm_mon, timeinfo->tm_mday, timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec);


  if (argc != 3)
  {
    printf ("USAGE : chat-client -user<userID> -server<server name>\n");
    return 1;
  }

  if ((host = gethostbyname(argv[2])) == NULL)
  {
    printf ("[CLIENT] : Host Info Search - FAILED\n");
    return 2;
  }

  // set user ID
  memcpy(&client_message.id, argv[1], ID_SIZE);
  printf("ID: %s\n", client_message.id);
  // initialize server address
  memset (&server_addr, 0, sizeof (server_addr));
  server_addr.sin_family = AF_INET;
  memcpy (&server_addr.sin_addr, host->h_addr, host->h_length); // copy the host's internal IP addr into the server_addr struct
  server_addr.sin_port = htons (PORT);

  printf ("[CLIENT] : Getting STREAM Socket to talk to SERVER\n");
  fflush(stdout);
  if ((my_server_socket = socket (AF_INET, SOCK_STREAM, 0)) < 0)
  {
    printf ("[CLIENT] : Getting Client Socket - FAILED\n");
    return 3;
  }

  printf ("[CLIENT] : Connecting to SERVER\n");
  fflush(stdout);
  if (connect (my_server_socket, (struct sockaddr *)&server_addr,sizeof (server_addr)) < 0)
  {
    printf ("[CLIENT] : Connect to Server - FAILED\n");
    close (my_server_socket);
    return 4;
  }
  done = TRUE;
  while(done == TRUE)
  {
     memset(buffer, 0, BUFFER_SIZE);
     fgets (buffer, sizeof (buffer), stdin);
     if (buffer[strlen (buffer) - 1] == '\n')
     {
       buffer[strlen (buffer) - 1] = '\0';
       memcpy(&client_message.chat, buffer, sizeof(buffer));
     }
     if(strcmp(buffer,"quit") == 0)
     {
       send(my_server_socket, (void *)&client_message, sizeof(client_message), FLAG);
       done = 0;
     }
     else
     {
       send(my_server_socket, (void *)&client_message, sizeof(client_message), FLAG);
       len = recv(my_server_socket, (void *)&client_message, sizeof(client_message), FLAG);

     }
  }

  close (my_server_socket);
  printf ("[CLIENT] : I'm outta here !\n");
  return 0;
}
