// FILE          : clientOperation.c
// PROJECT       : CanWeTalk
// programmer    : Euyoung Kim, Raj Dudhat, Yujin Jeong, Yujung Park
// FIRST VERSION : 2023-03-18
// DESCRIPTION   : This clientOperation.c file is to

#include "../inc/chat-client.h"

int startClient(struct hostent* host, MESSAGE* msg)
{
    int sckt = 0;
    int received_length = 0;
    size_t input_length = 0;
    int client_run = FALSE;
    struct sockaddr_in server_addr;

    int chat_line = 0;
    int shouldBlank = 0;
    char buf[BUFSIZ];

    char buffer[BUFFER_SIZE];
    pthread_t outgoing_window;
    pthread_t incoming_window;

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

    chat_height = 5;
    chat_width = COLS - 2;
    chat_startx = 1;
    chat_starty = LINES - chat_height;

    msg_height = 10;
    msg_width = 80;
    msg_startx = 0;
    msg_starty = 0;


    // create ncurses windows
    msg_win = create_newwin(msg_height, msg_width, msg_starty, msg_startx);
    scrollok(msg_win, TRUE); // enable scrollig
    chat_win = create_newwin(chat_height, chat_width, chat_starty, chat_startx);
    scrollok(chat_win, TRUE);

    client_run = TRUE;
    while (client_run == TRUE)
    {
        memset(buffer, 0, BUFFER_SIZE);
        input_win(chat_win, buffer);
        chat_line++;
        // 10 messages before scroll
        display_win(msg_win, msg->chat, chat_line, shouldBlank);
        input_length = strlen(buffer);
        if (buffer[strlen(buffer) - 1] == '\n')
        {
            buffer[strlen(buffer) - 1] = '\0';
            memcpy(msg->chat, buffer, sizeof(buffer));
        }
        else
        {
            memcpy(msg->chat, buffer, sizeof(buffer));
        }

        //THREAD TO HANDLE OUTGOIN AND THREAD TO HANDLE THE INCOMMING
        if (strcmp(buffer, ">>bye<<") == 0)
        {
            send(sckt, (void*)msg, sizeof(MESSAGE), FLAG);
            client_run = 0;
        }
        else
        {
            send(sckt, (void*)msg, sizeof(MESSAGE), FLAG);
            received_length = recv(sckt, (void*)msg, sizeof(MESSAGE), FLAG);

            display_win(msg_win, msg->chat, 0, shouldBlank);
        }
    }

    sleep(2);                   /* to get a delay */
    destroy_win(chat_win);
    destroy_win(msg_win);
    endwin();

    close(sckt);
    return 1;
}






WINDOW* create_newwin(int height, int width, int starty, int startx)
{
    WINDOW* local_win;

    local_win = newwin(height, width, starty, startx);
    box(local_win, 0, 0);               /* draw a box */

    wmove(local_win, 1, 1);             /* position cursor at top */
    wrefresh(local_win);
    return local_win;
}

/* This function is for taking input chars from the user */
void input_win(WINDOW* win, char* word)
{
    int i, ch;
    int maxrow, maxcol, row = 1, col = 0;

    blankWin(win);                          /* make it a clean window */
    getmaxyx(win, maxrow, maxcol);          /* get window size */
    bzero(word, BUFSIZ);
    wmove(win, 1, 1);                       /* position cusor at top */
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


void display_win(WINDOW* win, char* word, int whichRow, int shouldBlank)
{
    if (shouldBlank == 1) blankWin(win);                /* make it a clean window */
    wmove(win, (whichRow + 1), 1);                       /* position cusor at approp row */
    wprintw(win, word);
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
