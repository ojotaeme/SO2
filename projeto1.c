/*
 * Exercise: Enhanced Shell Implementation with Signal Handling
 *
 * Implement a C program that simulates an advanced shell (command-line interpreter) with additional features:
 *
 * Key Features:
 * 1. **Prompt**:
 *    - Display a prompt in the format `[MySh] <user>@<hostname>:<current_directory>$`.
 *    - Normalize the path to display `~` for the user's home directory.
 *
 * 2. **Command Execution**:
 *    - Parse and execute commands entered by the user.
 *    - Support up to 32 arguments per command.
 *    - Use `execvp` to execute external commands.
 *
 * 3. **Built-in Commands**:
 *    - `cd`: Change the current working directory.
 *    - `exit`: Exit the shell.
 *
 * 4. **Signal Handling**:
 *    - Ignore `Ctrl+C` (SIGINT) and `Ctrl+Z` (SIGTSTP) signals.
 *    - Provide a custom handler to print a newline when these signals are received.
 *
 * 5. **Process Management**:
 *    - Use `fork` to create a child process for command execution.
 *    - The parent process waits for the child process to complete using `wait`.
 *
 * Functions and System Calls:
 * - `getcwd`: Retrieve the current working directory.
 * - `getenv`: Get environment variables (e.g., user and home directory).
 * - `gethostname`: Retrieve the hostname of the system.
 * - `chdir`: Change the current working directory.
 * - `fork`: Create a child process for command execution.
 * - `execvp`: Execute a command in the child process.
 * - `wait`: Wait for the child process to complete.
 * - `sigaction`: Handle or ignore specific signals (e.g., SIGINT, SIGTSTP).
 *
 * Usage:
 * Compile and run the program. The shell will display a prompt and allow users to execute commands interactively. Type `exit` to terminate the shell.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#define MAX_ARGS 32
#define MAX_PATH 512
#define MAX_HOSTNAME 128
#define MAX_COMMAND 256

// Handler para sinais
void handler(int signal_number) {
    printf("\n");
}

int main(int argc, char *argv[]) {
    int count, i, status;
    char path[MAX_PATH];
    char normalizedPath[MAX_PATH];
    char hostName[MAX_HOSTNAME];
    char *userName;
    char commandLine[MAX_COMMAND];
    char *token;
    char *argList[MAX_ARGS];
    const char *barraToken = "/";
    const char *spaceToken = " ";
    pid_t child_pid;

    // Ignorando sinais de Ctrl+C e Ctrl+Z
    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = &handler;
    sigaction(SIGTSTP, &sa, NULL);
    sigaction(SIGINT, &sa, NULL);

    printf("\nInicializando [MySh]...\n");

    // Loop infinito do shell
    while (1) {
        // Obtendo informações do sistema
        if (getcwd(path, MAX_PATH) == NULL) {
            perror("getcwd");
            exit(EXIT_FAILURE);
        }
        if (gethostname(hostName, MAX_HOSTNAME) == -1) {
            perror("gethostname");
            exit(EXIT_FAILURE);
        }
        userName = getenv("USER");
        if (userName == NULL) {
            userName = "unknown";
        }

        // Normalizando o PATH
        strncpy(normalizedPath, path, MAX_PATH);
        token = strtok(normalizedPath, barraToken);
        if (token && strcmp(token, "home") == 0) {
            token = strtok(NULL, barraToken);
            if (token && strcmp(token, userName) == 0) {
                snprintf(normalizedPath, MAX_PATH, "~");
                while ((token = strtok(NULL, barraToken)) != NULL) {
                    strncat(normalizedPath, "/", MAX_PATH - strlen(normalizedPath) - 1);
                    strncat(normalizedPath, token, MAX_PATH - strlen(normalizedPath) - 1);
                }
                strncpy(path, normalizedPath, MAX_PATH);
            }
        }

        // Exibindo o prompt
        printf("[MySh] %s@%s:%s$ ", userName, hostName, path);
        if (fgets(commandLine, sizeof(commandLine), stdin) == NULL) {
            perror("fgets");
            continue;
        }

        // Removendo o caractere de nova linha
        commandLine[strcspn(commandLine, "\n")] = 0;

        // Continuar se a linha de comando estiver vazia
        if (strcmp(commandLine, "") == 0) continue;

        // Comando "exit"
        if (strcmp(commandLine, "exit") == 0) break;

        // Quebrando a linha de comando em argList[]
        for (i = 0; i < MAX_ARGS; i++) {
            argList[i] = NULL;
        }
        count = 0;
        token = strtok(commandLine, spaceToken);
        while (token != NULL && count < MAX_ARGS) {
            argList[count++] = token;
            token = strtok(NULL, spaceToken);
        }

        // Comando "cd"
        if (strcmp(argList[0], "cd") == 0) {
            if (argList[1] == NULL) {
                chdir(getenv("HOME"));
            } else if (chdir(argList[1]) != 0) {
                perror("cd");
            }
        } else {
            // Execução do programa da linha de comando
            child_pid = fork();
            if (child_pid < 0) {
                perror("fork");
                exit(EXIT_FAILURE);
            }
            if (child_pid != 0) {
                wait(&status);
            } else {
                execvp(argList[0], argList);
                perror("execvp");
                exit(EXIT_FAILURE);
            }
        }
    }

    printf("Saindo do [MySh]...\n\n");
    return 0;
}