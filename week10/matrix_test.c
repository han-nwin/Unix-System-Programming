#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

// Define matrix size, global mutex, and diagonal sum variable
int N, M;
int **MA, **MB, **MC;
pthread_mutex_t matrix_mutex;
int diagonal_sum = 0;

// Matrix multiplication function for each thread
void *multiply(void *arg) {
    int thread_id = *(int *)arg;
    int start_col = (N / M) * thread_id;
    int end_col = start_col + (N / M);

    for (int i = 0; i < N; i++) {
        for (int j = start_col; j < end_col; j++) {
            int sum = 0;
            for (int k = 0; k < N; k++) {
                sum += MA[i][k] * MB[k][j];
            }

            // Lock mutex before updating matrix MC
            pthread_mutex_lock(&matrix_mutex);
            MC[i][j] = sum;
            printf("Thread %d updates MC(%d,%d) set to %d\n", thread_id, i, j, sum);

            // If diagonal element, update the global diagonal sum
            if (i == j) {
                diagonal_sum += sum;
            }
            pthread_mutex_unlock(&matrix_mutex);
        }
    }
    pthread_exit(NULL);
}

int main(int argc, char *argv[]) {
    // Argument parsing, matrix initialization, and thread creation here
    ...
	if (argc != 3) {
        printf("Usage: %s <N (matrix size)> <M (number of threads)>\n", argv[0]);
        exit(1);
    }
    int N = atoi(argv[1]);  // Matrix size NxN
    int M = atoi(argv[2]);  // Number of threads

    // Validate that N is a multiple of M
    if (N % M != 0 || N < M) {
        printf("Error: N must be a multiple of M, and N >= M\n");
        exit(1);
    }
    
    // Create M threads
    pthread_t threads[M];
    int thread_ids[M];
    for (int i = 0; i < M; i++) {
        thread_ids[i] = i;
        pthread_create(&threads[i], NULL, multiply, &thread_ids[i]);
    }

    // Join threads
    for (int i = 0; i < M; i++) {
        pthread_join(threads[i], NULL);
    }

    // Print diagonal sum
    printf("Diagonal sum: %d\n", diagonal_sum);

    // Call system() to execute shell commands
    system("date; hostname; whoami; ps -eaf; ls -l");
    system("who > week10who.txt");

    return 0;
}

