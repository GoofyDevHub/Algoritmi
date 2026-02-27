#ifndef QUEUE_H
#define QUEUE_H 

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

#endif // QUEUE_H
