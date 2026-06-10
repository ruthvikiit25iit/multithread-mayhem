/*
 * sem_buffer.c
 *
 * Semaphore-based bounded buffer with multiple producers and consumers.
 *
 * Compile:
 *   gcc -Wall -Wextra -pthread -std=c11 sem_buffer.c -o sem_buffer
 *
 * Run:
 *   ./sem_buffer          // default capacity 8
 *   ./sem_buffer 3        // tiny buffer
 *   ./sem_buffer 64       // large buffer
 */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <limits.h>

#define DEFAULT_CAPACITY     8
#define NUM_PRODUCERS        4
#define NUM_CONSUMERS        3
#define ITEMS_PER_PRODUCER   100000
#define POISON_PILL          INT_MIN

typedef struct {
    int *data;
    int capacity;
    int in;
    int out;
    sem_t empty;                 /* free slots */
    sem_t full;                  /* filled slots */
    pthread_mutex_t mutex;       /* protects buffer state */
} buffer_t;

static buffer_t gbuf;

static void buffer_init(int capacity)
{
    gbuf.capacity = capacity;
    gbuf.data = malloc(sizeof(int) * (size_t)capacity);
    gbuf.in = 0;
    gbuf.out = 0;

    if (!gbuf.data) {
        fprintf(stderr, "buffer allocation failed\n");
        exit(1);
    }

    if (sem_init(&gbuf.empty, 0, (unsigned int)capacity) != 0) {
        fprintf(stderr, "sem_init(empty) failed\n");
        exit(1);
    }

    if (sem_init(&gbuf.full, 0, 0) != 0) {
        fprintf(stderr, "sem_init(full) failed\n");
        exit(1);
    }

    if (pthread_mutex_init(&gbuf.mutex, NULL) != 0) {
        fprintf(stderr, "pthread_mutex_init failed\n");
        exit(1);
    }
}

static void buffer_put(int item)
{
    sem_wait(&gbuf.empty);

    pthread_mutex_lock(&gbuf.mutex);
    gbuf.data[gbuf.in] = item;
    gbuf.in = (gbuf.in + 1) % gbuf.capacity;
    pthread_mutex_unlock(&gbuf.mutex);

    sem_post(&gbuf.full);
}

static int buffer_get(void)
{
    sem_wait(&gbuf.full);

    pthread_mutex_lock(&gbuf.mutex);
    int item = gbuf.data[gbuf.out];
    gbuf.out = (gbuf.out + 1) % gbuf.capacity;
    pthread_mutex_unlock(&gbuf.mutex);

    sem_post(&gbuf.empty);

    return item;
}

static void buffer_destroy(void)
{
    sem_destroy(&gbuf.empty);
    sem_destroy(&gbuf.full);
    pthread_mutex_destroy(&gbuf.mutex);
    free(gbuf.data);
}

typedef struct {
    int producer_id;
} producer_arg_t;

typedef struct {
    long long sum;
    long count;
} consumer_result_t;

typedef struct {
    consumer_result_t *result;
} consumer_arg_t;

static void *producer_thread(void *arg)
{
    producer_arg_t *a = (producer_arg_t *)arg;
    long long base = (long long)a->producer_id * ITEMS_PER_PRODUCER;

    for (int i = 0; i < ITEMS_PER_PRODUCER; i++) {
        /*
         * Unique positive values so the poison pill (INT_MIN) is safe.
         * Each producer generates:
         *   base+1, base+2, ... base+ITEMS_PER_PRODUCER
         */
        int item = (int)(base + i + 1);
        buffer_put(item);
    }

    return NULL;
}

static void *consumer_thread(void *arg)
{
    consumer_arg_t *a = (consumer_arg_t *)arg;
    long long sum = 0;
    long count = 0;

    for (;;) {
        int item = buffer_get();
        if (item == POISON_PILL) {
            break;
        }
        sum += item;
        count++;
    }

    a->result->sum = sum;
    a->result->count = count;
    return NULL;
}

static long long expected_sum(void)
{
    long long total = 0;

    for (int p = 0; p < NUM_PRODUCERS; p++) {
        long long n = ITEMS_PER_PRODUCER;
        long long base = (long long)p * ITEMS_PER_PRODUCER;

        /*
         * Sum of arithmetic sequence:
         * (base+1) + (base+2) + ... + (base+n)
         */
        total += n * (2 * base + n + 1) / 2;
    }

    return total;
}

int main(int argc, char **argv)
{
    int capacity = DEFAULT_CAPACITY;

    if (argc >= 2) {
        capacity = atoi(argv[1]);
        if (capacity <= 0) {
            capacity = DEFAULT_CAPACITY;
        }
    }

    buffer_init(capacity);

    pthread_t producers[NUM_PRODUCERS];
    pthread_t consumers[NUM_CONSUMERS];

    producer_arg_t pargs[NUM_PRODUCERS];
    consumer_arg_t cargs[NUM_CONSUMERS];
    consumer_result_t cresults[NUM_CONSUMERS];

    for (int i = 0; i < NUM_CONSUMERS; i++) {
        cargs[i].result = &cresults[i];
        cresults[i].sum = 0;
        cresults[i].count = 0;
        if (pthread_create(&consumers[i], NULL, consumer_thread, &cargs[i]) != 0) {
            fprintf(stderr, "pthread_create failed for consumer %d\n", i);
            buffer_destroy();
            return 1;
        }
    }

    for (int i = 0; i < NUM_PRODUCERS; i++) {
        pargs[i].producer_id = i;
        if (pthread_create(&producers[i], NULL, producer_thread, &pargs[i]) != 0) {
            fprintf(stderr, "pthread_create failed for producer %d\n", i);
            buffer_destroy();
            return 1;
        }
    }

    for (int i = 0; i < NUM_PRODUCERS; i++) {
        pthread_join(producers[i], NULL);
    }

    /*
     * Send one poison pill per consumer after all real production is done.
     * The buffer is FIFO, so poison pills will be consumed only after all
     * real items already in the queue.
     */
    for (int i = 0; i < NUM_CONSUMERS; i++) {
        buffer_put(POISON_PILL);
    }

    long long total_consumed_sum = 0;
    long long total_consumed_count = 0;

    for (int i = 0; i < NUM_CONSUMERS; i++) {
        pthread_join(consumers[i], NULL);
        total_consumed_sum += cresults[i].sum;
        total_consumed_count += cresults[i].count;

        printf("Consumer %d: count=%ld, sum=%lld\n",
               i, cresults[i].count, cresults[i].sum);
    }

    long long expected_count = (long long)NUM_PRODUCERS * ITEMS_PER_PRODUCER;
    long long exp_sum = expected_sum();

    printf("\nFinal check\n");
    printf("Produced count : %lld\n", expected_count);
    printf("Consumed count : %lld\n", total_consumed_count);
    printf("Produced sum   : %lld\n", exp_sum);
    printf("Consumed sum   : %lld\n", total_consumed_sum);

    if (total_consumed_count == expected_count && total_consumed_sum == exp_sum) {
        printf("Result: PASS\n");
    } else {
        printf("Result: FAIL\n");
    }

    buffer_destroy();
    return 0;
}