/* Richard Davis
 * CSCI 4100
 * Programming Assignment 2
 * The Simple Management of Applications SHell (SMASH) prompts for and executes
 * commands. It supports redirection of I/O and piping between two commands.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h>

const int MAX_LINE = 256;
const int MAX_ARGS = 64;

int split_cmd(char *cmd, char *args[]);
void process_args(char *args[], int count);

void exec_cmd(char *args[]);
void exec_cmd_red_out(char *args[], char *filename);
void exec_cmd_red_in(char *args[], char *filename);
void exec_cmd_pipe(char *args1[], char *args2[]);


int main()
{
  // The command line string
  char cmd[MAX_LINE];

  // The comand line broken into arguments
  char *args[MAX_ARGS];

  // Repeatedly prompt for a command and execute it
  while(1) {
    // Display prompt and read the command line
    printf("> ");
    fgets(cmd, MAX_LINE, stdin);

    // Process the command
    int count = split_cmd(cmd, args);
    process_args(args, count);
  }

  return 0;
}

// Break the command into command line arguments
int split_cmd(char *cmd, char *args[]) {
  int index = 0;

  // Extract each comand line argument and store it in args
  char *arg = strtok(cmd, " \t\n\v\f\r");
  while(arg != NULL && index < MAX_ARGS - 1) {
    args[index] = arg;
    arg = strtok(NULL, " \t\n\v\f\r");
    index++;
  }
  args[index] = NULL;
  return index;
}

// Takes an array of command line arguments and an argument count and processes
// the command line
void process_args(char *args[], int count) {

  // Find the location of the delimiter (|, <, or >)
  int i, delim = -1;
  for(i = 0; i < count; i++) {
    if(strcmp(args[i], "|") == 0 ||
       strcmp(args[i], "<") == 0 ||
       strcmp(args[i], ">") == 0)
      delim = i;
  }

  // If user entered a blank line do nothing
  if(count == 0)
    return;

  // If user entered exit, then exit the shell
  else if(count == 1 && strcmp(args[0], "exit") == 0) {
    puts("[exiting smash]\n");
    exit(EXIT_SUCCESS);
  }

  // If there are no pipes or redirects, execute the command
  else if(delim < 0)
    exec_cmd(args);

  // Pipe or redirect appears as the first or last argument
  else if(delim == 0 || delim == count - 1)
    fprintf(stderr, "smash: sytnax error\n");

  // Redirect output
  else if(strcmp(args[delim], ">") == 0) {
    args[delim] = NULL;
    exec_cmd_red_out(args, args[delim+1]);
  }

  // Redirect input
  else if(strcmp(args[delim], "<") == 0) {
    args[delim] = NULL;
    exec_cmd_red_in(args, args[delim+1]);
  }

  // Pipe
  else if(strcmp(args[delim], "|") == 0) {
    args[delim] = NULL;
    exec_cmd_pipe(args, &args[delim+1]);
  }

  // If this point is reached something went wrong with this code
  else {
    fprintf(stderr, "smash: internal error");
  }
}

// Execute the command
void exec_cmd(char *args[]) {

  // Fork a child process
  pid_t pid = fork();

  // Child process
  if(pid == 0) {
    // Execute the command
    execvp(args[0], args);
    fprintf(stderr, "smash: %s: command not found\n", args[0]);
    exit(EXIT_FAILURE);
  }

  // Parent process
  else {
    // Wait for the child to complete
    wait(NULL);
  }
}

// Redirect output to write to the given file, then execute the command
void exec_cmd_red_out(char *args[], char *filename) {
  pid_t pid = fork();

  if (pid == 0) {
    close(1);
    int fd = creat(filename, 0644);
    if (fd < 0) {
      fprintf(stderr, "smash: %s: file unable to be created\n", args[0]);
      exit(EXIT_FAILURE);
    }

    execvp(args[0], args);
    fprintf(stderr, "smash: %s: command not found\n", args[0]);
    exit(EXIT_FAILURE);
  }

  else {
    wait(NULL);
  }
}

// Redirect input to read from the given file, then execute the command
void exec_cmd_red_in(char *args[], char *filename) {
  pid_t pid = fork();

  if (pid == 0) {
    close(0);
    int fd = open(filename, O_RDONLY);
    if (fd < 0) {
      fprintf(stderr, "smash: %s: file unable to be read\n", args[0]);
      exit(EXIT_FAILURE);
    }

    execvp(args[0], args);
    fprintf(stderr, "smash: %s: command not found\n", args[0]);
    exit(EXIT_FAILURE);
  }

  else {
    wait(NULL);
  }
}

// Create a pipline between the first command and the second command
void exec_cmd_pipe(char *args1[], char *args2[]) {
  pid_t pid = fork();

  if (pid == 0) {
    int pipefd[2];
    int fd = pipe(pipefd);

    if (fd == -1) {
      fprintf(stderr, "smash: ");
      perror("pipe");
      exit(EXIT_FAILURE);
    }

    pid_t pid2 = fork();

    if (pid2 == 0) {
      close(pipefd[1]);
      dup2(pipefd[0], 1);
      close(pipefd[0]);

      execvp(args2[0], args2);
      fprintf(stderr, "smash: ");
      perror("pipe");
      exit(EXIT_FAILURE);
    }

    close(pipefd[0]);
    dup2(pipefd[1], 0);
    close(pipefd[1]);

    execvp(args1[0], args1);
    fprintf(stderr, "smash: ");
    perror("pipe");
    exit(EXIT_FAILURE);
  }

  else {
    wait(NULL);
  }
}
