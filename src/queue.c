/*
typedef struct Queue_t *Queue;
Queue queue_create(FreeFn free_fn);
void queue_enqueue(Queue q, void *data);
void *queue_dequeue(Queue q);
int queue_size(Queue q);
void queue_destroy(Queue q);
*/

#include <stdio.h>
#include <stdlib.h>
#include "../inc/queue.h"
#include "../inc/common.h"

/*array dinamico che contiente puntatori per i dati 
 *capacità dell'array
 *top (prima casella libera)
 *bottom (prima casella da liberare)
 *funzione free per liberare la memoria 
 **/
struct Queue_t
{
    void **data;
    int capacity;
    int size; // Quanti elementi ci sono effettivamente dentro
    int head; // Indice della prima casella da liberare (estrazione)
    int tail; // Indice della prima casella libera (inserimento)
    FreeFn_t freeFn;
};

pQueue_t QueueCreate(int capacity, FreeFn_t freeFn)
{
    // 1. Allocazione del guscio di controllo
    pQueue_t myQueue = (pQueue_t)malloc(sizeof(struct Queue_t));
    if (myQueue == NULL)
    {
        return NULL;
    }
    // 2. Normalizzazione della capacità richiesta
    myQueue->capacity = (capacity > 0) ? capacity : 8;
    
    // 3. Allocazione dell'array di puntatori generici
    myQueue->data = (void **)malloc(myQueue->capacity * sizeof(void *));

    // 4. Sicurezza contro Memory Leak a cascata
    if (myQueue->data == NULL)
    {
        free(myQueue); // Distruggo il guscio orfano
        return NULL;
    }

    // 5. Inizializzazione della logica circolare (O(1))
    // - size: traccia esattamente quanti elementi vivi ci sono, slegandosi dagli indici.
    // - head: l'indice da cui verrà estratto il prossimo elemento (Dequeue).
    // - tail: l'indice in cui verrà inserito il prossimo elemento (Enqueue).
    myQueue->size = 0;
    myQueue->head = 0;
    myQueue->tail = 0;

    // 6. Salvataggio del contratto di sistema per la pulizia della memoria
    myQueue->freeFn = freeFn;

    return myQueue;
}