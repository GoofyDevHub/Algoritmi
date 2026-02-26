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
typedef struct {
    void **data;     // Array dinamico di puntatori generici (void*)
    int capacity;    // Numero massimo di elementi attualmente ospitabili
    int top;         // Indice del prossimo slot libero (coincide con la size)
} Stack_t; 

typedef Stack_t* pStack_t; 


/* ========================================================================= *
 * IMPLEMENTAZIONE DELLE FUNZIONI                                            *
 * ========================================================================= */

/**
 * @brief Inizializza un nuovo stack in memoria (Heap).
 * * Alloca lo spazio per la struttura di controllo e per l'array interno 
 * di puntatori. Se la capacità iniziale richiesta non è valida (<= 0), 
 * applica un fallback sicuro a 8 slot.
 * * @param initialCapacity Dimensione iniziale dell'array interno.
 * @return pStack_t Puntatore allo stack creato, oppure NULL se la RAM è esaurita.
 * * @warning È responsabilità del chiamante liberare la memoria invocando 
 * una futura funzione StackDestroy() quando lo stack non serve più.
 */
pStack_t StackCreate(int initialCapacity) {
    
    // 1. Allocazione del "guscio" di controllo
    pStack_t myStack = (pStack_t)malloc(sizeof(Stack_t)); 
    if (myStack == NULL) return NULL; 

    // 2. Normalizzazione dell'input (Prevenzione allocazioni negative)
    myStack->capacity = (initialCapacity > 0) ? initialCapacity : 8; 

    // 3. Allocazione dell'array dei dati
    // sizeof(void*) garantisce 8 byte su sistemi a 64-bit per ogni slot
    myStack->data = (void**)malloc(myStack->capacity * sizeof(void*)); 
    
    // 4. Gestione sicura del fallimento a cascata
    if (myStack->data == NULL) {
        free(myStack); // Evita di lasciare il guscio orfano nella RAM
        return NULL; 
    }

    // 5. Setup dello stato iniziale (Convenzione: top = prossimo slot vuoto)
    myStack->top = 0; 
    
    return myStack;  
}

/**
 * @brief Inserisce un nuovo elemento in cima allo stack (Push).
 * * L'operazione avviene in tempo O(1). Nel caso in cui l'array interno 
 * sia pieno, la funzione raddoppia automaticamente la capacità allocata 
 * (Crescita Geometrica), garantendo un tempo O(1) ammortizzato.
 * * @param myStack Puntatore allo stack di destinazione.
 * @param data    Puntatore (void*) al dato effettivo da immagazzinare.
 * @return true   Se l'inserimento è andato a buon fine.
 * @return false  Se lo stack è NULL o se il sistema operativo rifiuta di 
 * concedere ulteriore RAM durante il ridimensionamento.
 * * @note La funzione copia solo l'indirizzo di memoria (il puntatore). 
 * Il dato reale puntato da 'data' deve rimanere valido in memoria 
 * per tutto il tempo in cui risiede nello stack.
 */
bool StackPush(pStack_t myStack, void* data) {

    // Sicurezza: Evita la dereferenziazione di puntatori nulli
    if (myStack == NULL) {
        return false; 
    }

    // Controllo saturazione: top ha raggiunto il limite della capacità
    if (myStack->capacity == myStack->top) {
        int newCapacity = myStack->capacity * 2;  
        
        // Raddoppio della memoria usando un puntatore temporaneo.
        // Se si usasse myStack->data direttamente e la realloc fallisse, 
        // si perderebbe per sempre l'indirizzo dell'array originale.
        void** tempData = (void**)realloc(myStack->data, newCapacity * sizeof(void*));   
        
        if (tempData == NULL) {
            // RAM esaurita. Lo stack originale rimane intatto.
            return false; 
        }
        
        // Aggiornamento dei metadati post-espansione
        myStack->data = tempData; 
        myStack->capacity = newCapacity;
    }

    // Inserimento del dato e avanzamento dell'indice (Post-incremento)
    myStack->data[myStack->top] = data; 
    myStack->top++; 

    return true;      
}

/**
 * @brief Estrae e restituisce l'elemento in cima allo stack (Pop).
 * * L'operazione avviene in tempo costante. Il dato non viene sovrascritto 
 * fisicamente nell'array: l'indice 'top' viene semplicemente retrocesso, 
 * rendendo quello slot di memoria logicamente "libero" per la prossima operazione di Push.
 * * @param myStack Puntatore allo stack da cui estrarre.
 * @return void* Puntatore al dato estratto. 
 * @retval NULL   Se lo stack è vuoto (Underflow) o se il puntatore passato non è valido.
 */
void* StackPop(pStack_t myStack) {
    
    // 1. Sicurezza: Prevenzione dereferenziazione puntatore nullo
    if (myStack == NULL) {
        return NULL; 
    }

    // 2. Controllo Underflow: Lo stack è vuoto (top == 0 indica zero elementi)
    if (myStack->top == 0) {
        return NULL;
    }
    
    // 3. Estrazione (Pre-decremento)
    // Decremento l'indice top di 1 per puntare all'ultimo elemento valido, 
    // dopodiché uso questo nuovo indice per accedere all'array e ritornare il dato.
    return myStack->data[--myStack->top]; 
}

/**
 * @brief Restituisce l'elemento in cima allo stack senza estrarlo (Sola Lettura).
 * * Permette di ispezionare il prossimo elemento che verrebbe restituito da una Pop, 
 * lasciando inalterato lo stato logico e la dimensione dello stack.
 * * @param myStack Puntatore allo stack da ispezionare.
 * @return void* Puntatore al dato in cima. 
 * @retval NULL   Se lo stack è invalido (NULL) o se è vuoto (Underflow).
 */
void* StackPeek(pStack_t myStack) {

    // 1. Sicurezza: Lo stack esiste in memoria?
    if (myStack == NULL) {
        return NULL; 
    }

    // 2. Controllo Underflow: C'è almeno un elemento da leggere?
    if (myStack->top == 0) {
        return NULL; 
    }

    // 3. Ispezione (Read-Only)
    // Sottraendo 1 a 'top' calcolo l'indice dell'ultimo dato inserito,
    // ma la variabile 'top' all'interno della struct rimane intatta.
    return myStack->data[myStack->top - 1]; 
}

/**
 * @brief Restituisce il numero di elementi attualmente presenti nello stack.
 * * L'operazione è risolta in tempo O(1) leggendo direttamente la variabile di stato 'top'.
 * * @param myStack Puntatore allo stack da ispezionare.
 * @return int Numero di elementi presenti.
 * @retval -1 Se il puntatore allo stack passato come argomento è invalido (NULL).
 */
int StackSize(pStack_t myStack) {

    // Sicurezza: prevenzione crash su puntatore inesistente
    if (myStack == NULL) {
        return -1; 
    }

    // top coincide esattamente con la cardinalità dell'insieme
    return myStack->top; 
}

