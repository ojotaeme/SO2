/*
 * Exercise: File System Simulation
 *
 * Implement a C program that simulates a basic file system with the following features:
 *
 * Key Features:
 * 1. **Directory and File Management**:
 *    - Create directories (`criad <path/name>`).
 *    - Create files with specified sizes (`criaa <path/name> <size>`).
 *    - Remove empty directories (`removed <path/name>`).
 *    - Remove files (`removea <path/name>`).
 *
 * 2. **Directory Listing and Tree Display**:
 *    - List the contents of a directory (`verd <path>`).
 *    - Display the directory tree structure (`arvore`).
 *
 * 3. **Disk Space Management**:
 *    - Show the disk sector map (`mapa`), indicating free and occupied sectors.
 *    - Display the sectors occupied by a specific file (`verset <path/name>`).
 *
 * 4. **File System Initialization**:
 *    - Simulate disk space with 256 blocks, where the first 10 blocks are reserved for boot/system data.
 *    - Track free and occupied blocks using an array (`blocosLivres`).
 *
 * 5. **Command-Line Interface**:
 *    - Provide a shell-like interface for interacting with the file system.
 *    - Support commands such as `ajuda` (help) and `sair` (exit).
 *
 * Functions to Implement:
 * - `inicializar_blocos`: Initialize the disk blocks, marking the first 10 as reserved.
 * - `alocar_bloco`: Allocate a free block from the disk.
 * - `liberar_bloco`: Free an allocated block.
 * - `obter_data_atual`: Get the current date and time for file/directory metadata.
 * - `criad`: Create a new directory in the specified path.
 * - `criaa`: Create a new file in the specified path with allocated blocks.
 * - `removed`: Remove an empty directory.
 * - `removea`: Remove a file and free its allocated blocks.
 * - `verd`: List the contents of a directory, including files and subdirectories.
 * - `arvore`: Display the hierarchical tree structure of directories.
 * - `mapa`: Show the disk sector map, marking free and occupied sectors.
 *
 * Usage:
 * Compile and run the program. Use commands like `criad`, `criaa`, `verd`, etc., to interact with the simulated file system. Type `ajuda` for a full list of commands.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>

typedef struct arquivo {
    struct arquivo *prox;
    int posicao;
} Arquivo;

typedef struct bloco {
    char nome[100];
    int tamanho;
    char data[20];
    Arquivo *arq;
    int posicao;
    struct bloco *prox;
    struct bloco *filho;
} Bloco;

int blocosLivres[256];
int espacosLivres = 256 - 10;
Bloco *raiz;

char in[256], in_bkp[256], argumentos[256];
char **argList;
char **dirList;

void inicializar_blocos();
char* obter_data_atual();
char* substituir_string(const char *string, const char *search, const char *replacement);
void liberar_bloco(int i);
int alocar_bloco();
void criad();
void criaa();
void removed();
void removea();
void verd();
void mapa();
void arvore();
void verset();
void ajuda();

int main() {
    inicializar_blocos();
    raiz = (Bloco*)malloc(sizeof(Bloco));
    strcpy(raiz->nome, "raiz");
    raiz->arq = NULL;
    raiz->prox = NULL;
    raiz->filho = NULL;
    printf("Sistema de arquivos inicializado.\n");

    while (1) {
        printf("[MyExplorer] >> ");
        fgets(in, 256, stdin);
        in[strlen(in) - 1] = '\0';
        if (feof(stdin)) {
            printf("\n");
            exit(0);
        }

        int count = 0;
        argList = (char**)malloc(256 * sizeof(char*));
        char *token = strtok(in, " ");
        while (token) {
            argList[count++] = token;
            token = strtok(NULL, " ");
        }
        argList[count] = NULL;

        if (argList[1] != NULL) 
            strcpy(argumentos, argList[1]);

        if (!strcmp(argList[0], "sair")) 
            exit(0);
        if (!strcmp(argList[0], "ajuda")) {
            ajuda();
            continue;
        }
        if (!strcmp(argList[0], "arvore")) {
            arvore();
            continue;
        }
        if (!strcmp(argList[0], "mapa")) {
            mapa();
            continue;
        }
        if (!strcmp(argList[0], "verset")) {
            verset();
            continue;
        }
        if (!strcmp(argList[0], "verd")) {
            verd();
            continue;
        }
        if (!strcmp(argList[0], "criad")) {
            criad();
            continue;
        }
        if (!strcmp(argList[0], "removed")) {
            removed();
            continue;
        }
        if (!strcmp(argList[0], "criaa")) {
            criaa();
            continue;
        }
        if (!strcmp(argList[0], "removea")) {
            removea();
            continue;
        }
        printf("Comando inválido!\nDigite 'ajuda' para ver a lista de comandos disponíveis.\n");
    }
    return 0;
}

void ajuda() {
    printf("Comandos disponíveis:\n");
    printf("  criad <caminho/nome_do_diretorio> - Cria um novo diretório.\n");
    printf("  criaa <caminho/nome_do_arquivo> <tamanho> - Cria um novo arquivo com o tamanho especificado.\n");
    printf("  removed <caminho/nome_do_diretorio> - Remove um diretório vazio.\n");
    printf("  removea <caminho/nome_do_arquivo> - Remove um arquivo.\n");
    printf("  verd <caminho> - Lista o conteúdo de um diretório.\n");
    printf("  verset <caminho/nome_do_arquivo> - Mostra os setores ocupados por um arquivo.\n");
    printf("  mapa - Mostra o mapa de setores do disco.\n");
    printf("  arvore - Mostra a árvore de diretórios.\n");
    printf("  ajuda - Mostra esta mensagem de ajuda.\n");
    printf("  sair - Sai do sistema de arquivos.\n");
}

void mapa() {
    for (int i = 0; i < 256; i++) {
        if (i < 10) {
            printf("B ");
        } else {
            printf(blocosLivres[i] ? "0 " : "# ");
        }
    }
    printf("\nB-Boot 0-Livre #-Ocupado\n");
}

void arvore() {
    printf("\nEstrutura de Diretórios:\n");
    printf("Raiz\n");
    Bloco* atual = raiz->filho;
    if (atual != NULL) {
        int nivel = 0;
        Bloco* bloco = atual;
        while (bloco != NULL) {
            if (bloco->arq == NULL) {
                for (int i = 0; i < nivel; i++) printf("  ");
                printf("|- %s/\n", bloco->nome);
            }
            if (bloco->filho != NULL) {
                bloco = bloco->filho;
                nivel++;
            } else if (bloco->prox != NULL) {
                bloco = bloco->prox;
            } else {
                bloco = NULL;
            }
        }
    }
}

void inicializar_blocos() {
    for (int i = 0; i < 256; i++) {
        blocosLivres[i] = (i < 10) ? 0 : 1;
    }
    espacosLivres = 256 - 10;
    printf("Blocos inicializados.\n");
}

void liberar_bloco(int i) {
    if (i >= 0 && i < 256 && !blocosLivres[i]) {
        blocosLivres[i] = 1;
        espacosLivres++;
        printf("Bloco %d liberado.\n", i);
    }
}

int alocar_bloco() {
    for (int i = 10; i < 256; i++) {
        if (blocosLivres[i]) {
            blocosLivres[i] = 0;
            espacosLivres--;
            printf("Bloco %d alocado.\n", i);
            return i;
        }
    }
    printf("Erro: não há mais blocos livres.\n");
    return -1;
}

char* obter_data_atual() {
    static char time_string[20];
    time_t now = time(NULL);
    struct tm *ptm = localtime(&now);

    if (ptm != NULL) {
        strftime(time_string, sizeof(time_string), "%d/%m/%Y %H:%M:%S", ptm);
    } else {
        strcpy(time_string, "Data Inválida");
    }

    return time_string;
}

char* substituir_string(const char *string, const char *search, const char *replacement) {
    const char *pos = strstr(string, search);
    if (!pos) return strdup(string);

    size_t new_length = strlen(string) - strlen(search) + strlen(replacement) + 1;
    char *newstr = (char *)malloc(new_length);
    if (!newstr) return NULL;

    size_t prefix_length = pos - string;
    strncpy(newstr, string, prefix_length);
    newstr[prefix_length] = '\0';
    strcat(newstr, replacement);
    strcat(newstr, pos + strlen(search));

    return newstr;
}

void criad() {
    if (argList[1] == NULL) {
        printf("Erro: nome do diretório não fornecido.\n");
        return;
    }

    dirList = (char**)malloc(256 * sizeof(char*));
    int count = 0;
    char *token = strtok(argList[1], "/");
    while (token) {
        dirList[count++] = token;
        token = strtok(NULL, "/");
    }
    dirList[count] = NULL;

    Bloco* atual = raiz;
    int i = 0;

    while (dirList[i + 1] != NULL) {
        Bloco* current = atual->filho;
        while (current != NULL) {
            if (current->arq == NULL && strcmp(current->nome, dirList[i]) == 0) {
                atual = current;
                break;
            }
            current = current->prox;
        }
        if (current == NULL) {
            printf("Erro: diretório '%s' não encontrado.\n", dirList[i]);
            free(dirList);
            return;
        }
        i++;
    }

    Bloco* current = atual->filho;
    while (current != NULL) {
        if (current->arq == NULL && strcmp(current->nome, dirList[i]) == 0) {
            printf("Erro: diretório '%s' já existe.\n", dirList[i]);
            free(dirList);
            return;
        }
        current = current->prox;
    }

    int pos = alocar_bloco();
    if (pos == -1) {
        free(dirList);
        return;
    }

    Bloco* novoBloco = (Bloco*)malloc(sizeof(Bloco));
    if (novoBloco == NULL) {
        printf("Erro: falha na alocação de memória.\n");
        free(dirList);
        return;
    }

    strcpy(novoBloco->nome, dirList[i]);
    strcpy(novoBloco->data, obter_data_atual());
    novoBloco->arq = NULL;
    novoBloco->filho = NULL;
    novoBloco->prox = atual->filho;
    novoBloco->posicao = pos;
    atual->filho = novoBloco;

    printf("Diretório '%s' criado com sucesso.\n", dirList[i]);
    free(dirList);
}

void criaa() {
    if (argList[1] == NULL || argList[2] == NULL) {
        printf("Erro: nome e/ou tamanho do arquivo não fornecido.\n");
        return;
    }

    dirList = (char**)malloc(256 * sizeof(char*));
    if (dirList == NULL) {
        printf("Erro: falha na alocação de memória.\n");
        return;
    }

    int count = 0;
    char *token = strtok(argList[1], "/");
    while (token) {
        dirList[count++] = token;
        token = strtok(NULL, "/");
    }
    dirList[count] = NULL;

    Bloco* atual = raiz;
    int i = 0;

    while (dirList[i + 1] != NULL) {
        Bloco* current = atual->filho;
        while (current != NULL) {
            if (current->arq == NULL && strcmp(current->nome, dirList[i]) == 0) {
                atual = current;
                break;
            }
            current = current->prox;
        }
        if (current == NULL) {
            printf("Erro: diretório '%s' não encontrado.\n", dirList[i]);
            free(dirList);
            return;
        }
        i++;
    }

    Bloco* current = atual->filho;
    while (current != NULL) {
        if (current->arq != NULL && strcmp(current->nome, dirList[i]) == 0) {
            printf("Erro: arquivo '%s' já existe.\n", dirList[i]);
            free(dirList);
            return;
        }
        current = current->prox;
    }

    int file_size = atoi(argList[2]);
    int num_blocks = (file_size + 512 - 1) / 512;
    if (num_blocks > espacosLivres) {
        printf("Erro: espaço insuficiente para criar o arquivo.\n");
        free(dirList);
        return;
    }

    Arquivo* arq = (Arquivo*)malloc(sizeof(Arquivo));
    if (arq == NULL) {
        printf("Erro: falha na alocação de memória.\n");
        free(dirList);
        return;
    }
    arq->posicao = alocar_bloco();
    arq->prox = NULL;

    Arquivo* temp = arq;
    for (int j = 1; j < num_blocks; j++) {
        Arquivo* novoArq = (Arquivo*)malloc(sizeof(Arquivo));
        if (novoArq == NULL) {
            printf("Erro: falha na alocação de memória.\n");
            while (arq != NULL) {
                Arquivo* next = arq->prox;
                free(arq);
                arq = next;
            }
            free(dirList);
            return;
        }
        novoArq->posicao = alocar_bloco();
        novoArq->prox = NULL;
        temp->prox = novoArq;
        temp = novoArq;
    }

    Bloco* novoBloco = (Bloco*)malloc(sizeof(Bloco));
    if (novoBloco == NULL) {
        printf("Erro: falha na alocação de memória.\n");
        while (arq != NULL) {
            Arquivo* next = arq->prox;
            free(arq);
            arq = next;
        }
        free(dirList);
        return;
    }

    strcpy(novoBloco->nome, dirList[i]);
    strcpy(novoBloco->data, obter_data_atual());
    novoBloco->arq = arq;
    novoBloco->tamanho = file_size;
    novoBloco->filho = NULL;
    novoBloco->prox = atual->filho;
    novoBloco->posicao = arq->posicao;
    atual->filho = novoBloco;

    printf("Arquivo '%s' criado com sucesso.\n", dirList[i]);
    free(dirList);
}

void removed() {
    if (argList[1] == NULL) {
        printf("Erro: nome do diretório não fornecido.\n");
        return;
    }

    dirList = (char**)malloc(256 * sizeof(char*));
    if (dirList == NULL) {
        printf("Erro: falha na alocação de memória.\n");
        return;
    }

    int count = 0;
    char *token = strtok(argList[1], "/");
    while (token) {
        dirList[count++] = token;
        token = strtok(NULL, "/");
    }
    dirList[count] = NULL;

    Bloco* atual = raiz;
    int i = 0;

    while (dirList[i + 1] != NULL) {
        Bloco* current = atual->filho;
        while (current != NULL) {
            if (current->arq == NULL && strcmp(current->nome, dirList[i]) == 0) {
                atual = current;
                break;
            }
            current = current->prox;
        }
        if (current == NULL) {
            printf("Erro: diretório '%s' não encontrado.\n", dirList[i]);
            free(dirList);
            return;
        }
        i++;
    }

    Bloco* alvo = NULL;
    Bloco* current = atual->filho;
    while (current != NULL) {
        if (current->arq == NULL && strcmp(current->nome, dirList[i]) == 0) {
            alvo = current;
            break;
        }
        current = current->prox;
    }
    if (alvo == NULL) {
        printf("Erro: diretório '%s' não encontrado.\n", dirList[i]);
        free(dirList);
        return;
    }

    if (alvo->filho != NULL) {
        printf("Erro: diretório '%s' não está vazio.\n", dirList[i]);
        free(dirList);
        return;
    }

    liberar_bloco(alvo->posicao);

    if (atual->filho == alvo) {
        atual->filho = alvo->prox;
    } else {
        Bloco* prev = atual->filho;
        while (prev->prox != alvo) {
            prev = prev->prox;
        }
        prev->prox = alvo->prox;
    }
    free(alvo);

    printf("Diretório '%s' removido com sucesso.\n", dirList[i]);
    free(dirList);
}

void removea() {
    if (argList[1] == NULL) {
        printf("Erro: nome do arquivo não fornecido.\n");
        return;
    }

    int count = 0;
    dirList = (char**)malloc(256 * sizeof(char*));
    char *token = strtok(argList[1], "/");
    while (token) {
        dirList[count++] = token;
        token = strtok(NULL, "/");
    }
    dirList[count] = NULL;

    Bloco* atual = raiz;
    int i = 0;

    while (dirList[i + 1] != NULL) {
        Bloco* current = atual->filho;
        while (current != NULL) {
            if (current->arq == NULL && strcmp(current->nome, dirList[i]) == 0) {
                atual = current;
                break;
            }
            current = current->prox;
        }
        if (current == NULL) {
            printf("Erro: diretório '%s' não encontrado.\n", dirList[i]);
            return;
        }
        i++;
    }

    Bloco* alvo = NULL;
    Bloco* current = atual->filho;
    while (current != NULL) {
        if (current->arq != NULL && strcmp(current->nome, dirList[i]) == 0) {
            alvo = current;
            break;
        }
        current = current->prox;
    }
    if (alvo == NULL) {
        printf("Erro: arquivo '%s' não encontrado.\n", dirList[i]);
        return;
    }

    Arquivo* arq = alvo->arq;
    while (arq != NULL) {
        Arquivo* prox_arq = arq->prox;
        liberar_bloco(arq->posicao);
        free(arq);
        arq = prox_arq;
    }

    Bloco* prev = atual->filho;
    if (prev == alvo) {
        atual->filho = alvo->prox;
    } else {
        while (prev->prox != alvo) {
            prev = prev->prox;
        }
        prev->prox = alvo->prox;
    }
    free(alvo);

    printf("Arquivo '%s' removido com sucesso.\n", dirList[i]);
}

void verd() {
    int not_found = 0;
    int total_files = 0;
    int total_dirs = 0;
    int file_size = 0;
    int free_space = espacosLivres * 512;
    Bloco* atual = raiz;

    if (argList[1] != NULL) {
        dirList = (char**)malloc(256 * sizeof(char*));
        if (dirList == NULL) {
            printf("Erro: falha na alocação de memória.\n");
            return;
        }

        int count = 0;
        char *token = strtok(argList[1], "/");
        while (token) {
            dirList[count++] = token;
            token = strtok(NULL, "/");
        }
        dirList[count] = NULL;

        int i = 0;

        while (dirList[i] != NULL) {
            Bloco* current = atual->filho;
            while (current != NULL) {
                if (current->arq == NULL && strcmp(current->nome, dirList[i]) == 0) {
                    atual = current;
                    break;
                }
                current = current->prox;
            }
            if (current == NULL) {
                printf("Erro: diretório '%s' não encontrado.\n", dirList[i]);
                free(dirList);
                return;
            }
            i++;
        }

        free(dirList);
    }

    atual = atual->filho;
    if (atual == NULL) {
        printf("Nenhum arquivo ou diretório encontrado.\n");
    } else {
        while (atual != NULL) {
            if (atual->arq == NULL) {
                printf("%s    <DIR>    %s\n", atual->data, atual->nome);
                total_dirs++;
            } else {
                printf("%s    %d    %s\n", atual->data, atual->tamanho, atual->nome);
                total_files++;
                file_size += atual->tamanho;
            }
            atual = atual->prox;
        }
        printf("\n%d arquivo(s)     %d bytes ocupados\n", total_files, file_size);
        printf("%d diretório(s)   %d bytes disponíveis\n", total_dirs, free_space);
    }
}

void verset() {
    if (argList[1] == NULL) {
        printf("Erro: nome do arquivo não fornecido.\n");
        return;
    }

    dirList = (char**)malloc(256 * sizeof(char*));
    if (dirList == NULL) {
        printf("Erro: falha na alocação de memória.\n");
        return;
    }

    int count = 0;
    char *token = strtok(argList[1], "/");
    while (token) {
        dirList[count++] = token;
        token = strtok(NULL, "/");
    }
    dirList[count] = NULL;

    Bloco* atual = raiz;
    int i = 0;

    while (dirList[i + 1] != NULL) {
        Bloco* current = atual->filho;
        while (current != NULL) {
            if (current->arq == NULL && strcmp(current->nome, dirList[i]) == 0) {
                atual = current;
                break;
            }
            current = current->prox;
        }
        if (current == NULL) {
            printf("Erro: diretório '%s' não encontrado.\n", dirList[i]);
            free(dirList);
            return;
        }
        i++;
    }

    Bloco* alvo = NULL;
    Bloco* current = atual->filho;
    while (current != NULL) {
        if (current->arq != NULL && strcmp(current->nome, dirList[i]) == 0) {
            alvo = current;
            break;
        }
        current = current->prox;
    }
    if (alvo == NULL) {
        printf("Erro: arquivo '%s' não encontrado.\n", dirList[i]);
        free(dirList);
        return;
    }

    printf("Setores ocupados pelo arquivo '%s': ", dirList[i]);
    Arquivo* temp = alvo->arq;
    while (temp != NULL) {
        printf("%d ", temp->posicao);
        temp = temp->prox;
    }
    printf("\n");

    free(dirList);
}
