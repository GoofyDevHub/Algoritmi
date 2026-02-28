#ifndef COMMON_H
#define COMMON_H

/* ========================================================================= *
 * RIPASSO RAPIDO: PUNTATORI A FUNZIONE (IL MOTORE DELL'ASTRAZIONE)          *
 * ========================================================================= *
 * SOTTO IL COFANO:
 * In C, una funzione è solo un blocco contiguo di istruzioni Assembly
 * salvato nel Segmento Text della RAM. Il "nome" della funzione è, di fatto,
 * l'indirizzo di memoria della sua prima istruzione.
 * * SINTASSI:
 * tipo_di_ritorno (*nome_tipo)(tipo_arg1, tipo_arg2);
 * (Le parentesi attorno all'asterisco sono obbligatorie, altrimenti il C
 * crederà che la funzione restituisca un puntatore).
 * * IL PARADIGMA (INVERSION OF CONTROL):
 * Usiamo i puntatori a funzione per iniettare logica esterna dentro moduli
 * "ciechi". Se un BST deve ordinare dei void*, non può usare l'operatore '>'.
 * Invece, il BST chiamerà l'indirizzo di memoria della nostra funzione custom
 * (es. compare_drones), delegando la decisione a chi conosce davvero il dato.
 * ========================================================================= */

 
/**
 * @brief Firma per le funzioni di pulizia memoria personalizzate.
 * Utile se il modulo deve distruggere struct complesse allocate dall'utente.
 */
typedef void (*FreeFn_t)(void* data);

/**
 * @brief Firma per le funzioni di confronto generiche (Motore Decisionale).
 * * Il compilatore C non sa cosa ci sia dentro i puntatori void*, quindi
 * delega la decisione a questa funzione iniettata dall'utente.
 * @param a Puntatore al primo elemento da confrontare.
 * @param b Puntatore al secondo elemento.
 * @return int Un valore > 0 se 'a' ha priorità su 'b', < 0 se ha priorità 'b', 0 se uguali.
 */
typedef int (*CompareFn_t)(const void *a, const void *b);

#endif // COMMON_H