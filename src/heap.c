/*
// heap.h
typedef struct Heap_t* Heap;
// capacity è la dimensione iniziale dell'array dinamico
Heap heap_create(int capacity, CompareFn cmp, FreeFn free_fn);
void heap_insert(Heap h, void* data);
void* heap_extract_top(Heap h);
void* heap_peek(Heap h);
// decrease_key serve a Dijkstra per aggiornare il costo di un nodo senza rompere l'Heap
void heap_decrease_key(Heap h, void* old_data, void* new_data);
void heap_destroy(Heap h);
*/

/*
@section Descrizione di uno HEAP
    *si tratta di una struttura dati gerarchica non lineare basata sugli alberi binari.
        *affinchè un heap venga considerato tale devono valere le seguenti proprietà :
            *HEAP SHAPE = albero deve essere completo,
                  ovvero tutti i nodi devono avere 2 o 0 figli
                      *meno che per ultimo ramo in cui è sufficiente che albero venga riempito da sinitra.
                          *HEAP PROPERTY = una proprietà sui valori contenuti che ne determina un ORDINAMENTO PARZIALE,
                  nel dettaglio :
                      *MAX -
    HEAP = parenti > figli
                             *MIN -
                         HEAP = parenti < figli
*/ 


#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "../inc/heap.h"
#include "../inc/common.h"

struct Heap_t
{
    void **data; 
    int capacity; 
    int size; 
    FreeFn_t freeFn; 
    CompareFn_t cmpFn; 
};

/* ========================================================================= *
 * FUNZIONI PRIVATE (MOTORE LOGICO)                                          *
 * ========================================================================= */


 static bool HeapIsEmpty(pHeap_t myHeap)
 {
    return (myHeap->size == 0); 
 }

/**
 * @brief Calcola se l'heap o pieno o meno.
 */
static bool HeapIsFull(pHeap_t myHeap)
{
    return (myHeap->size == myHeap->capacity);
}

/**
 * @brief Calcola l'indice del nodo padre.
 * Risolve l'equazione: Parent(i) = floor((i - 1) / 2)
 */
static int GetParent(int index)
{
    return (index - 1) / 2;
}

/**
 * @brief Calcola l'indice del figlio sinistro.
 * Risolve l'equazione: LeftChild(i) = 2i + 1
 */
static int GetLeftChild(int index)
{
    return (2 * index) + 1;
}

/**
 * @brief Calcola l'indice del figlio destro.
 * Risolve l'equazione: RightChild(i) = 2i + 2
 */
static int GetRightChild(int index)
{
    return (2 * index) + 2;
}

/**
 * @brief Scambia fisicamente due puntatori in memoria.
 * Operazione atomica fondamentale per far salire o scendere i nodi (Sift-Up / Sift-Down).
 */
static void Swap(void **a, void **b)
{
    void *temp = *a;
    *a = *b;
    *b = temp;
}
/**
 * @brief Algoritmo di Sift-Down. Ripristina la proprietà dell'Heap dall'alto verso il basso.
 * * Confronta il nodo corrente con i suoi figli (se esistono). Se uno dei figli ha una
 * priorità maggiore (determinata dinamicamente dalla cmpFn), il nodo corrente viene
 * scambiato con il figlio "vincente". Il processo si ripete finché il nodo non
 * raggiunge una posizione valida o diventa una foglia.
 * * @param myHeap Puntatore all'Heap.
 * @param index L'indice da cui far partire lo sprofondamento (solitamente 0).
 * * @note Costo computazionale temporale: $O(\log N)$. Costo spaziale: $O(1)$ (Iterativo).
 */
static void HeapifyDown(pHeap_t myHeap, int index)
{
    int currentIndex = index;

    while (true)
    {
        int extremeIndex = currentIndex; // Assumiamo che il nodo corrente sia il "vincente"
        int leftChild = GetLeftChild(currentIndex);
        int rightChild = GetRightChild(currentIndex);

        // 1. Controllo il figlio sinistro.
        // Se esiste nell'array E se la cmpFn dice che è "maggiore/minore" del nodo corrente,
        // diventa lui il nuovo candidato vincente.
        if (leftChild < myHeap->size &&
            myHeap->cmpFn(myHeap->data[leftChild], myHeap->data[extremeIndex]) > 0)
        {
            extremeIndex = leftChild;
        }

        // 2. Controllo il figlio destro.
        // Se esiste E se la cmpFn dice che batte il candidato vincente attuale
        // (che potrebbe essere il nodo corrente o il figlio sinistro), aggiorniamo.
        if (rightChild < myHeap->size &&
            myHeap->cmpFn(myHeap->data[rightChild], myHeap->data[extremeIndex]) > 0)
        {
            extremeIndex = rightChild;
        }

        // 3. Condizione di uscita: se l'extremeIndex è ancora il currentIndex,
        // significa che il nodo è più "forte" di entrambi i figli. L'Heap è ripristinato.
        if (extremeIndex == currentIndex)
        {
            break;
        }

        // 4. Scambio fisico dei dati e avanzamento del ciclo
        Swap(&myHeap->data[currentIndex], &myHeap->data[extremeIndex]);
        currentIndex = extremeIndex;
    }
}

/**
 * @brief Algoritmo di Sift-Up. Ripristina la proprietà dell'Heap dal basso verso l'alto.
 * Viene invocato tipicamente dopo l'inserimento di un nuovo elemento in coda all'array.
 * L'algoritmo confronta il nodo corrente esclusivamente con il proprio padre.
 * Se la cmpFn rileva che il figlio ha una priorità maggiore del padre, i due
 * vengono scambiati. La risalita si ferma quando si raggiunge la radice (indice 0)
 * o quando l'equilibrio è matematicamente ripristinato.
 * @param myHeap Puntatore all'Heap.
 * @param index L'indice di partenza (tipicamente l'ultimo elemento inserito).
 * @note Costo temporale nel caso peggiore (risalita fino alla radice): $O(\log N)$.
 */
static void HeapifyUp(pHeap_t myHeap, int index)
{
    int currentIndex = index;

    // Condizione di sicurezza: la radice (indice 0) non ha un padre,
    // quindi la risalita si deve fermare prima.
    while (currentIndex > 0)
    {

        int parentIndex = GetParent(currentIndex);

        // Il motore decisionale agnostico: se il figlio batte il padre (> 0)
        if (myHeap->cmpFn(myHeap->data[currentIndex], myHeap->data[parentIndex]) > 0)
        {

            // Violazione rilevata: scambio i puntatori fisici nella RAM
            Swap(&myHeap->data[currentIndex], &myHeap->data[parentIndex]);

            // Aggiorno l'indice per il prossimo ciclo di iterazione
            currentIndex = parentIndex;
        }
        else
        {
            // Early Exit: il nodo non ha la forza di battere il padre attuale.
            // L'integrità strutturale dell'Heap è confermata, interrompo il ciclo.
            break;
        }
    }
}

/* ========================================================================= *
 * API PUBBLICA (IMPLEMENTAZIONE)                                            *
 * ========================================================================= */

pHeap_t HeapCreate(int capacity, CompareFn_t cmpFn, FreeFn_t freeFn)
{
    // Sicurezza: Un heap senza funzione di confronto non può funzionare
    if (cmpFn == NULL)
    {
        return NULL;
    }

    pHeap_t myHeap = (pHeap_t)malloc(sizeof(struct Heap_t));
    if (myHeap == NULL)
        return NULL;

    myHeap->capacity = (capacity > 0) ? capacity : 16; // Capacity base leggermente più ampia
    myHeap->size = 0;

    myHeap->data = (void **)malloc(myHeap->capacity * sizeof(void *));
    if (myHeap->data == NULL)
    {
        free(myHeap);
        return NULL;
    }

    myHeap->cmpFn = cmpFn;
    myHeap->freeFn = freeFn;

    return myHeap;
}

bool HeapInsert(pHeap_t myHeap, void *data)
{
    // Sicurezza: Validazione del puntatore
    if (myHeap == NULL)
        return false;

    // Controllo saturazione e Riallocazione Geometrica
    if (HeapIsFull(myHeap))
    {
        int newCapacity = myHeap->capacity * 2;

        // Allocazione su puntatore temporaneo per proteggere i dati in caso di fallimento
        void **tempData = (void **)realloc(myHeap->data, newCapacity * sizeof(void *));

        if (tempData == NULL)
            return false;

        // Commit della transazione in memoria
        myHeap->data = tempData;
        myHeap->capacity = newCapacity;
    }

    // 1. Salvataggio dell'indice di inserimento (0-indexed)
    int insertIndex = myHeap->size;

    // 2. Inserimento fisico nell'ultimo slot libero
    myHeap->data[insertIndex] = data;

    // 3. Aggiornamento dello stato logico
    myHeap->size++;

    // 4. Ripristino dell'invariante dell'Heap (Sift-Up)
    HeapifyUp(myHeap, insertIndex);

    return true;
}

void *HeapExtract(pHeap_t myHeap)
{
    // 1. Sicurezza: Validazione puntatore e controllo Underflow
    if (myHeap == NULL || myHeap->size == 0)
    {
        return NULL;
    }

    // 2. Salvataggio del dato a massima priorità (Radice all'indice 0)
    void *temp = myHeap->data[0];

    // 3. Spostamento dell'ultimo elemento foglia nella posizione della Radice
    myHeap->data[0] = myHeap->data[myHeap->size - 1];

    // 4. Rimozione logica dell'ultimo elemento
    myHeap->size--;

    // 5. Ripristino dell'invariante: faccio "sprofondare" la nuova radice
    // (Ha senso farlo solo se è rimasto almeno un elemento nell'Heap)
    if (myHeap->size > 0)
    {
        HeapifyDown(myHeap, 0);
    }

    // 6. Ritorno il dato originale
    return temp;
}

void *HeapPeek(pHeap_t myHeap)
{
    if (myHeap == NULL || myHeap->size == 0)
        return NULL;

    return myHeap->data[0];
}

int HeapSize(pHeap_t myHeap)
{
    if (myHeap == NULL)
        return -1;

    return myHeap->size;
}