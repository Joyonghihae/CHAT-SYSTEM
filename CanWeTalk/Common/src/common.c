// FILE          : common.c
// PROJECT       : CanWeTalk
// programmer    : Euyoung Kim, Raj Dudhat, Yujin Jeong, Yujung Park
// FIRST VERSION : 2023-03-18
// DESCRIPTION   : This is a common file for chat-server, chat-client applications of CanWeTalk system


#include "../inc/common.h"

// FUNCTION   : writeLog()
// DESCRIPTION: This function takes parameters of apptype and c style string
//              write logfile for each application type
// PARAMETERS : char appType
//              char* logText
// RETURN     : int return value
int writeLogFile(char appType, char* logText)
{
    FILE* pLogFile = NULL;
    char logEntry[LOG_BUFFER_SIZE] = { 0 };
    char fileName[LOG_BUFFER_SIZE] = { 0 };
    char serverFileName[] = "/tmp/chatServer.log";
    char clientFileName[] = "/tmp/chatClient.log";


    time_t rawtime;
    struct tm* timeinfo;
    time(&rawtime);
    timeinfo = localtime(&rawtime);

    sprintf(logEntry, "[%04d-%02d-%02d %02d:%02d:%02d] : %s\n",
        timeinfo->tm_year + 1900, timeinfo->tm_mon + 1, timeinfo->tm_mday, timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec,
        logText);

    if (appType == SERVER)
    {
        strcpy(fileName, serverFileName);
    }
    else
    {
        strcpy(fileName, clientFileName);
    }

    pLogFile = fopen(fileName, "a");
    if (pLogFile == NULL)
    {
        perror("[LOG ERROR-1] ");
        return -1; //fopen error - write permission or etc)
    }
    else
    {
        fprintf(pLogFile, "%s\n", logEntry);
        if (fclose(pLogFile) != 0)
        {
            perror("[LOG ERROR-2] ");
            return -2; //fclose error
        }
    }

    return 1;
}