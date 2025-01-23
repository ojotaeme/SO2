/*
 * Exercise: Simple Shell Implementation
 *
 * Implement a C program that simulates a basic shell (command-line interpreter). The shell should:
 *
 * Key Features:
 * 1. **Prompt**:
 *    - Display a prompt in the format `[MySh] <user>@<hostname>:<current_directory>$`.
 *    - Use the `getcwd` function to display the current working directory.
 *
 * 2. **Command Execution**:
 *    - Read user input and parse it into a command and its arguments.
 *    - Use `execvp` to execute commands entered by the user.
 *    - Support up to 10 arguments per command.
 *
 * 3. **Process Management**:
 *    - Use `fork` to create a child process for command execution.
 *    - The parent process waits for the child process to complete using `waitpid`.
 *
 * 4. **Exit Command**:
 *    - Allow the user to exit the shell by typing `exit`.
 *
 * Functions and System Calls:
 * - `getcwd`: Retrieve the current working directory.
 * - `getenv`: Get the username from the environment variables.
 * - `gethostname`: Retrieve the hostname of the system.
 * - `fork`: Create a child process.
 * - `execvp`: Execute a command in the child process.
 * - `waitpid`: Wait for the child process to complete.
 *
 * Usage:
 * Compile and run the program. The shell will display a prompt and allow users to execute commands interactively. Type `exit` to terminate the shell.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

#define TAMANHO 100
#define QUANT_ARG 10

char host[255] = "";
char * user;

int main() {

  int flag = 1;

  char command[TAMANHO];
  char * arguments[QUANT_ARG];
  char * token;

  do {
    char buf[1024];
    if (getcwd(buf, sizeof(buf)) != NULL) {
      user = getenv("USER");
      gethostname(host, 255);
      printf("[MySh] %s@%s:%s$ ", user, host, buf);
      fgets(command, TAMANHO, stdin);
      command[strcspn(command, "\n")] = '\0'; // Remove a quebra de linha

      if (strcmp(command, "exit") == 0) {
        flag = 0;
      } else {
        token = strtok(command, " ");
        arguments[0] = token;
        int arg_count = 1;
        while ((token = strtok(NULL, " ")) != NULL && arg_count < QUANT_ARG - 1) {
          arguments[arg_count] = token;
          arg_count++;
        }
        arguments[arg_count] = NULL;

        // Faz Fork do processo filho para executar o comando
        pid_t pid = fork();
        if (pid == 0) {
          // Processo filho
          execvp(arguments[0], arguments);
          // Se execvp retornar, houve um erro
          perror("Erro ao executar comando");
          exit(EXIT_FAILURE);
        } else if (pid < 0) {
          // Fork falhou
          perror("Erro ao criar processo filho");
        } else {
          // Processo pai
          // Espera o processo filho terminar
          int status;
          waitpid(pid, & status, 0);
        }
      }
    } else {
      perror("getcwd");
      return 1;
    }

  } while (flag == 1);
  return 0;
}
