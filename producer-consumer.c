#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>


/* Static variables */
static pthread_mutex_t list_mutex = PTHREAD_MUTEX_INITIALIZER;
static FILE* pro1_output_p;
static FILE* pro2_output_p;
static FILE* con1_output_p;
static FILE* con2_output_p;


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
void print_all(list_t* list, FILE* file_p);


int main(void)
{
    pro1_output_p = fopen("producer1-output.txt", "w+");
//    pro2_output_p = fopen("producer2-output.txt", "w+");
//    con1_output_p = fopen("consumer1-output.txt", "w+");
//    con2_output_p = fopen("consumer2-output.txt", "w+");

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
    pthread_t prod1, prod2;
    pthread_t con1, con2;

    struct node_t* current_node = list.head;

    while (current_node != NULL) {
        printf("NODE #%d\trandom_val = %d\n", current_node->id, current_node->random_val);
        //fprintf(file_p, "NODE #%d\trandom_val = %d\n", current_node->id, current_node->random_val);
        current_node = current_node->next;
    }

    printf("Calling pthread_create\n");

    pthread_create( &prod1, NULL, (void*) pro_thread1, list_p );
//    pthread_create( &prod2, NULL, (void*) pro_thread2, list_p );
//    pthread_create( &con1, NULL, (void*) con_thread1, list_p );
//    pthread_create( &con2, NULL, (void*) con_thread2, list_p );


    /* Imagine a method to free all nodes */

    pthread_join(prod1, NULL);
    
    fclose(pro1_output_p);
//    fclose(pro2_output_p);
//    fclose(con1_output_p);
//    fclose(con2_output_p);

    return 0;
}

void pro_thread1(list_t* list)
{
    /* allocate a spot for the new node */
    struct node_t* new_node = (struct node_t*) malloc(sizeof(struct node_t));

    /* loop until an odd number is created */
    int odd_number = rand() % 40 + 1;
    while (odd_number % 2 == 0) {
        odd_number = rand() % 40 + 1;
    }

    if (list->current_size > 40) {
        printf("Uh oh. Looks like you went past the max size.\n");
        free(new_node);
        return;
    }

//    while (pthread_mutex_trylock(&list_mutex) != 0) {
//        /* Wait until list can be accessed */
//    }
//
//    printf("mutex is unlocked\n");

    new_node->id = list->current_size;
    list->tail = new_node;
    list->current_size++;

//    pthread_mutex_unlock(&list_mutex);

    printf("Added a node to the end of the linked list\n");

    /* Save current list state to file */
    print_all(list, pro1_output_p);

    return;
}

void pro_thread2(list_t* list)
{
    /* Allocate a new spot for the new node */
    struct node_t* new_node = (struct node_t*) malloc(sizeof(struct node_t));

    /* Loop until an even number is created */
    int even_number = rand() % 40 + 1;
    while (even_number % 2 != 0) {
        even_number = rand() % 40 + 1;
    }

    if (list->current_size > 40) {
        printf("Uh oh. Looks like you went past the max size.\n");
        free(new_node);
        return;
    }
//    else {
//        /* Attempt to access the linked list and add the node to the head */
//        new_node->random_val = even_number;
//        list->head = new_node;
//        list->current_size++;
//    }

    while (pthread_mutex_trylock(&list_mutex) != 0) {
        /* Wait until list can be accessed */
    }

    /* Add to beginning of list somehow */

    /* Save current list state to file */
    print_all(list, pro2_output_p);

    pthread_mutex_unlock(&list_mutex);

    printf("Added a node to the beginning of the linked list\n");

    return;
}

void con_thread1(list_t* list)
{
    /* Delete from the head of the list */

    while (pthread_mutex_trylock(&list_mutex) != 0) {
        /* Wait until list can be accessed */
    }

    /* Remove from the head of the list */
    list->head = list->head->next;
    free(list->head->prev);
    list->head->prev = NULL;

    /* Save current list state to file */
    print_all(list, con1_output_p);

    pthread_mutex_unlock(&list_mutex);

    return;
}

void con_thread2(list_t* list)
{

    return;
}

void print_all(list_t* list, FILE* file_p)
{
    struct node_t* current_node = list->head;

    while (current_node != NULL) {
        printf("NODE #%d\trandom_val = %d\n", current_node->id, current_node->random_val);
        fprintf(file_p, "NODE #%d\trandom_val = %d\n", current_node->id, current_node->random_val);
        current_node = current_node->next;
    }

    return;    
}
