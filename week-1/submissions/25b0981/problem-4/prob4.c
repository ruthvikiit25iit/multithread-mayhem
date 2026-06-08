#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define max_seats 800
#define customers 800 // no of threads
#define tickpp 4      // tickets per person as they go family outing

int avl_seats = max_seats;

void *booking(void *arg)
{
    int id = *(int *)arg;
    if (avl_seats >= tickpp){
        for(int i = 0; i <10000000; i++); // Simulate the time taken to process the booking, which can lead to a race condition
        avl_seats -= tickpp;;
    }
    return NULL;
}

int main()
{
    pthread_t threads[customers];
    int id[customers];
    for (int i = 0; i < customers; i++)
    {
        id[i] = i;
        pthread_create(&threads[i], NULL, booking, &id[i]);
    }

    for (int i = 0; i < customers; i++)
    {
        pthread_join(threads[i], NULL); // Wait for all threads to finish before proceeding to print the results
    }

    printf("actual tickets:800 ; net available tickets: %d\n", avl_seats);
}
