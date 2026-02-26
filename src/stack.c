/*
primitive da implementare
Stack stack_create(FreeFn free_fn);
void stack_push(Stack s, void* data);
void* stack_pop(Stack s);
void* stack_peek(Stack s);
int stack_size(Stack s);
void stack_destroy(Stack s);

*/

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

/*struttura per descrivere uno stack implementato da un array dinamico*/
typedef struct{
    void **data; //array di puntatori
    int capacity; //capacita 
    int top;     //top dello stack
} Stack_t; 
/*tipo puntatore alla stuttura stack*/
typedef Stack_t* pStack_t; 

/*crea uno stack in memoria ritornando il suo indirizzo, 
se ci sono stati problemi ritorna NULL*/
pStack_t stack_create(int initialCapacity){
   
    pStack_t myStack = (pStack_t)malloc(sizeof(Stack_t)); 
    if(myStack == NULL) return NULL; 

    myStack->capacity = (initialCapacity > 0) ? initialCapacity : 8); 

    myStack->data = (void**)malloc(myStack->capacity * sizeof(void*)); 
    if(myStack->data == NULL){
        free(myStack); 
        return NULL; 
    }

    myStack->top = 0; 
    
    return myStack;  
}

/*push nello stack un nuovo elemento, controllando che ci sia spazio 
nel caso in cui non ci sia spazio allora viene raddoppiata la dimensione dello stack
se questa operazione fallisce ritorno NULL*/
bool stack_push(pStack_t myStack, void* data){

    if (myStack == NULL) {
        return false; 
    }

    if((myStack->capacity - 1) == myStack->top){
        myStack->capacity *= 2; 
        myStack->data = (void**)realloc(myStack->data, myStack->capacity  * sizeof(void*)); 
        if(myStack->data == NULL){
            //free della memoria 
            return NULL; 
        }
    }

    myStack->data[myStack->top++] = data; 
    return myStack;      
}



