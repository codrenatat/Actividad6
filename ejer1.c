#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include <sys/time.h>
#include <pthread.h>

#define SIZE 2000
#define INICIAL 900000000
#define FINAL 1000000000
#define NUM_THREADS 4 // Número de hilos

int mat[SIZE][SIZE];

// Pasar datos a los hilos
typedef struct {
    int start_row;
    int end_row;
} ThreadData;

void initmat(int mat[][SIZE]);
void printnonzeroes(int mat[SIZE][SIZE]);
int isprime(int n);
void* process_rows(void* arg);

int main() {
    long long start_ts;
    long long stop_ts;
    long long elapsed_time;
    struct timeval ts;
    pthread_t threads[NUM_THREADS];
    ThreadData thread_data[NUM_THREADS];
    int i;

    // Inicializa la matriz con números al azar
    initmat(mat);

    gettimeofday(&ts, NULL);
    start_ts = ts.tv_sec; // Tiempo inicial

    // Crear hilos para procesar porciones de la matriz
    int rows_per_thread = SIZE / NUM_THREADS;
    for (i = 0; i < NUM_THREADS; i++) {
        thread_data[i].start_row = i * rows_per_thread;
        thread_data[i].end_row = (i + 1) * rows_per_thread;
        if (i == NUM_THREADS - 1) {
            thread_data[i].end_row = SIZE; // El último hilo toma las filas restantes
        }
        pthread_create(&threads[i], NULL, process_rows, (void*)&thread_data[i]);
    }

    // Esperar a que todos los hilos terminen
    for (i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    gettimeofday(&ts, NULL);
    stop_ts = ts.tv_sec; // Tiempo final
    elapsed_time = stop_ts - start_ts;

    printnonzeroes(mat);
    printf("------------------------------\n");
    printf("TIEMPO TOTAL, %lld segundos\n", elapsed_time);

    return 0;
}

void initmat(int mat[][SIZE]) {
    int i, j;
    srand(getpid());
    for (i = 0; i < SIZE; i++) {
        for (j = 0; j < SIZE; j++) {
            mat[i][j] = INICIAL + rand() % (FINAL - INICIAL);
        }
    }
}

void printnonzeroes(int mat[SIZE][SIZE]) {
    int i, j;
    for (i = 0; i < SIZE; i++) {
        for (j = 0; j < SIZE; j++) {
            if (mat[i][j] != 0) {
                printf("%d\n", mat[i][j]);
            }
        }
    }
}

int isprime(int n) {
    int d = 3;
    int prime = n == 2;
    int limit = sqrt(n);

    if (n > 2 && n % 2 != 0) {
        while (d <= limit && n % d) {
            d += 2;
        }
        prime = d > limit;
    }
    return prime;
}

void* process_rows(void* arg) {
    ThreadData* data = (ThreadData*)arg;
    int i, j;

    for (i = data->start_row; i < data->end_row; i++) {
        for (j = 0; j < SIZE; j++) {
            if (!isprime(mat[i][j])) {
                mat[i][j] = 0;
            }
        }
    }

    pthread_exit(NULL);
}