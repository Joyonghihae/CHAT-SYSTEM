// FILE          : chat-client.c
// PROJECT       : CanWeTalk
// programmer    : Euyoung Kim, Raj Dudhat, Yujin Jeong, Yujung Park
// FIRST VERSION : 2023-03-18
// DESCRIPTION   : This is an internet server application that will respond
// to requests on port 5000

#include "../inc/chat-server.h"


int main (void)
{
  // initiate mutex
  pthread_mutex_init(&mtx, NULL);
  // start server
  startServer();
  // Destroy mutex
  pthread_mutex_destroy(&mtx);

  return 0;
}
