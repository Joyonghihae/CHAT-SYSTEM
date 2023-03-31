// FILE          : clientOperation.c
// PROJECT       : CanWeTalk
// programmer    : Euyoung Kim, Raj Dudhat, Yujin Jeong, Yujung Park
// FIRST VERSION : 2023-03-18
// DESCRIPTION   : This clientOperation.c file has the following functions for chat-client operation.
// - startClient
// - sendMessage
// - receiveMessage
// - create_newwin
// - input_win
// - display_win
// - destroy_win
// - blankWin

#include "../inc/chat-client.h"

// FUNCTION   : startClient()
// DESCRIPTION: This function operates client-chat by initiating the client socket,
// and connect to the server, and receive and send the server client message.
// PARAMETERS : struct hostent* host
// RETURN     : int retvalue
int startClient(struct hostent* host)
{
    char logText[LOG_BUFFER_SIZE] = { 0 };
    int received_length = 0;
    struct sockaddr_in server_addr;
    struct ifreq ifr;
    pthread_t outgoing;
    pthread_t incoming;

    // ncurses windows
    WINDOW* chat_title_win;
    WINDOW* chat_win;
    WINDOW* msg_title_win;
    WINDOW* msg_win;

    // LINES, COLS deindesd in <ncurses.h>, filled in by initscr with the size of the screen
    int chat_startx, chat_starty, chat_width, chat_height;
    int msg_startx, msg_starty, msg_width, msg_height;
    int msg_title_height, msg_title_width, msg_title_startx, msg_title_starty;
    int chat_title_height, chat_title_width, chat_title_startx, chat_title_starty;

    // initialize server address
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    memcpy(&server_addr.sin_addr, host->h_addr, host->h_length);
    server_addr.sin_port = htons(PORT);

    // get client socket
    if ((theSocket = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror("client socket creation : ");
        snprintf(logText, LOG_BUFFER_SIZE, "[CLIENT ERROR-3] %s\n", strerror(errno));
        writeLogFile(CLIENT, logText);
        return -3;
    }

    // get client ip with interface request
    ifr.ifr_addr.sa_family = AF_INET;
    strncpy(ifr.ifr_name, "ens33", IFNAMSIZ - 1);
    ioctl(theSocket, SIOCGIFADDR, &ifr);
    strcpy(client_message->ipAddress, inet_ntoa(((struct sockaddr_in*)&ifr.ifr_addr)->sin_addr));

    // connect to server
    if (connect(theSocket, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0)
    {
        perror("connect to server : ");
        snprintf(logText, LOG_BUFFER_SIZE, "[CLIENT ERROR-4] %s\n", strerror(errno));
        writeLogFile(CLIENT, logText);
        close(theSocket);
        return -4;
    }

    // if connected to server, then ncurses start
    initscr();    // initialize the ncurses data structure
    cbreak();     // set the input mode for the terminal
    noecho();     // do not echo() while do getch
    clear();
    refresh();    // copy the window to the physical terminal screen
    start_color();
    init_pair(1, COLOR_BLACK, COLOR_WHITE);

    msg_title_height = 3;
    msg_title_width = COLS;
    msg_title_starty = 0;
    msg_title_startx = 0;

    msg_height = 10;
    msg_width = COLS;
    msg_starty = 3;
    msg_startx = 0;

    chat_title_height = 3;
    chat_title_width = COLS;
    chat_title_starty = 16;
    chat_title_startx = 0;

    chat_height = 2;
    chat_width = COLS;
    chat_starty = 19;
    chat_startx = 0;

    // create ncurses windows after server connection
    msg_title_win = create_newwin(msg_title_height, msg_title_width, msg_title_starty, msg_title_startx, 's');
    msg_win = create_newwin(msg_height, msg_width, msg_starty, msg_startx, 'm');
    scrollok(msg_win, TRUE); // enable scrollig
    chat_title_win = create_newwin(chat_title_height, chat_title_width, chat_title_starty, chat_title_startx, 'h');
    chat_win = create_newwin(chat_height, chat_width, chat_starty, chat_startx, 'c');


    if (pthread_create(&outgoing, NULL, sendMessage, (void*)chat_win))
    {
        perror("thread creation : ");
        snprintf(logText, LOG_BUFFER_SIZE, "[CLIENT ERROR-5] %s\n", strerror(errno));
        writeLogFile(CLIENT, logText);

        destroy_win(chat_win);
        destroy_win(msg_win);
        endwin();
        close(theSocket);
        return -5;
    }
    // pthread_detach(outgoing);
    if (pthread_create(&incoming, NULL, receiveMessage, (void*)msg_win))
    {
        perror("thread creation : ");
        snprintf(logText, LOG_BUFFER_SIZE, "[CLIENT ERROR-5] %s\n", strerror(errno));
        writeLogFile(CLIENT, logText);

        destroy_win(chat_win);
        destroy_win(msg_win);
        endwin();
        close(theSocket);
        return -5;
    }
    // pthread_detach(incoming);
    pthread_join(outgoing, NULL);
    pthread_join(incoming, NULL);
    sleep(1);

    close(theSocket);
    destroy_win(chat_win);
    destroy_win(msg_win);
    endwin();

    return 1;
}


// FUNCTION   : sendMessage()
// DESCRIPTION: This function completes the message struct with user input which is entered through input window,
// and send client_message struct to server
// PARAMETERS : void* win
// RETURN     : nothing
void* sendMessage(void* win)
{
    pthread_mutex_lock(&mtx_ncs);
    int sock = theSocket;
    char buffer[BUFFER_SIZE] = { 0 };
    char quit[] = ">>bye<<";
    int run = TRUE;

    while (run == TRUE)
    {
        memset(buffer, 0, BUFFER_SIZE - 1);
        input_win(win, buffer);

        memcpy(client_message->chat, buffer, sizeof(buffer));
        if (strcmp(buffer, quit) == 0)
        {
            run = FALSE;
        }
        send(sock, (void*)client_message, sizeof(MESSAGE), FLAG);
    }

    pthread_mutex_unlock(&mtx_ncs);
    pthread_exit(NULL);
}


// FUNCTION   : receiveMessage()
// DESCRIPTION: This function receives server's broadcast message of struct,
//              call display_win function to display the received message
// PARAMETERS : void* win
// RETURN     : nothing
void* receiveMessage(void* win)
{
    int sock = 0;
    int run = TRUE;
    int ret = 1;
    int chat_line = 0;
    int shouldBlank = 0;
    int tokenLocation = 0;
    char timestamp[TIMESTAMP_SIZE] = { 0 };
    char buf[MAX_CHAR + 1];
    char* tokenPointer = NULL;
    struct tm* timeinfo;
    size_t chat_length = 0;
    size_t lineOverForty = 0;
    time_t rawtime;
    MESSAGE* message_received = (MESSAGE*)malloc(sizeof(MESSAGE));

    pthread_mutex_lock(&mtx);
    sock = theSocket;
    pthread_mutex_unlock(&mtx);

    while (run == TRUE)
    {
        ret = recv(sock, (void*)message_received, sizeof(MESSAGE), FLAG);
        chat_length = strlen(message_received->chat);

        if (ret == 0)
        {
            run = FALSE;
        }
        else
        {
            time(&rawtime);
            timeinfo = localtime(&rawtime);
            sprintf(timestamp, "(%02d:%02d:%02d)", timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec);

            // display max 40 chars by word for each line
            lineOverForty = chat_length / (MAX_CHAR + 1);
            if (lineOverForty == 0)
            {
                memcpy(buf, message_received->chat, MAX_CHAR + 1);
                display_win(win, timestamp, chat_line, shouldBlank, message_received, buf);
                chat_line++;
            }
            else
            {
                tokenLocation = 0;
                while (tokenLocation < chat_length)
                {
                    memset(buf, '\0', MAX_CHAR);
                    memcpy(buf, message_received->chat + tokenLocation, MAX_CHAR);
                    buf[MAX_CHAR] = '\0';
                    tokenPointer = strrchr(buf, ' ');
                    // not found then null
                    if (tokenPointer == NULL || (chat_length - tokenLocation) < MAX_CHAR)
                    {
                        tokenLocation += MAX_CHAR;
                    }
                    else
                    {
                        tokenLocation += (tokenPointer - buf + 1);
                        memset(tokenPointer, '\0', 1);
                    }
                    // display message
                    display_win(win, timestamp, chat_line, shouldBlank, message_received, buf);
                    chat_line++;
                } // end while
            } // end else
        } // end else
    } // end whlie

    free(message_received);
    pthread_mutex_unlock(&mtx);
    pthread_exit(NULL);
}


// FUNCTION   : create_newwin()
// DESCRIPTION: This function creates ncurses windows according to the type
// PARAMETERS : int height
//              int width
//              int starty
//              int startx
//              char type
// RETURN     : WINDOW* local_win
WINDOW* create_newwin(int height, int width, int starty, int startx, char type)
{
    WINDOW* local_win;
    char* outgoing_title = "Outgoing Message";
    char* msg_title = "CanWeTalk Message";

    if (type == 's')
    {
        local_win = newwin(height, width, starty, startx);
        wbkgd(local_win, COLOR_PAIR(1));
        wrefresh(local_win);
        mvwprintw(local_win, height / 2, (width - strlen(msg_title)) / 2, msg_title);
        wrefresh(local_win);
        curs_set(FALSE);
    } //msg title
    else if (type == 'c')
    {
        local_win = newwin(height, width, starty, startx);
        wmove(local_win, 0, 0);         // move the cursor
        mvwprintw(local_win, 1, 3, "> ");
        wrefresh(local_win);
    } // input window
    else if (type == 'h')
    {
        local_win = newwin(height, width, starty, startx);
        wbkgd(local_win, COLOR_PAIR(1));
        wrefresh(local_win);
        mvwprintw(local_win, height / 2, (width - strlen(outgoing_title)) / 2, outgoing_title);
        wrefresh(local_win);
        curs_set(FALSE);
        //leaveok(local_win, true);
    } // input title
    else
    {
        local_win = newwin(height, width, starty, startx);
        scrollok(local_win, TRUE);   // use scroll
        idlok(local_win, TRUE);      // enable insert, delete ok for scroll
        //wrefresh(local_win);
        //wmove(local_win, 1, 0);   // position cursor at top
    }
    return local_win;
}


// FUNCTION   : input_win()
// DESCRIPTION: This function takes the user input and stores input text to c string
// PARAMETERS : WINDOW* win
//              char* word
// RETURN     : nothing
void input_win(WINDOW* win, char* word)
{
    int i, ch;
    int maxrow, maxcol, row = 1, col = 0;
    int max_input = 79;

    blankWin(win);                    // make it a clean window
    getmaxyx(win, maxrow, maxcol);    // get window size
    bzero(word, BUFFER_SIZE);
    wmove(win, 1, 1);                 // move the cursor
    wprintw(win, "> ");               // print in the window
    wrefresh(win);                    // update screen and clean the buffer

    for (i = 0; (ch = wgetch(win)) != '\n'; i++)
    {
        word[i] = ch;
        if (i < max_input)                 // if within window
        {
            wprintw(win, "%c", word[i]);    // display the char recv'd
        }
    }
}


// FUNCTION   : display_win()
// DESCRIPTION: This function takes WINDOW*, timestamp, window row, MESSAGE*, char* buffer,
// and displays the output format [IP, user, message(40chars), timestamp]
// PARAMETERS : WINDOW* win
//              char* tstmp
//              int whichRow
//              int shouldBlank
//              MESSAGE* msg
//              char* buf
// RETURN     : nothing
void display_win(WINDOW* win, char* tstmp, int whichRow, int shouldBlank, MESSAGE* msg, char* buf)
{
    if (whichRow >= MAX_DISPLAY_LINE)
    {
        wscrl(win, 1);      // scroll up one line
        wrefresh(win);      // update window and clean buffer
        wmove(win, (MAX_DISPLAY_LINE - 1), 0);   // move cursor to the beginning of line 10 (10-1)
        wclrtoeol(win);     // clean the line 10
        wrefresh(win);      // update window and clean buffer
        wmove(win, (MAX_DISPLAY_LINE - 1), 1);  // move cursor to the beginning of the line for wprintw later
    }
    else
    {
        wmove(win, whichRow, 1);
    }
    wrefresh(win);

    if (strcmp(msg->id, user) == 0)
    {
        wprintw(win, "%-15s [%-5s] >> %-40s %10s", msg->ipAddress, msg->id, buf, tstmp);
    }
    else
    {
        wprintw(win, "%-15s [%-5s] << %-40s %10s", msg->ipAddress, msg->id, buf, tstmp);
    }
    wrefresh(win); // update screen and clean the buffer
    //refresh(win);

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
    wrefresh(win);
}  /* blankWin */
