#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>


/* Static variables */
static pthread_mutex_t list_mutex = PTHREAD_MUTEX_INITIALIZER;

/* node_t struct thing */
typedef struct node_t {
    struct node_t* next;
    struct node_t* prev;
    int id;
    int random_val;
} node_t;

typedef struct list_t {
    struct node_t* head;
    struct node_t* tail;
    int max_size;
    int current_size;
} list_t;


/* function prototypes */
void pro_thread1(list_t* list);
void pro_thread2(list_t* list);
void con_thread1(list_t* list);
void con_thread2(list_t* list);
void print_all(list_t* list, char* thread_name);
void remove_all(list_t* list);


int main(void)
{
    srand(time(NULL));

    /* Initialize the list with the 3 nodes and their values */
    struct node_t* node1 = (struct node_t*) malloc(sizeof(struct node_t));
    node1->random_val = rand() % 40 + 1;
    node1->id = 1;
    struct node_t* node2 = (struct node_t*) malloc(sizeof(struct node_t));
    node2->random_val = rand() % 40 + 1;
    node2->id = 2;
    struct node_t* node3 = (struct node_t*) malloc(sizeof(struct node_t));
    node3->random_val = rand() % 40 + 1;
    node3->id = 3;

    node1->next = node2;
    node1->prev = NULL;
    node2->next = node3;
    node2->prev = node1;
    node3->next = NULL;
    node3->prev = node2;

    printf("All nodes created\n");

    /* Create list_t list */
    struct list_t list = {node1, node3, 40, 3};
           list_t* list_p = &list;

    /* Spin the 4 threads - producers 1 & 2, consumers 1 & 2 */
    pthread_t prod1; 
    pthread_t prod2;
    pthread_t con1;
    pthread_t con2;

    /* Print all information prior to modifications made by producers 1 & 2 */
    printf("INITIAL CONTENTS:\n\t");
    struct node_t* current_node = list.head;
    while (current_node->next != NULL) {
        printf("%d, ", current_node->random_val);
        current_node = current_node->next;
    }
    printf("%d\n", current_node->random_val);

    printf("Calling pthread_create\n");

    pthread_create( &prod1, NULL, (void*) pro_thread1, list_p );
    pthread_create( &prod2, NULL, (void*) pro_thread2, list_p );
    pthread_create( &con1, NULL, (void*) con_thread1, list_p );
    pthread_create( &con2, NULL, (void*) con_thread2, list_p );


    /* Imagine a method to free all nodes */

    pthread_join(prod1, NULL);
    pthread_join(prod2, NULL);
    pthread_join(con1, NULL);
    pthread_join(con2, NULL);
    
    return 0;
}

void pro_thread1(list_t* list)
{
    /* Loop until the buffer gets filled. */
    for( ; ; ) {

        /* loop until an odd number is created */
        int odd_number = rand() % 40 + 1;
        while (odd_number % 2 == 0) {
            odd_number = rand() % 40 + 1;
        }

        /* allocate a spot for the new node */
        struct node_t* new_node = (struct node_t*) malloc(sizeof(struct node_t));

        /* Initialize the new_node as much as possible before locking up the list, minimize time in critical region */
        new_node->random_val    = odd_number;
        new_node->next          = NULL;

        while (pthread_mutex_trylock(&list_mutex) != 0) {
            /* ... */
        }

        /* Check if the loop can be terminated */
        if (list->current_size >= 40) {
            printf("Uh oh. Looks like you went past the max size.\n");
            free(new_node);
            pthread_mutex_unlock(&list_mutex);
            return;
        }

        /* Output buffer contents */
        print_all(list, "THREAD - pro1_output_p contents prior");

        list->tail->next    = new_node;
        new_node->id        = list->current_size + 1;
        new_node->prev      = list->tail;
        list->tail          = new_node;
        list->current_size++;

        /* Output buffer contents */
        print_all(list, "THREAD - pro1_output_p contents after");

        pthread_mutex_unlock(&list_mutex);

    }

    return;
}

void pro_thread2(list_t* list)
{
    for ( ; ; ) {

        /* Allocate a new spot for the new node */
        struct node_t* new_node = (struct node_t*) malloc(sizeof(struct node_t));

        /* Loop until an even number is created */
        int even_number = rand() % 40 + 1;
        while (even_number % 2 != 0) {
            even_number = rand() % 40 + 1;
        }

        /* Initialize the new_node as much as possible before locking up the list, minimize time in critical region */
        new_node->random_val = even_number;
        new_node->prev = NULL;

        while (pthread_mutex_trylock(&list_mutex) != 0) {
            /* Wait until list can be accessed */
        }

        /* Check if the loop can be terminated */
        if (list->current_size >= 40) {
            printf("Uh oh. Looks like you went past the max size.\n");
            free(new_node);
            pthread_mutex_unlock(&list_mutex);
            return;
        }

        /* Output buffer contents */
        print_all(list, "THREAD - pro2_output_p contents prior");

        /* Add to beginning of list somehow */
        new_node->next      = list->head;
        new_node->id        = list->current_size;
        list->head->prev    = new_node;
        list->head          = new_node;
        list->current_size++;

        /* Output buffer contents */
        print_all(list, "THREAD - pro2_output_p contents after");

        pthread_mutex_unlock(&list_mutex);

    }

    return;
}

void con_thread1(list_t* list)
{
    for ( ; ; ) {

        /* Wait for access to list */ 
        while (pthread_mutex_trylock(&list_mutex) != 0) {
            /* Keep checking */
        }

        /* If the list is empty, then exit. There's nothing for you to do. */
        if (list->current_size == 0) {
            printf("Uh oh. Looks like the list is empty.\n");
            pthread_mutex_unlock(&list_mutex);
            return;
        }

        /* If the first node has an even value, try again and check for an odd. */
        if (list->head->random_val % 2 == 0) {
            pthread_mutex_unlock(&list_mutex);
            continue;
        }

        /* Output buffer contents */
        print_all(list, "THREAD - con1_thread1 contents prior");

        /* Upon exiting the loop, iterator_node has an odd random value */
        struct node_t* node_to_remove = list->head; 
        list->head = list->head->next;
        list->head->prev = NULL;
        list->current_size--;
        free(node_to_remove);

        print_all(list, "THREAD - con1_thread1 contents after");

        pthread_mutex_unlock(&list_mutex);

    }

    return;
}

void con_thread2(list_t* list)
{
    for ( ; ; ) {

        /* Wait for access to list */ 
        while (pthread_mutex_trylock(&list_mutex) != 0) {
            /* Keep checking */
        }

        /* If the list is empty, then exit. There's nothing for you to do. */
        if (list->current_size == 0) {
            printf("Uh oh. Looks like the list is empty.\n");
            pthread_mutex_unlock(&list_mutex);
            return;
        }

        /* If the first node has an even value, try again and check for an odd. */
        if (list->head->random_val % 2 != 0) {
            pthread_mutex_unlock(&list_mutex);
            continue;
        }

        /* Output buffer contents */
        print_all(list, "THREAD - con2_thread1 contents prior");

        /* Upon exiting the loop, iterator_node has an odd random value */
        struct node_t* node_to_remove = list->head; 
        list->head = list->head->next;
        list->head->prev = NULL;
        list->current_size--;
        free(node_to_remove);

        print_all(list, "THREAD - con2_thread1 contents after");

        pthread_mutex_unlock(&list_mutex);

    }

    return;
}

void print_all(list_t* list, char* thread_name)
{
    printf("\n%s:\n\t", thread_name);

    struct node_t* current_node = list->head;

    while (current_node->next != NULL) {
        printf("%d, ", current_node->random_val);
//        fprintf(file_p, "NODE #%d \trandom_val = %d\n", current_node->id, current_node->random_val);
        current_node = current_node->next;
    }

    printf("%d\n", current_node->random_val);

    return;    
}

void remove_all(list_t* list)
{
//    while c
    return;
}
