// FILE          : chat-client.c
// PROJECT       : CanWeTalk
// programmer    : Euyoung Kim, Raj Dudhat, Yujin Jeong, Yujung Park
// FIRST VERSION : 2023-03-18
// DESCRIPTION   : This is a chat-client.c file for chat-client application of CanWeTalk System.
// chat-client application uses ncurese UI and receives user message input,
// and send the message to the server.

#include "../inc/chat-client.h"

int main(int argc, char* argv[])
{
    struct hostent* host;
    char user[ID_SIZE];

    //ncurses
    WINDOW* chat_title_win;
    WINDOW* chat_win;
    WINDOW* msg_title_win;
    WINDOW* msg_win;
    int chat_startx, chat_starty, chat_width, chat_height;
    int msg_startx, msg_starty, msg_width, msg_height, i;



    char timestamp[BUFFER_SIZE] = { 0 };
    time_t rawtime;
    struct tm* timeinfo;
    time(&rawtime);
    timeinfo = localtime(&rawtime);
    sprintf(timestamp, "(%02d:%02d:%02d)\n", timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec);

    if (argc != 3)
    {
        printf("USAGE : chat-client -user<userID> -server<server name>\n");
        return 1;
    }
    if ((host = gethostbyname(argv[2])) == NULL)
    {
        printf("[CLIENT] : Host Info Search - FAILED\n");
        return 2;
    }

    // SANITY CHECK FOR USERID IPADDRESS
    // id no more than 5, ipAddress
    strcpy(user, argv[1]);





    initscr();  // initialize the ncurses data structure
    cbreak();   // set the input mode for the terminal
    noecho();   // control whether characters typed by the user
    refresh();  // copy the window to the physical terminal screen

    chat_height = 5;
    chat_width = COLS - 2;
    chat_startx = 1;
    chat_starty = LINES - chat_height;
    msg_height = LINES - chat_height - 1;
    msg_width = COLS;
    msg_startx = 0;
    msg_starty = 0;

    // create ncurses windows
    msg_win = create_newwin(msg_height, msg_width, msg_starty, msg_startx);
    scrollok(msg_win, TRUE);
    chat_win = create_newwin(chat_height, chat_width, chat_starty, chat_startx);
    scrollok(chat_win, TRUE);

    startClient(host, user, chat_win, msg_win);

    return 0;
}
