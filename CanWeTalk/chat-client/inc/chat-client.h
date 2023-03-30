// FILE          : chat-client.h
// PROJECT       : CanWeTalk
// programmer    : Eunyoung Kim, Raj Dudhat, Yujin Jeong, Yujung Park
// FIRST VERSION : 2023-03-18
// DESCRIPTION   : This is a header file for chat-client

#ifndef __chat_client_h__
#define __chat_client_h__

#include "../../Common/inc/common.h"
#include <signal.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <stdarg.h>
#include <ncurses.h>

#define TIMESTAMP_SIZE 11
#define MAX_CHAR 40

char user[ID_SIZE + 1];
MESSAGE* client_message;
pthread_mutex_t mtx;
pthread_mutex_t mtx_ncs;

static int theSocket;

void* receiveMessage(void* win);
void* sendMessage(void* sock);

WINDOW* create_newwin(int, int, int, int, char);
void destroy_win(WINDOW*);
void input_win(WINDOW*, char*);
void display_win(WINDOW*, char*, int, int, MESSAGE*, char*);
void destroy_win(WINDOW* win);
void blankWin(WINDOW* win);

int startClient(struct hostent* host);

#endif
