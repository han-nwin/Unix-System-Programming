#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h> // For sleep()

pthread_mutex_t mutex1 = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex2 = PTHREAD_MUTEX_INITIALIZER;

void *thread_func1(void *arg) {
    if (pthread_mutex_lock(&mutex1) == 0) {
        printf("Thread 1: Locked mutex1\n");
        sleep(1); // Simulate some work
        if (pthread_mutex_trylock(&mutex2) == 0) {
            printf("Thread 1: Locked mutex2\n");
            // Critical section (both mutex1 and mutex2 locked)
            pthread_mutex_unlock(&mutex2);
            printf("Thread 1: Released mutex2\n");
        } else {
            printf("Thread 1: Could not lock mutex2, releasing mutex1\n");
        }
        pthread_mutex_unlock(&mutex1);
        printf("Thread 1: Released mutex1\n");
    }
    return NULL;
}

void *thread_func2(void *arg) {
    if (pthread_mutex_lock(&mutex2) == 0) {
        printf("Thread 2: Locked mutex2\n");
        sleep(1); // Simulate some work
        if (pthread_mutex_trylock(&mutex1) == 0) {
            printf("Thread 2: Locked mutex1\n");
            // Critical section (both mutex2 and mutex1 locked)
            pthread_mutex_unlock(&mutex1);
            printf("Thread 2: Released mutex1\n");
        } else {
            printf("Thread 2: Could not lock mutex1, releasing mutex2\n");
        }
        pthread_mutex_unlock(&mutex2);
        printf("Thread 2: Released mutex2\n");
    }
    return NULL;
}

int main() {
    pthread_t t1, t2;

    // Create two threads
    pthread_create(&t1, NULL, thread_func1, NULL);
    pthread_create(&t2, NULL, thread_func2, NULL);

    // Wait for both threads to finish
    pthread_join(t1, NULL);
    pthread_join(t2, NULL);

    return 0;
}

