#pragma once

#include <vector>
#include <pthread.h>

using namespace std;

/**
 * Diccionario de dueños de nodos. Esta es una estructura auxiliar al algoritmo
 * `mstParalelo` que permite manejar el "ownership" de los nodos por los
 * threads. Es una estructura compartida que todo thread puede acceder
 * concurrentemente.
 *
 * Un nodo puede cambiar de dueño (color) en dos situaciones:
 * - Puede ser capturado por un thread si previamente no tenía color asignado.
 * - Todos los nodos de un thread pasan a ser propiedad de otro thread luego de
 *   resolver un proceso de fusión.
 */
class Colores {

private:

    vector<int> _colores;
    vector<pthread_mutex_t> _locks;

    void lockNodo(const int nodoID) const;
    void unlockNodo(const int nodoID) const;

public:

    Colores();

    /** Reinicia el estado de la clase */
    void reset(const size_t n);

    /** Intenta capturar un nodo. Devuelve el threadID del dueño del nodo.
     *
     * nodoID: ID del nodo a capturar.
     * threadID: Id del thread que llama a este metodo.
     */
    int capturarNodo(const int nodoID, const int threadID);

    /** Devuelve id del thread dueño de un nodo. Devuelve -1 si el nodo no
     * tiene dueño. 
     */
    bool esDueno(const int nodoID, const int threadID);

    /** Se adueña de un nodo libre. De no existir devuelve -1. */
    int buscarNodoLibre(const int threadID);

    /** Pasa todos los nodos del thread con id mas grande al thread con id mas
     * chico. */
    void fusionarThreads(const int threadID1, const int threadID2);
};
