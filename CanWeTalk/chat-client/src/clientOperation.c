// FILE          : clientOperation.c
// PROJECT       : CanWeTalk
// programmer    : Euyoung Kim, Raj Dudhat, Yujin Jeong, Yujung Park
// FIRST VERSION : 2023-03-18
// DESCRIPTION   : This clientOperation.c file is to 

#include "../inc/chat-client.h"

int startClient(struct hostent* host, char* id, WINDOW* chat_win, WINDOW* msg_win)
{
    int my_server_socket, len, done;
    struct sockaddr_in server_addr;
    MESSAGE client_message;

    int i;
    int shouldBlank = 0;
    char buf[BUFSIZ];

    char buffer[BUFFER_SIZE];
    pthread_t outgoing_window, incoming_window;

    // set user ID
    memcpy(&client_message.id, id, ID_SIZE);

    // initialize server address
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    memcpy(&server_addr.sin_addr, host->h_addr, host->h_length);
    server_addr.sin_port = htons(PORT);

    // get client socket
    if ((my_server_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        // printf ("[CLIENT] : Getting Client Socket - FAILED\n");
        return 3;
    }

    // connect to server
    if (connect(my_server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0)
    {
        // printf ("[CLIENT] : Connect to Server - FAILED\n");
        close(my_server_socket);
        return 4;
    }
    done = TRUE;
    while (done == TRUE)
    {
        memset(buffer, 0, BUFFER_SIZE);

        input_win(chat_win, buffer);
        display_win(msg_win, buffer, i, shouldBlank);

        // fgets (buffer, sizeof (buffer), stdin);
        if (buffer[strlen(buffer) - 1] == '\n')
        {
            buffer[strlen(buffer) - 1] = '\0';
            memcpy(&client_message.chat, buffer, sizeof(buffer));
        }
        else
        {
            memcpy(&client_message.chat, buffer, sizeof(buffer));
        }

        if (strcmp(buffer, ">>bye<<") == 0)
        {
            send(my_server_socket, (void*)&client_message, sizeof(client_message), FLAG);
            done = 0;
        }
        else
        {
            send(my_server_socket, (void*)&client_message, sizeof(client_message), FLAG);
            len = recv(my_server_socket, (void*)&client_message, sizeof(client_message), FLAG);

            display_win(msg_win, buf, i, shouldBlank);
        }
    }

    sleep(2);                   /* to get a delay */
    destroy_win(chat_win);
    destroy_win(msg_win);
    endwin();

    close(my_server_socket);
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
