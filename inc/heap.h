#ifndef HEAP_H
#define HEAP_H

#include <stdbool.h>
#include "common.h"

/* ========================================================================= *
 * OPAQUE POINTER (INCAPSULAMENTO)                                           *
 * ========================================================================= */

typedef struct Heap_t *pHeap_t;

/* ========================================================================= *
 * API PUBBLICA                                                              *
 * ========================================================================= */

/**
 * @brief Alloca e inizializza un nuovo Heap generico in memoria.
 * * @param capacity Capacità iniziale dell'array. Se <= 0, applica un fallback a 16.
 * @param cmpFn Puntatore alla funzione di confronto. Definisce le regole di priorità
 * (es. A > B per un Max-Heap). È un parametro architetturalmente critico:
 * se passato NULL, la creazione viene abortita.
 * @param freeFn Puntatore alla funzione per la deallocazione dei dati. Passare NULL
 * per la gestione manuale della memoria da parte del chiamante.
 * * @return pHeap_t Puntatore alla struttura inizializzata.
 * @retval NULL Se la RAM è esaurita durante le allocazioni o se cmpFn è NULL.
 * * @note La complessità temporale della creazione è $O(1)$.
 */
pHeap_t HeapCreate(int capacity, CompareFn_t cmpFn, FreeFn_t freeFn);

/**
 * @brief Inserisce un nuovo elemento nella Coda di Priorità.
 * * L'inserimento avviene in due fasi fisiche:
 * 1. Il dato viene posizionato nell'ultimo slot disponibile dell'array contiguo
 * per mantenere la proprietà di "Albero Binario Completo".
 * 2. Viene invocato l'algoritmo di Sift-Up per far "galleggiare" il dato verso
 * l'alto finché non rispetta la priorità dettata dalla funzione di confronto (cmpFn).
 * * @param myHeap Puntatore all'infrastruttura Heap.
 * @param data    Puntatore (void*) al payload da inserire.
 * @return true   Se l'inserimento e il bilanciamento sono andati a buon fine.
 * @return false  Se la memoria RAM è esaurita o il puntatore all'Heap è nullo.
 * * @note Costo ammortizzato: $O(\log N)$. Nel caso peggiore, se l'array è saturo,
 * scatta una reallocazione geometrica (raddoppio della capacità) in tempo $O(N)$,
 * ma matematicamente il costo spalmato sulle N operazioni resta logaritmico.
 */
bool HeapInsert(pHeap_t myHeap, void *data);

#endif // HEAP_H