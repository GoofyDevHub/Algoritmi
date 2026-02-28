#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "../inc/common.h"
#include "../inc/stack.h"

/* ========================================================================= *
 * MANIFESTO ARCHITETTURALE: LO STACK (PILA) E IL PRINCIPIO LIFO             *
 * ========================================================================= *
 * MECCANICA DI BASE:
 * Lo Stack è una struttura dati lineare basata sul principio LIFO
 * (Last-In, First-Out). L'ultimo elemento inserito è il primo a essere estratto.
 * Immagina una pila di piatti: puoi aggiungere un piatto solo in cima (Push)
 * e puoi togliere solo il piatto che si trova in cima (Pop).
 * * IMPLEMENTAZIONE SOTTO IL COFANO (Array Dinamico):
 * Invece di usare nodi concatenati sparsi nella memoria (che causano Cache Miss),
 * questo stack usa un blocco di memoria contiguo (Array).
 * - La variabile 'top' agisce come un cursore: indica l'indice del prossimo
 * slot libero nell'array.
 * - Quando l'array si riempie, la sua capacità viene raddoppiata (Crescita
 * Geometrica). Questo garantisce che il costo computazionale dell'inserimento
 * sia O(1) ammortizzato.
 * * DESIGN OPACO E GENERICO:
 * Lo stack non contiene i dati reali, ma solo i loro indirizzi di memoria (void**).
 * Questo lo rende "agnostico": può impilare interi, stringhe o strutture
 * complesse senza che il suo codice debba essere modificato.
 * ========================================================================= */

/* ========================================================================= *
 * STRUTTURE DATI E TIPI                                                     *
 * ========================================================================= */

/**
 * @struct Stack_t
 * @brief Rappresenta uno stack generico basato su array dinamico.
 * * L'utilizzo di void** permette allo stack di immagazzinare indirizzi
 * di memoria di qualsiasi tipo di dato strutturato, disaccoppiando
 * la logica della struttura dati dal payload effettivo.
 */
struct Stack_t
{
    void **data;     // Array dinamico di puntatori generici (void*)
    int capacity;    // Numero massimo di elementi attualmente ospitabili
    int top;         // Indice del prossimo slot libero (coincide con la size)
    FreeFn_t freeFn; // Puntatore a funzione per la deallocazione automatica del payload
};

/* ========================================================================= *
 * IMPLEMENTAZIONE DELLE FUNZIONI                                            *
 * ========================================================================= */

pStack_t StackCreate(int initialCapacity, FreeFn_t freeFn)
{

    pStack_t myStack = (pStack_t)malloc(sizeof(struct Stack_t));
    if (myStack == NULL)
        return NULL;

    myStack->capacity = (initialCapacity > 0) ? initialCapacity : 8;

    // sizeof(void*) garantisce 8 byte su sistemi a 64-bit per ogni slot
    myStack->data = (void **)malloc(myStack->capacity * sizeof(void *));

    if (myStack->data == NULL)
    {
        free(myStack); // Evita memory leak del guscio
        return NULL;
    }

    myStack->top = 0;
    myStack->freeFn = freeFn;

    return myStack;
}

bool StackPush(pStack_t myStack, void *data)
{

    if (myStack == NULL)
    {
        return false;
    }

    // Controllo saturazione
    if (myStack->capacity == myStack->top)
    {
        int newCapacity = myStack->capacity * 2;

        // Uso un puntatore temporaneo per proteggere i dati originali in caso di fallimento
        void **tempData = (void **)realloc(myStack->data, newCapacity * sizeof(void *));

        if (tempData == NULL)
        {
            return false;
        }

        myStack->data = tempData;
        myStack->capacity = newCapacity;
    }

    myStack->data[myStack->top] = data;
    myStack->top++;

    return true;
}

void *StackPop(pStack_t myStack)
{

    if (myStack == NULL)
    {
        return NULL;
    }

    if (myStack->top == 0)
    {
        return NULL;
    }

    // Decremento logico e ritorno simultaneo
    return myStack->data[--myStack->top];
}

void *StackPeek(pStack_t myStack)
{

    if (myStack == NULL)
    {
        return NULL;
    }

    if (myStack->top == 0)
    {
        return NULL;
    }

    return myStack->data[myStack->top - 1];
}

int StackSize(pStack_t myStack)
{

    if (myStack == NULL)
    {
        return -1;
    }

    return myStack->top;
}

bool StackDestroy(pStack_t myStack)
{

    if (myStack == NULL)
    {
        return false;
    }

    // 1. Pulizia opzionale e automatica dei dati inseriti dall'utente
    if (myStack->freeFn != NULL)
    {
        for (int i = 0; i < myStack->top; i++)
        {
            myStack->freeFn(myStack->data[i]);
        }
    }

    // 2. Distruzione infrastruttura interna
    free(myStack->data);
    free(myStack);

    return true;
}