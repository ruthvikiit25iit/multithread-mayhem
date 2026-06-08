#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

#define ITEMS_TO_PRODUCE  1000

int BUFFER_CAPACITY;
int count, in, out;
int *buffer;

pthread_mutex_t buffer_lock = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t  not_full  = PTHREAD_COND_INITIALIZER;
pthread_cond_t  not_empty = PTHREAD_COND_INITIALIZER;

void buffer_init(int capacity)
{
    if (capacity <= 0) {
        fprintf(stderr, "Invalid buffer capacity\n");
        exit(EXIT_FAILURE);
    }

    buffer = malloc(capacity * sizeof(int));
    if (buffer == NULL) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }

    BUFFER_CAPACITY = capacity;// set the global capacity variable
    count = 0;
    in = 0;
    out = 0;
}

void buffer_destroy(void)
{
    free(buffer);
    buffer = NULL;
    pthread_mutex_destroy(&buffer_lock);
    pthread_cond_destroy(&not_full);
    pthread_cond_destroy(&not_empty);
}

int buffer_get(void){
    int item = buffer[out];
    out = (out + 1) % BUFFER_CAPACITY;
    count--;
    printf("[consumer] consumed %d  (buffer now has %d)\n", item, count);
    return item;
}

void buffer_put(int item){
    buffer[in] = item;
    in = (in + 1) % BUFFER_CAPACITY;
    count++;
}

void* producer (void* arg){
    (void)arg;
    int id = *(int*)arg;
    for (int i = 0; i < ITEMS_TO_PRODUCE; i++) {
        pthread_mutex_lock(&buffer_lock);
        while (count == BUFFER_CAPACITY) {
            printf("[producer] buffer full, waiting...\n");
            pthread_cond_wait(&not_full, &buffer_lock);
        }
        int item = i+id*1000;
        buffer_put(item);
        printf("[producer %d] produced %d  (buffer now has %d)\n", id, i+id*1000, count);
        pthread_cond_broadcast(&not_empty);
        pthread_mutex_unlock(&buffer_lock);
    }
    return NULL;
}

void *consumer(void *arg) {
    (void)arg;
    for (int i = 0; i < (ITEMS_TO_PRODUCE*3)/2; i++) {
        pthread_mutex_lock(&buffer_lock);
        while (count == 0) {
            printf("[consumer] buffer empty, waiting...\n");
            pthread_cond_wait(&not_empty, &buffer_lock);
        }
        buffer_get();
        pthread_cond_broadcast(&not_full);
        pthread_mutex_unlock(&buffer_lock);
    }
    return NULL;
}

int main(void) {
    buffer_init(20);
    pthread_t p[3],c1,c2;
    int id[3];
    for (int i=0;i<3;i++){
    id[i]=i;
    pthread_create(&p[i], NULL, producer, &id[i]);
    }
    pthread_create(&c1, NULL, consumer, NULL);
    pthread_create(&c2, NULL, consumer, NULL);
    for (int i=0;i<3;i++){
        pthread_join(p[i],NULL);
    }
    pthread_join(c1, NULL);
    pthread_join(c2, NULL);

    printf("\nAll %d items produced and consumed. No items lost.\n",
           ITEMS_TO_PRODUCE*3);

    buffer_destroy();
    return 0;
}