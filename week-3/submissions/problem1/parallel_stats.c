/*
 * parallel_stats.c
 *
 * Parallel statistics over a large array using pthreads.
 * Computes sum, min, max, and average.
 *
 * Compile:
 *   gcc -Wall -Wextra -pthread -std=c11 parallel_stats.c -o parallel_stats
 *
 * Run:
 *   ./parallel_stats            // default 4 threads
 *   ./parallel_stats 8          // use 8 threads
 */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <limits.h>

#define ARRAY_SIZE      10000000
#define DEFAULT_THREADS 4

typedef struct {
    long long sum;
    int min;
    int max;
    size_t count;
} worker_result_t;

typedef struct {
    const int *data;
    size_t start;
    size_t end;
    worker_result_t *out;
} worker_arg_t;

static void *worker(void *arg)
{
    worker_arg_t *a = (worker_arg_t *)arg;

    long long sum = 0;
    int mn = INT_MAX;
    int mx = INT_MIN;

    for (size_t i = a->start; i < a->end; i++) {
        int v = a->data[i];
        sum += v;
        if (v < mn) mn = v;
        if (v > mx) mx = v;
    }

    a->out->sum = sum;
    a->out->min = mn;
    a->out->max = mx;
    a->out->count = a->end - a->start;

    return NULL;
}

static int parse_threads(int argc, char **argv)
{
    if (argc < 2) return DEFAULT_THREADS;

    char *end = NULL;
    long n = strtol(argv[1], &end, 10);

    if (end == argv[1] || *end != '\0' || n <= 0) {
        return DEFAULT_THREADS;
    }

    if (n > (long)ARRAY_SIZE) {
        n = ARRAY_SIZE;
    }

    return (int)n;
}

int main(int argc, char **argv)
{
    int thread_count = parse_threads(argc, argv);

    int *data = malloc(sizeof(int) * ARRAY_SIZE);
    pthread_t *threads = malloc(sizeof(pthread_t) * thread_count);
    worker_arg_t *args = malloc(sizeof(worker_arg_t) * thread_count);
    worker_result_t *results = calloc((size_t)thread_count, sizeof(worker_result_t));

    if (!data || !threads || !args || !results) {
        fprintf(stderr, "Allocation failed\n");
        free(data);
        free(threads);
        free(args);
        free(results);
        return 1;
    }

    /*
     * Deterministic data so the answer is stable across runs.
     * Values range from -500 to 499.
     */
    for (size_t i = 0; i < ARRAY_SIZE; i++) {
        data[i] = (int)(((i * 37) % 1000) - 500);
    }

    size_t chunk = ARRAY_SIZE / (size_t)thread_count;

    for (int i = 0; i < thread_count; i++) {
        args[i].data = data;
        args[i].start = (size_t)i * chunk;
        args[i].end = (i == thread_count - 1) ? ARRAY_SIZE : (size_t)(i + 1) * chunk;
        args[i].out = &results[i];

        if (pthread_create(&threads[i], NULL, worker, &args[i]) != 0) {
            fprintf(stderr, "pthread_create failed for thread %d\n", i);
            free(data);
            free(threads);
            free(args);
            free(results);
            return 1;
        }
    }

    long long total_sum = 0;
    int global_min = INT_MAX;
    int global_max = INT_MIN;
    size_t total_count = 0;

    for (int i = 0; i < thread_count; i++) {
        pthread_join(threads[i], NULL);

        total_sum += results[i].sum;
        if (results[i].min < global_min) global_min = results[i].min;
        if (results[i].max > global_max) global_max = results[i].max;
        total_count += results[i].count;

        printf("Thread %d: slice [%zu, %zu) -> sum=%lld, min=%d, max=%d\n",
               i, args[i].start, args[i].end,
               results[i].sum, results[i].min, results[i].max);
    }

    double average = (double)total_sum / (double)total_count;

    printf("\nFinal statistics\n");
    printf("Count   : %zu\n", total_count);
    printf("Sum     : %lld\n", total_sum);
    printf("Min     : %d\n", global_min);
    printf("Max     : %d\n", global_max);
    printf("Average : %.6f\n", average);

    free(data);
    free(threads);
    free(args);
    free(results);

    return 0;
}