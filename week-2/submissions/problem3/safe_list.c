#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define NUM_THREADS 8
#define OPERATIONS 1000
#define RANGE 100

typedef struct Node {
    int value;
    struct Node *next;
} Node;

Node *head = NULL;
pthread_mutex_t lock;

void list_init(void)
{
    head = NULL;
    pthread_mutex_init(&lock, NULL);
}

void list_insert(int value)
{
    pthread_mutex_lock(&lock);

    Node *newNode = (Node *)malloc(sizeof(Node));
    newNode->value = value;
    newNode->next = head;
    head = newNode;

    pthread_mutex_unlock(&lock);
}

int list_contains(int value)
{
    pthread_mutex_lock(&lock);

    Node *temp = head;

    while (temp != NULL)
    {
        if (temp->value == value)
        {
            pthread_mutex_unlock(&lock);
            return 1;
        }

        temp = temp->next;
    }

    pthread_mutex_unlock(&lock);
    return 0;
}

void list_remove(int value)
{
    pthread_mutex_lock(&lock);

    Node *temp = head;
    Node *prev = NULL;

    while (temp != NULL)
    {
        if (temp->value == value)
        {
            if (prev == NULL)
            {
                head = temp->next;
            }
            else
            {
                prev->next = temp->next;
            }

            free(temp);
            break;
        }

        prev = temp;
        temp = temp->next;
    }

    pthread_mutex_unlock(&lock);
}

void list_destroy(void)
{
    pthread_mutex_lock(&lock);

    Node *temp = head;

    while (temp != NULL)
    {
        Node *next = temp->next;
        free(temp);
        temp = next;
    }

    head = NULL;

    pthread_mutex_unlock(&lock);
    pthread_mutex_destroy(&lock);
}

void *worker(void *arg)
{
    int i;

    for (i = 0; i < OPERATIONS; i++)
    {
        int op = rand() % 3;
        int value = rand() % RANGE;

        if (op == 0)
            list_insert(value);

        else if (op == 1)
            list_contains(value);

        else
            list_remove(value);
    }

    return NULL;
}

int main()
{
    pthread_t threads[NUM_THREADS];
    int i;

    srand(1);

    list_init();

    for (i = 0; i < NUM_THREADS; i++)
        pthread_create(&threads[i], NULL, worker, NULL);

    for (i = 0; i < NUM_THREADS; i++)
        pthread_join(threads[i], NULL);

    list_insert(12345);

    printf("Contains 12345 = %d\n", list_contains(12345));
    printf("Contains 99999 = %d\n", list_contains(99999));

    list_destroy();

    return 0;
}