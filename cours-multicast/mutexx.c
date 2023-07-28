#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

#define NUM_THREADS 4
#define NUM_ITERATIONS 1000000

int counter = 0;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
int t;

void *increment_counter(void *thread_id)
{
    int i;
    for (i = 0; i < NUM_ITERATIONS; i++)
    {
        pthread_mutex_lock(&mutex);
        counter++;
        pthread_mutex_unlock(&mutex);
    }
    pthread_exit(NULL);
}

int main(int argc, char *argv[])
{
    pthread_t threads[NUM_THREADS];
    int rc, t;

    // Initialize mutex
    pthread_mutex_init(&mutex, NULL);

    // Create threads
    for (t = 0; t < NUM_THREADS; t++)
    {
        rc = pthread_create(&threads[t], NULL, increment_counter, (void *)t);
        if (rc)
        {
            fprintf(stderr, "Error creating thread %d\n", t);
            exit(EXIT_FAILURE);
        }
    }

    // Wait for threads to finish
    for (t = 0; t < NUM_THREADS; t++)
    {
        rc = pthread_join(threads[t], NULL);
        if (rc)
        {
            fprintf(stderr, "Error joining thread %d\n", t);
            exit(EXIT_FAILURE);
        }
    }

    // Print final value of counter
    printf("Final value of counter: %d\n", counter);

    // Clean up mutex
    pthread_mutex_destroy(&mutex);

    pthread_exit(NULL);
}
