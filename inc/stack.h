#ifndef STACK_H
#define STACK_H

#include <stdbool.h>
#include "common.h"

/* ========================================================================= *
 * OPAQUE POINTER (INCAPSULAMENTO)                                           *
 * ========================================================================= */
/**
 * @struct Stack_t
 * @brief Opaque pointer per lo stack generico. Nasconde l'implementazione
 * interna (array dinamico) all'utente finale.
 */
typedef struct Stack_t *pStack_t;

/* ========================================================================= *
 * API PUBBLICA                                                              *
 * ========================================================================= */

/**
 * @brief Inizializza un nuovo stack in memoria (Heap).
 * @param initialCapacity Dimensione iniziale dell'array interno (minimo 8).
 * @param freeFn Puntatore alla funzione custom per liberare i dati inseriti.
 * Passare NULL se si desidera gestire la memoria manualmente.
 * @return pStack_t Puntatore allo stack creato, oppure NULL se la RAM è esaurita.
 */
pStack_t StackCreate(int initialCapacity, FreeFn_t freeFn);

/**
 * @brief Inserisce un nuovo elemento in cima allo stack (Push).
 * L'operazione avviene in tempo O(1) ammortizzato.
 * @param myStack Puntatore allo stack di destinazione.
 * @param data    Puntatore (void*) al dato effettivo da immagazzinare.
 * @return true   Se l'inserimento è andato a buon fine, false in caso di errore.
 */
bool StackPush(pStack_t myStack, void *data);

/**
 * @brief Estrae e restituisce l'elemento in cima allo stack (Pop).
 * @param myStack Puntatore allo stack da cui estrarre.
 * @return void* Puntatore al dato estratto. NULL in caso di stack vuoto o invalido.
 */
void *StackPop(pStack_t myStack);

/**
 * @brief Restituisce l'elemento in cima allo stack senza estrarlo (Sola Lettura).
 * @param myStack Puntatore allo stack da ispezionare.
 * @return void* Puntatore al dato in cima. NULL in caso di stack vuoto o invalido.
 */
void *StackPeek(pStack_t myStack);

/**
 * @brief Restituisce il numero di elementi attualmente presenti nello stack.
 * @param myStack Puntatore allo stack da ispezionare.
 * @return int Numero di elementi. Restituisce -1 in caso di stack invalido.
 */
int StackSize(pStack_t myStack);

/**
 * @brief Distrugge lo stack e libera la RAM in modo sicuro.
 * Se specificata alla creazione, invoca la 'freeFn' su tutti gli elementi residui.
 * @param myStack Puntatore allo stack da distruggere.
 * @return true Se la distruzione è avvenuta con successo, false altrimenti.
 */
bool StackDestroy(pStack_t myStack);

#endif // STACK_H