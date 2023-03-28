// FILE          : clientOperation.c
// PROJECT       : CanWeTalk
// programmer    : Euyoung Kim, Raj Dudhat, Yujin Jeong, Yujung Park
// FIRST VERSION : 2023-03-18
// DESCRIPTION   : This clientOperation.c file is to

#include "../inc/chat-client.h"

int startClient(struct hostent* host)
{
    int sckt = 0;
    int received_length = 0;
    size_t lineOverForty = 0;
    int client_run = FALSE;
    struct sockaddr_in server_addr;

    int chat_line = 0;
    int shouldBlank = 0;


    char buffer[BUFFER_SIZE] = { 0 };
    pthread_t outgoing;
    pthread_t incoming;

    // ncurses
    WINDOW* chat_title_win;
    WINDOW* chat_win;
    WINDOW* msg_title_win;
    WINDOW* msg_win;

    //LINES, COLS deindesd in <ncurses.h>, filled in by initscr with the size of the screen
    int chat_startx, chat_starty, chat_width, chat_height;
    int msg_startx, msg_starty, msg_width, msg_height;

    // initialize server address
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    memcpy(&server_addr.sin_addr, host->h_addr, host->h_length);
    server_addr.sin_port = htons(PORT);

    // get client socket
    if ((sckt = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        // printf ("[CLIENT ERROR] Getting Client Socket\n");
        return 3;
    }

    // connect to server
    if (connect(sckt, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0)
    {
        printf("[CLIENT ERROR] Connect to Server\n");
        perror("binding error");
        exit(EXIT_FAILURE);
        close(sckt);
        // return 4;
    }

    // if connected to server, then ncurses start
    initscr();  // initialize the ncurses data structure
    cbreak();   // set the input mode for the terminal
    noecho();   // do not echo() while do getch
    refresh();  // copy the window to the physical terminal screen
    msg_height = 13;
    msg_width = 80;
    msg_starty = 0;
    msg_startx = 0;
    chat_height = 5;
    chat_width = COLS - 2;
    chat_starty = LINES - chat_height;
    chat_startx = 1;
    // create ncurses windows after server connection
    msg_win = create_newwin(msg_height, msg_width, msg_starty, msg_startx, 'm');
    scrollok(msg_win, TRUE); // enable scrollig
    chat_win = create_newwin(chat_height, chat_width, chat_starty, chat_startx, 'c');
    scrollok(chat_win, TRUE);


    client_run = TRUE;
    while (client_run == TRUE)
    {
        // get user input by 80 chars,
        memset(buffer, 0, BUFFER_SIZE - 1);
        input_win(chat_win, buffer);
        memcpy(client_message->chat, buffer, sizeof(buffer));
        if (strcmp(buffer, ">>bye<<") == 0)
        {
            client_run = FALSE;
        }
        if (pthread_create(&outgoing, NULL, sendMessage, (void*)&sckt))
        {
            return -5;
        }
        // receive message
        if (pthread_create(&incoming, NULL, receiveMessage, (void*)&sckt))
        {
            return -5;
        }
        time_t rawtime;
        struct tm* timeinfo;
        char timestamp[11] = { 0 };
        time(&rawtime);
        timeinfo = localtime(&rawtime);
        sprintf(timestamp, "(%02d:%02d:%02d)", timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec);
        // display message
        display_win(msg_win, timestamp, chat_line, shouldBlank, client_message);
        //display_win(msg_win, msg->chat, 0, shouldBlank);
        chat_line++;
        pthread_detach(outgoing);
        pthread_detach(incoming);
    }



    sleep(1);
    destroy_win(chat_win);
    destroy_win(msg_win);
    endwin();

    close(sckt);
    return 1;
}

void* sendMessage(void* sock)
{
    int clientSocket = *((int*)sock);
    pthread_mutex_lock(&mtx);
    send(clientSocket, (void*)client_message, sizeof(MESSAGE), FLAG);
    pthread_mutex_unlock(&mtx);
    pthread_exit(NULL);
}

void* receiveMessage(void* sock)
{
    int clientSocket = *((int*)sock);
    pthread_mutex_lock(&mtx);
    recv(clientSocket, (void*)client_message, sizeof(MESSAGE), FLAG);
    pthread_mutex_unlock(&mtx);
    pthread_exit(NULL);
}

WINDOW* create_newwin(int height, int width, int starty, int startx, char type)
{
    WINDOW* local_win;

    if (type == 'c')
    {
        local_win = newwin(height, width, starty, startx);
        box(local_win, 0, 0);     // draw box
        mvwprintw(local_win, 1, 3, "> ");
        //wmove(local_win, 1, 1);   // position cursor at top
        wrefresh(local_win);
    }
    else
    {
        local_win = newwin(height, width, starty, startx);
        box(local_win, 0, 0);     // draw box
        wmove(local_win, 1, 1);   // position cursor at top
        wrefresh(local_win);
    }
    return local_win;
}

/* This function is for taking input chars from the user */
void input_win(WINDOW* win, char* word)
{
    int i, ch;
    int maxrow, maxcol, row = 1, col = 0;

    blankWin(win);                  // make it a clean window
    getmaxyx(win, maxrow, maxcol);  // get window size
    bzero(word, BUFFER_SIZE);
    wmove(win, 1, 1);
    wprintw(win, "> ");

    for (i = 0; (ch = wgetch(win)) != '\n'; i++)
    {
        word[i] = ch;                       /* '\n' not copied */
        if (col++ < maxcol - 2)               /* if within window */
        {
            wprintw(win, "%c", word[i]);      /* display the char recv'd */
        }
        else                                /* last char pos reached */
        {
            col = 1;
            if (row == maxrow - 2)              /* last line in the window */
            {
                scroll(win);                    /* go up one line */
                row = maxrow - 2;                 /* stay at the last line */
                wmove(win, row, col);           /* move cursor to the beginning */
                wclrtoeol(win);                 /* clear from cursor to eol */
                box(win, 0, 0);                 /* draw the box again */
            }
            else
            {
                row++;
                wmove(win, row, col);           /* move cursor to the beginning */
                wrefresh(win);
                wprintw(win, "%c", word[i]);    /* display the char recv'd */
            }
        }
    }
}  /* input_win */


void display_win(WINDOW* win, char* tstmp, int whichRow, int shouldBlank, MESSAGE* msg)
{
    if (shouldBlank == 1) blankWin(win); // make it a clean window
    wmove(win, (whichRow + 1), 1);      // position cusor at approp row

    if (strcmp(msg->id, user) == 0)
    {
        wprintw(win, "%-16s[%-5s] >> %-40s %10s", msg->ipAddress, msg->id, msg->chat, tstmp);
    }
    else
    {
        wprintw(win, "%-16s[%-5s] << %-40s %10s", msg->ipAddress, msg->id, msg->chat, tstmp);
    }

    wrefresh(win);
} /* display_win */

void destroy_win(WINDOW* win)
{
    delwin(win);
}  /* destory_win */

void blankWin(WINDOW* win)
{
    int i;
    int maxrow, maxcol;

    getmaxyx(win, maxrow, maxcol);
    for (i = 1; i < maxcol - 2; i++)
    {
        wmove(win, i, 1);
        refresh();
        wclrtoeol(win);
        wrefresh(win);
    }
    box(win, 0, 0);             /* draw the box again */
    wrefresh(win);
}  /* blankWin */
