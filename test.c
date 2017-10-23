#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>


/* node_t struct thing */
typedef struct node_t {
    struct node_t* next;
    struct node_t* prev;
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


int main(void)
{
    srand(time(NULL));

    return 0;
}

void pro_thread1(list_t* list)
{
    return;
}

void pro_thread2(list_t* list)
{
    return;
}

void con_thread1(list_t* list)
{
    return;
}

void con_thread2(list_t* list)
{
    return;
}

