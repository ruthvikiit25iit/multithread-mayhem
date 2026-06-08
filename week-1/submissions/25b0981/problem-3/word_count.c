#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

typedef struct {// Structure to hold arguments for each thread to compute its partial sum
    int thread_id;
    long start_index;
    long end_index;     /* exclusive */
} thread_arg_t;

int partial_count = 0;

void *partial_sum(void *arg) {
    thread_arg_t *t = (thread_arg_t *)arg;
    for (long i = t->start_index; i < t->end_index; i++) {
        for (long i = 0; i < 10000000; i++)
        partial_count++;// Increment the shared count for each word processed in this thread's assigned portion of the array
    }

    return NULL;
}

int main(int argc, char *argv[]) {

    // take filename from terminal
    char *filename = argv[1];

    // take thread count from terminal
    int threads = atoi(argv[2]);

    // open file
    FILE *fp = fopen(filename, "r");

    if (fp == NULL) {
        printf("File not found\n");
        return 1;
    }
    // variable to store each word
    char word[1000][100];// Declare a 2D array to store up to 1000 words, each with a maximum length of 99 characters (plus the null terminator)

    int count = 0;// Initialize a counter to keep track of the number of words read from the file
    // read one word at a time
    while (fscanf(fp, "%s", word[count]) == 1) {//why fscanf shows 1 when it successfully reads a word? Because fscanf returns the number of items successfully read, which is 1 in this case since we are reading one word at a time. If it reaches the end of the file or encounters an error, it will return a different value (like EOF or 0), which is why we check for == 1 to continue reading words until we can't anymore.
        //printf("%s\n", word);
        count++;// Increment the word count for each successfully read word
    }

    fclose(fp);// Close the file after reading all the words to free up system resources

    long chunk = count / threads;
    pthread_t thread[threads];

    thread_arg_t args[threads];
    for (int i = 0; i < threads; i++) {
        args[i].thread_id = i;
        args[i].start_index = i * chunk;
        args[i].end_index = (i == threads - 1) ? count : (i + 1) * chunk;
        pthread_create(&thread[i], NULL, partial_sum, &args[i]);// Create a thread for each portion of the array, passing the appropriate start and end indices to compute the partial sum
    }
    for (int i = 0; i < threads; i++) {
        pthread_join(thread[i], NULL);// Wait for all threads to finish their computation before proceeding to print the results
    }
    printf("Total words counted: %d\ntotal words actual: %d\n", partial_count, count);
    return 0;
}

