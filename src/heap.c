#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "../inc/heap.h"
#include "../inc/common.h"

/* ========================================================================= *
 * MANIFESTO ARCHITETTURALE: LA CODA A PRIORITA' E L'HEAP BINARIO            *
 * ========================================================================= *
 * MECCANICA DI BASE (L'Astrazione):
 * Una Coda a Priorità non si basa sul tempo di arrivo (FIFO) come una normale
 * Queue, ma sull'importanza del dato. Immagina il Pronto Soccorso: non importa
 * chi è arrivato prima, il "Codice Rosso" viene estratto e servito per primo.
 * * L'IMPLEMENTAZIONE (L'Heap Binario):
 * Dal punto di vista strutturale, l'Heap è un Albero Binario che rispetta
 * due regole ferree:
 * 1. HEAP SHAPE (Struttura): È un albero "completo". Tutti i livelli sono
 * pieni, tranne al massimo l'ultimo, che deve essere riempito rigorosamente
 * da sinistra verso destra. Nessun buco è permesso.
 * 2. HEAP PROPERTY (Ordinamento Parziale): Ogni nodo Padre ha una priorità
 * strettamente maggiore (o minore) rispetto a entrambi i suoi figli.
 * * IL TRUCCO DELL'ALBERO IMPLICITO (Cache Locality):
 * Invece di usare costosi nodi concatenati (struct con puntatori left/right),
 * mappiamo l'albero su un Array Dinamico contiguo. Questo elimina l'overhead
 * dei puntatori e massimizza la Cache Locality della CPU, risolvendo i legami
 * padre-figlio con purissima e velocissima aritmetica degli indici in tempo O(1).
 * ========================================================================= */

struct Heap_t
{
    void **data;
    int capacity;
    int size;
    FreeFn_t freeFn;
    CompareFn_t cmpFn;
};

/* ========================================================================= *
 * FUNZIONI PRIVATE (MOTORE LOGICO E MATEMATICA DEGLI INDICI)                *
 * ========================================================================= */

static bool HeapIsEmpty(pHeap_t myHeap)
{
    // Ritorna true solo se la cardinalità logica è zero
    return (myHeap->size == 0);
}

static bool HeapIsFull(pHeap_t myHeap)
{
    // Confronta la cardinalità logica con il limite fisico allocato in RAM
    return (myHeap->size == myHeap->capacity);
}

static int GetParent(int index)
{
    // Sfrutta il troncamento della divisione tra interi in C per emulare la funzione matematica floor()
    return (index - 1) / 2;
}

static int GetLeftChild(int index)
{
    // Salto di livello verso il basso a sinistra
    return (2 * index) + 1;
}

static int GetRightChild(int index)
{
    // Salto di livello verso il basso a destra
    return (2 * index) + 2;
}

static void Swap(void **a, void **b)
{
    // Scambio atomico dei puntatori fisici per muovere il payload senza copiarne i dati
    void *temp = *a;
    *a = *b;
    *b = temp;
}

static void HeapifyDown(pHeap_t myHeap, int index)
{
    int currentIndex = index;

    while (true)
    {
        // 1. Assumiamo che il nodo corrente sia già nella posizione corretta
        int extremeIndex = currentIndex;

        // 2. Calcolo preventivo degli indici dei figli in tempo O(1)
        int leftChild = GetLeftChild(currentIndex);
        int rightChild = GetRightChild(currentIndex);

        // 3. Ispezione figlio sinistro: verifico che esista nell'array (leftChild < size)
        // e chiedo al motore decisionale (cmpFn) se batte il genitore
        if (leftChild < myHeap->size &&
            myHeap->cmpFn(myHeap->data[leftChild], myHeap->data[extremeIndex]) > 0)
        {
            extremeIndex = leftChild; // Il sinistro è il nuovo candidato vincitore
        }

        // 4. Ispezione figlio destro: verifico che esista e chiedo se batte il candidato attuale
        // (che potrebbe essere il genitore o il figlio sinistro)
        if (rightChild < myHeap->size &&
            myHeap->cmpFn(myHeap->data[rightChild], myHeap->data[extremeIndex]) > 0)
        {
            extremeIndex = rightChild; // Il destro domina su entrambi
        }

        // 5. Condizione di uscita: se il candidato vincitore è rimasto il nodo corrente,
        // l'invariante dell'Heap è matematicamente ripristinata.
        if (extremeIndex == currentIndex)
        {
            break;
        }

        // 6. Esecuzione del collasso: scambio i puntatori in memoria e preparo il prossimo ciclo
        Swap(&myHeap->data[currentIndex], &myHeap->data[extremeIndex]);
        currentIndex = extremeIndex;
    }
}

static void HeapifyUp(pHeap_t myHeap, int index)
{
    int currentIndex = index;

    // Continua finché non raggiunge la radice assoluta dell'albero (indice 0)
    while (currentIndex > 0)
    {
        // 1. Individuo matematicamente il genitore del nodo corrente
        int parentIndex = GetParent(currentIndex);

        // 2. Chiedo al motore decisionale se il figlio ha una priorità maggiore del padre
        if (myHeap->cmpFn(myHeap->data[currentIndex], myHeap->data[parentIndex]) > 0)
        {
            // 3a. Violazione rilevata: scambio i nodi per far "galleggiare" il dato verso l'alto
            Swap(&myHeap->data[currentIndex], &myHeap->data[parentIndex]);

            // 3b. Aggiorno l'indice per il prossimo controllo al livello superiore
            currentIndex = parentIndex;
        }
        else
        {
            // 4. Early Exit: il figlio ha una priorità minore o uguale al padre.
            // L'integrità strutturale è confermata.
            break;
        }
    }
}

/* ========================================================================= *
 * API PUBBLICA (IMPLEMENTAZIONE)                                            *
 * ========================================================================= */

pHeap_t HeapCreate(int capacity, CompareFn_t cmpFn, FreeFn_t freeFn)
{
    // 1. Blocco critico: un Heap generico non sa ordinarsi senza la CompareFn
    if (cmpFn == NULL)
        return NULL;

    // 2. Allocazione del guscio infrastrutturale
    pHeap_t myHeap = (pHeap_t)malloc(sizeof(struct Heap_t));
    if (myHeap == NULL)
        return NULL;

    // 3. Normalizzazione capacità: fallback a 16 per evitare array troppo piccoli
    myHeap->capacity = (capacity > 0) ? capacity : 16;
    myHeap->size = 0;

    // 4. Allocazione dell'array contiguo che ospiterà l'albero implicito
    myHeap->data = (void **)malloc(myHeap->capacity * sizeof(void *));

    // 5. Prevenzione Memory Leak: se fallisce l'array, distruggo anche il guscio
    if (myHeap->data == NULL)
    {
        free(myHeap);
        return NULL;
    }

    // 6. Iniezione delle dipendenze per l'ordinamento e la distruzione
    myHeap->cmpFn = cmpFn;
    myHeap->freeFn = freeFn;

    return myHeap;
}

bool HeapInsert(pHeap_t myHeap, void *data)
{
    // 1. Controllo validità dell'infrastruttura
    if (myHeap == NULL)
        return false;

    // 2. Gestione saturazione tramite Crescita Geometrica (costo O(N) ammortizzato su N inserimenti)
    if (HeapIsFull(myHeap))
    {
        int newCapacity = myHeap->capacity * 2;

        // Uso un puntatore temporaneo per non corrompere l'Heap in caso di esaurimento RAM
        void **tempData = (void **)realloc(myHeap->data, newCapacity * sizeof(void *));

        if (tempData == NULL)
            return false;

        myHeap->data = tempData;
        myHeap->capacity = newCapacity;
    }

    // 3. Posizionamento fisico del dato nell'ultimo slot logico dell'array (Nuova Foglia)
    int insertIndex = myHeap->size;
    myHeap->data[insertIndex] = data;

    // 4. Aggiornamento cardinalità
    myHeap->size++;

    // 5. Ripristino dell'ordine gerarchico tramite emersione (costo O(log N))
    HeapifyUp(myHeap, insertIndex);

    return true;
}

void *HeapExtract(pHeap_t myHeap)
{
    // 1. Controllo Underflow: non posso estrarre da un albero vuoto
    if (myHeap == NULL || myHeap->size == 0)
        return NULL;

    // 2. Salvataggio in cache locale del payload a massima priorità (Radice)
    void *temp = myHeap->data[0];

    // 3. Usurpazione: sposto brutalmente l'ultima foglia dell'albero nella posizione della Radice
    myHeap->data[0] = myHeap->data[myHeap->size - 1];

    // 4. Restringimento logico dell'array (l'ultima cella diventa spazzatura ignorata)
    myHeap->size--;

    // 5. Se l'albero non è rimasto vuoto, faccio sprofondare l'usurpatore per ritrovare l'equilibrio
    if (myHeap->size > 0)
    {
        HeapifyDown(myHeap, 0);
    }

    // 6. Restituisco il dato originariamente in testa
    return temp;
}

void *HeapPeek(pHeap_t myHeap)
{
    // Semplice ispezione O(1) con controlli di sicurezza
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

bool HeapUpdatePriority(pHeap_t myHeap, void *data)
{
    if (myHeap == NULL || data == NULL || myHeap->size == 0)
        return false;

    // 1. Ricerca Lineare Costosa O(N): Poiché l'Heap non impone un ordine laterale,
    // devo ispezionare tutta la RAM allocata per trovare il puntatore bersaglio.
    int targetIndex = -1;
    for (int i = 0; i < myHeap->size; i++)
    {
        if (myHeap->data[i] == data)
        {
            targetIndex = i;
            break;
        }
    }

    // 2. Early exit: l'elemento non fa parte di questo Heap
    if (targetIndex == -1)
        return false;

    // 3. Doppio colpo (Costo O(log N)): Poiché non so a priori se la priorità esterna
    // è stata aumentata o diminuita, chiamo entrambi i motori. Solo uno farà davvero il lavoro.
    HeapifyUp(myHeap, targetIndex);
    HeapifyDown(myHeap, targetIndex);

    return true;
}

bool HeapDestroy(pHeap_t myHeap)
{
    if (myHeap == NULL)
        return false;

    // 1. Pulizia dei payload: L'Heap garantisce che da 0 a size-1 la RAM sia contigua.
    // Iterazione O(N) lineare pura, eccellente per il pre-fetching della CPU Cache.
    if (myHeap->freeFn != NULL)
    {
        for (int i = 0; i < myHeap->size; i++)
        {
            myHeap->freeFn(myHeap->data[i]);
        }
    }

    // 2. Deallocazione dell'infrastruttura fisica
    free(myHeap->data);
    free(myHeap);

    return true;
}