/*
 * Exercise: Sleeping Barber Problem Simulation
 *
 * Implement a C program that simulates the "Sleeping Barber Problem" using multithreading. 
 * The program models a barbershop with multiple barbers, a waiting room with limited chairs, 
 * and customers arriving at random intervals.
 *
 * Key Features:
 * 1. **Barbers**:
 *    - Barbers sleep when there are no customers.
 *    - Barbers wake up to serve customers when they arrive.
 *    - Each barber works for a specified amount of time to cut a customer's hair.
 *
 * 2. **Customers**:
 *    - Customers arrive at random intervals.
 *    - If the waiting room is full, customers leave without getting a haircut.
 *    - Customers wait in the queue until served by a barber.
 *
 * 3. **Synchronization**:
 *    - Use mutexes and condition variables to synchronize barbers and customers.
 *    - Ensure thread-safe access to shared resources like the customer queue.
 *
 * Functions:
 * - `initialize_flag`: Initializes synchronization primitives (mutex and condition variable).
 * - `set_thread_flag`: Updates the thread flag and signals waiting threads.
 * - `thread_function`: The main function executed by barber threads to serve customers.
 * - `cortar_cabelo`: Simulates the time taken by a barber to cut hair.
 *
 * Command-line Arguments:
 * - Number of barbers (`qtdBarbeiros`).
 * - Number of chairs in the waiting room (`qtdCadeiras`).
 * - Time taken by a barber to cut hair (`tempoTrabalho` in milliseconds).
 * - Time between customer arrivals (`tempoEntreChegadas` in milliseconds).
 *
 * Usage:
 * Compile and run the program with appropriate arguments:
 * `./program <qtdBarbeiros> <qtdCadeiras> <tempoTrabalho> <tempoEntreChegadas>`.
 */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

int qtdBarbeiros;               
int qtdCadeiras;               
int tempoTrabalho;             
int tempoEntreChegadas;       

int thread_flag;                
int cliente;                    
pthread_cond_t thread_flag_cv;  // Variável de condição para sinalizar mudanças na flag
pthread_mutex_t thread_flag_mutex;  // Mutex para controlar acesso seguro à flag

// Estrutura para passar parâmetros para as threads dos barbeiros
struct char_print_parms {
    int numeroDoBarbeiro;
};

// Estrutura para representar um cliente na fila de espera
struct clientes {
    int numero;             
    struct clientes* next;  
};

// Ponteiros para o início e o fim da lista de clientes
struct clientes* lista_cliente = NULL;
struct clientes* ultimo_cliente = NULL;

void initialize_flag() {
    pthread_mutex_init(&thread_flag_mutex, NULL);  // Inicializa o mutex
    pthread_cond_init(&thread_flag_cv, NULL);      // Inicializa a variável de condição
    thread_flag = 0;    
}

void set_thread_flag(int flag_value) {
    pthread_mutex_lock(&thread_flag_mutex);     // Bloqueia o acesso ao mutex
    thread_flag = flag_value;                   // Atualiza o valor da flag
    pthread_cond_signal(&thread_flag_cv);       // Sinaliza que a flag foi alterada
    pthread_mutex_unlock(&thread_flag_mutex);   // Libera o acesso ao mutex
}

void* thread_function(void* thread_arg) {
    struct char_print_parms* arg = (struct char_print_parms*)thread_arg;
    int numeroDoBarbeiro = arg->numeroDoBarbeiro;
    struct clientes* cli = NULL;

    while (1) {
        pthread_mutex_lock(&thread_flag_mutex);  // Bloqueia o acesso ao mutex
        if (lista_cliente == NULL) {
            while (thread_flag == 0) {
                printf("O barbeiro %i está dormindo.\n", numeroDoBarbeiro);
                pthread_cond_wait(&thread_flag_cv, &thread_flag_mutex);  // Aguarda sinal para acordar
                printf("O barbeiro %i acordou.\n", numeroDoBarbeiro);
            }
        }
        thread_flag--;      // Decrementa o contador de barbeiros acordados
        cli = lista_cliente;
        lista_cliente = cli->next;
        pthread_mutex_unlock(&thread_flag_mutex);  // Libera o acesso ao mutex
        printf("O Barbeiro %i está cortando o cabelo do Cliente %i.\n", numeroDoBarbeiro, cli->numero);
        cortar_cabelo();    
        printf("O Barbeiro %i acabou de cortar o cabelo do Cliente %i.\n", numeroDoBarbeiro, cli->numero);
    }

    return NULL;
}

void cortar_cabelo() {
    usleep(tempoTrabalho * 1000);  
}

int main(int argc, char* argv[]) {

    int i;
    pthread_t idBarbeiro[qtdBarbeiros];    

    qtdBarbeiros = atoi(argv[1]);
    qtdCadeiras = atoi(argv[2]);
    tempoTrabalho = atoi(argv[3]);
    tempoEntreChegadas = atoi(argv[4]);

    initialize_flag();  

    for (i = 1; i <= qtdBarbeiros; i++) {
        thread_args.numeroDoBarbeiro = i;
        pthread_create(&idBarbeiro[i], NULL, &thread_function, &thread_args);
        usleep(100000);  
    }

    // Loop para simular a chegada dos clientes
    for (cliente = 1; cliente <= 20; cliente++) {
        printf("\n");
        if (thread_flag >= qtdCadeiras) {
            printf("Cliente %i chegou e foi embora sem cortar o cabelo. Sala de espera cheia.\n", cliente);
        } else {
            if (lista_cliente == NULL) {
                struct clientes* cli = malloc(sizeof(struct clientes));
                cli->numero = cliente;
                cli->next = NULL;
                lista_cliente = cli;
                ultimo_cliente = cli;
            } else {
                struct clientes* cli = malloc(sizeof(struct clientes));
                cli->numero = cliente;
                cli->next = NULL;
                ultimo_cliente->next = cli;
                ultimo_cliente = cli;
            }
            set_thread_flag(++thread_flag);  // Incrementa a flag para acordar um barbeiro
            printf("Cliente %i chegou.\n", cliente);
        }
        usleep(tempoEntreChegadas * 1000);  // Aguarda o tempo entre a chegada de clientes
    }
    return 0;
}
