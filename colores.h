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

    /** Devuelve un nodo sin dueño al momento de ejecutar esta función.
     *
     * El propietario del nodo puede ser distinto al momento de ejecutar
     * `capturarNodo`.
     */
    int buscarNodoLibre() const;

};
