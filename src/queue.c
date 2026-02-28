#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "../inc/queue.h"
#include "../inc/common.h"

/* ========================================================================= *
 * MANIFESTO ARCHITETTURALE: LA CODA (QUEUE) E IL RING BUFFER                *
 * ========================================================================= *
 * MECCANICA DI BASE:
 * La Coda è una struttura dati lineare basata sul principio FIFO
 * (First-In, First-Out). Il primo elemento inserito è il primo a uscire.
 * È l'astrazione perfetta per gestire priorità di esecuzione, scheduling di
 * processi OS o flussi di pacchetti di rete.
 * * IL PROBLEMA DELLA "CODA STRISCIANTE" (CREEPING QUEUE):
 * Un'implementazione lineare ingenua porterebbe i cursori di inserimento ed
 * estrazione a scivolare inesorabilmente verso la fine della memoria allocata,
 * costringendo a costose traslazioni fisiche dei dati (O(N)) per recuperare spazio.
 * * IMPLEMENTAZIONE SOTTO IL COFANO (Buffer Circolare / Aritmetica Modulare):
 * Per mantenere Cache Locality (usando un array contiguo) e prestazioni O(1),
 * questo modulo implementa un Ring Buffer. Sfruttando l'operatore matematico
 * modulo (%), gli indici 'head' e 'tail' rimbalzano istantaneamente all'inizio
 * dell'array non appena raggiungono il limite fisico (capacity).
 * Lo spazio viene così riutilizzato all'infinito senza muovere fisicamente i dati.
 * ========================================================================= */

/* ========================================================================= *
 * STRUTTURE DATI E TIPI                                                     *
 * ========================================================================= */

/**
 * @struct Queue_t
 * @brief Rappresenta una coda circolare generica basata su array dinamico.
 */
struct Queue_t
{
    void **data;     // Array dinamico di puntatori generici (void*)
    int capacity;    // Limite fisico massimo dell'array
    int size;        // Cardinalità logica: numero di elementi vivi presenti
    int head;        // Indice di estrazione (Dequeue)
    int tail;        // Indice di inserimento (Enqueue)
    FreeFn_t freeFn; // Contratto di sistema per la deallocazione del payload
};

/* ========================================================================= *
 * FUNZIONI PRIVATE (MOTORE LOGICO)                                          *
 * ========================================================================= */

/**
 * @brief Verifica in tempo O(1) se il buffer circolare ha esaurito lo spazio allocato.
 */
static bool QueueIsFull(pQueue_t myQueue)
{
    return (myQueue->size == myQueue->capacity);
}

/**
 * @brief Verifica in tempo O(1) se la coda è priva di elementi vivi.
 */
static bool QueueIsEmpty(pQueue_t myQueue)
{
    return (myQueue->size == 0);
}

/**
 * @brief Fa avanzare l'indice di inserimento (tail) applicando l'aritmetica modulare.
 */
static void UpdateTail(pQueue_t myQueue)
{
    myQueue->tail = (myQueue->tail + 1) % myQueue->capacity;
}

/**
 * @brief Fa avanzare l'indice di estrazione (head) applicando l'aritmetica modulare.
 */
static void UpdateHead(pQueue_t myQueue)
{
    myQueue->head = (myQueue->head + 1) % myQueue->capacity;
}

/* ========================================================================= *
 * API PUBBLICA (IMPLEMENTAZIONE)                                            *
 * ========================================================================= */

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

    // 5. Inizializzazione della logica circolare
    myQueue->size = 0;
    myQueue->head = 0;
    myQueue->tail = 0;
    myQueue->freeFn = freeFn;

    return myQueue;
}

bool QueueEnqueue(pQueue_t myQueue, void *data)
{
    // Sicurezza e prevenzione sovrascrittura
    if (myQueue == NULL || QueueIsFull(myQueue))
    {
        return false;
    }

    // Inserimento e avanzamento di stato
    myQueue->data[myQueue->tail] = data;
    UpdateTail(myQueue);
    myQueue->size++;

    return true;
}

void *QueueDequeue(pQueue_t myQueue)
{
    // Sicurezza e controllo Underflow
    if (myQueue == NULL || QueueIsEmpty(myQueue))
    {
        return NULL;
    }

    // Salvataggio dell'indice prima del wrap-around
    int tempHead = myQueue->head;

    UpdateHead(myQueue);
    myQueue->size--;

    return myQueue->data[tempHead];
}

void *QueuePeek(pQueue_t myQueue)
{
    // Prevenzione ispezione di dati fantasma
    if (myQueue == NULL || QueueIsEmpty(myQueue))
    {
        return NULL;
    }

    return myQueue->data[myQueue->head];
}

int QueueSize(pQueue_t myQueue)
{
    if (myQueue == NULL)
    {
        return -1;
    }

    return myQueue->size;
}

bool QueueDestroy(pQueue_t myQueue)
{
    if (myQueue == NULL)
    {
        return false;
    }

    // Svuotamento organico tramite interfaccia logica (Decoupling architetturale)
    if (myQueue->freeFn != NULL)
    {
        while (!QueueIsEmpty(myQueue))
        {
            void *payload = QueueDequeue(myQueue);
            myQueue->freeFn(payload);
        }
    }

    // Distruzione dell'infrastruttura fisica
    free(myQueue->data);
    free(myQueue);

    return true;
}