//
// Created by Maxi on 7/15/20.
//

#include "unistd.h"
#include "stdlib.h"
#include "stdio.h"
#include "string.h"
#include "fcntl.h"

void childFunction(char *line);

char *readALineFrom(int fromWhat);

int main(int argc, char *argv[])
{

    // the message to show at the beginning.
    char message[] = "Enter a sentence, e.g., Good morning sir,my name is BigFoot\n";
    // store the input:
    char *inputLine = (char *) malloc(64 * sizeof(char));

    // A message to show while waiting.
    char waitingMessage[] = "Created a child to perform task, waiting...\n";
    // A message to show when there is an empty input in child process.
    char emptyLineMessage[] = "Empty line\n";
    // store child process exit status
    int status;

    // child process pid.
    int pid = 0;

    // file index
    int fp;
    // file path
    char *filePath = "./codes.txt";

    // a goto label.
    BEGINNING:;
    // reset inputLine for goto.
    free(inputLine);
    inputLine = (char *) malloc(64 * sizeof(char));

    // show the beginning message. Using write() to STDOUT_FILENO (1);
    write(STDOUT_FILENO, message, sizeof(message));

    // read a line from STDIN_FILENO(0).
    inputLine = readALineFrom(STDIN_FILENO);

    // reset child process pid for goto.
    pid = 0;
    if ((pid = fork()) != 0)
    {
        // parent process
        // write the message to std out.
        write(STDOUT_FILENO, waitingMessage, sizeof(waitingMessage));

        // wait until child process dead.
        wait(&status);
        // check child process exit status, if it equal to 10.
        if (WEXITSTATUS(status) == 10)
        {
            // write the emptyLineMessage to std out.
            write(STDOUT_FILENO, emptyLineMessage, sizeof(emptyLineMessage));
            // go to the beginning of the program.
            goto BEGINNING;
        }

        // try to open the file within the same directory.
        if ((fp = open(filePath, O_RDONLY)) == -1)
        {
            // raise an error if there is an error.
            perror("Cannot open output file!\n");
            exit(1);
        }

        // get file size.
        int fileSize = lseek(fp, 0, SEEK_END);
        // reset file stream position
        lseek(fp, 0, SEEK_SET);
        // initial the buffer to store the one line content of the file.
        char *result = (char *) malloc(fileSize * sizeof(char));
        // read one line content from the file.
        read(fp, result, fileSize);

        // close the file stream.
        close(fp);

        // show the content of the file on screen.
        write(STDOUT_FILENO, result, fileSize);
        write(STDOUT_FILENO, "\n", 1);

        //free result, malloc()
        free(result);
        // go to the beginning, step 1.
        goto BEGINNING;
    }
    else if (pid == -1)
    {
        // error handle
        perror("fork() ERROR!");
    }
    else
    {
        // child process
        childFunction(inputLine);
    }

    // free char string, malloc()
    free(inputLine);
    // exit normally
    exit(0);
}

// the child function
void childFunction(char *line)
{
    // inside child process
    // write a message: "I am a child working for my parent".
    char childMessage[] = "I am a child working for my parent\n";
    write(STDOUT_FILENO, childMessage, sizeof(childMessage));
    // get the unit length of line
    unsigned long stringLength = strlen(line);
    // if the input line is empty.
    if (stringLength <= 1)
    {
        // exit with status 10.
        exit(10);
    }
    // initialize the result.
    char *result = (char *) malloc(sizeof(line));
    // temporary store a char
    char theChar = (char) NULL;
    // index of the string.
    int i = 0;
    // while theChar is not the end of the line.
    while (theChar != '\n')
    {
        // get the current char in line[]
        theChar = line[i];
        // put the current char in result[] reversely.
        result[stringLength - i - 2] = theChar;
        // update i for next char.
        i++;
    }
    // append '\n' at the end of the string.
    result[i] = '\n';

    // file index
    int fp;
    // file path
    char *filePath = "./codes.txt";
    // try to open the file within the same directory.
    if ((fp = open(filePath, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)) == -1)
    {
        // raise an error if there is an error.
        perror("Cannot open output file!\n");
        exit(1);
    }

    // write result to the file.
    write(fp, result, strlen(result));
    // close the file.
    close(fp);
    // exit normally.
    exit(0);
}

// read a line from somewhere
char *readALineFrom(int fromWhat)
{
    // temporary store a char
    char tmp;
    // indicate the index used in inputLine.
    int i = 0;
    // store the input:
    char *inputLine = (char *) malloc(64 * sizeof(char));
    // initial inputLine size
    int inputLineSize = 64;

    // when there is next byte got read.
    while (read(fromWhat, &tmp, 1) > 0)
    {
        // if the index used in inputLine is bigger or equal to the inputLine size.
        if (i >= inputLineSize)
        {
            // double the inputLine size
            inputLineSize *= 2;
            // re-allocate the inputLine to double the size.
            inputLine = realloc(inputLine, inputLineSize * sizeof(char));
        }
        // add the char to the end of the inputLine.
        inputLine[i] = tmp;
        // if tmp equal to line end, break the loop.
        if (tmp == '\n')
        { break; }
        // increase i to index on the next using unit.
        i++;
    }
    return inputLine;
}