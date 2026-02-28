#ifndef QUEUE_H
#define QUEUE_H

#include <stdbool.h>
#include "common.h"

/* ========================================================================= *
 * OPAQUE POINTER (INCAPSULAMENTO)                                           *
 * ========================================================================= */

typedef struct Queue_t *pQueue_t;

/* ========================================================================= *
 * API PUBBLICA                                                              *
 * ========================================================================= */

/**
 * @brief Inizializza una nuova coda circolare (Ring Buffer) in memoria Heap.
 * * Alloca lo spazio per la struttura di controllo e per l'array interno di puntatori.
 * L'architettura a buffer circolare risolve il problema della "coda strisciante" (creeping queue),
 * permettendo il riutilizzo logico degli slot di memoria liberati in testa senza
 * dover traslare fisicamente gli elementi rimanenti all'interno dell'array.
 * * @param capacity Dimensione iniziale dell'array interno. Se il valore fornito è <= 0,
 * il sistema applica un fallback di sicurezza a 8 slot.
 * @param freeFn Puntatore alla funzione custom (definita in common.h) per liberare
 * i dati inseriti. Passare NULL se si desidera gestire la deallocazione
 * del payload manualmente.
 * @return pQueue_t Puntatore alla coda creata, oppure NULL se la RAM è esaurita.
 * * @warning È responsabilità del chiamante invocare una futura funzione QueueDestroy()
 * per liberare l'infrastruttura quando non più necessaria.
 */
pQueue_t QueueCreate(int capacity, FreeFn_t freeFn);

/**
 * @brief Inserisce un nuovo elemento in fondo alla coda (Enqueue).
 * Sfrutta il ring buffer per garantire un inserimento in tempo $O(1)$ senza
 * alcuno spostamento di memoria. Se la coda è satura, l'operazione viene
 * abortita (Early Exit) per preservare l'integrità dello stato interno.
 * @param myQueue Puntatore alla coda bersaglio.
 * @param data    Puntatore (void*) al payload da memorizzare.
 * @return true   Se l'inserimento è andato a buon fine.
 * @return false  Se il puntatore alla coda è NULL o se la coda è già piena.
 */
bool QueueEnqueue(pQueue_t myQueue, void *data);

/**
 * @brief Estrae e restituisce l'elemento in testa alla coda (Dequeue).
 * L'operazione avviene in tempo $O(1)$ modificando esclusivamente gli indici logici,
 * senza spostare fisicamente gli altri elementi in memoria.
 * @param myQueue Puntatore alla coda bersaglio.
 * @return void* Puntatore al payload estratto.
 * @retval NULL Se la coda è vuota (Underflow) o il puntatore è invalido.
 */
void *QueueDequeue(pQueue_t myQueue);

/**
 * @brief Restituisce l'elemento in testa alla coda senza estrarlo (Read-Only).
 * Permette di ispezionare il prossimo elemento che verrebbe restituito da una
 * Dequeue, lasciando inalterato lo stato logico della struttura.
 * @param myQueue Puntatore alla coda da ispezionare.
 * @return void* Puntatore al dato in testa.
 * @retval NULL Se la coda è invalida o logicamente vuota (previene la lettura
 * di dati fantasma nel ring buffer).
 */
void *QueuePeek(pQueue_t myQueue);

/**
 * @brief Restituisce il numero di elementi attualmente vivi nella coda.
 * Risolta in tempo O(1) leggendo direttamente la variabile di stato.
 * @param myQueue Puntatore alla coda.
 * @return int Cardinalità dell'insieme.
 * @retval -1 Se il puntatore passato è invalido (NULL).
 */
int QueueSize(pQueue_t myQueue); 


#endif // QUEUE_H
