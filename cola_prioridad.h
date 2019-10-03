#pragma once

#include <vector>
#include <queue>
#include "grafo.h"

using namespace std;

/** Estructura para obtener el eje mas corto alcanzable en un ciclo de Prim.
 *
 * - En todo momento, `top()` debe devolver el eje mas corto alcanzable.
 * - Cuando el thread se expande debe indicarle con `pop()` que ese eje mas
 *   corto fue añadido al AGM.
 */
class ColaDePrioridad {

private:

    // Lambda para comparar el peso de los ejes en la cola de prioridad
    class GreaterEje {
    public:
        bool operator()(const Eje &lhs, const Eje &rhs) const {
            return lhs.peso > rhs.peso;
        }
    };

    // -------------------------------------------------------------------------
    // Datos internos
    // -------------------------------------------------------------------------

    // Ejes alcanzables por el thread.
    std::priority_queue<Eje, vector<Eje>, GreaterEje> ejesVecinos;

public:

    // -------------------------------------------------------------------------
    // Métodos publicos
    // -------------------------------------------------------------------------

    /** Añade un eje a la cola de prioridad */
    void addEje(const Eje &e);

    /** Devuelve el par de nodos mas cercano en la cola de prioridad. Lo
     * elimina de la estructura de datos. */
    Eje top();

    /** Devuelve true si la cola de prioridad esta vacia */
    bool empty() const;

    /** Reinicia el estado de la cola de prioridad */
    void reset();

    /** Quita todos los elementos de la otra cola de prioridad y los inserta en
     * esta cola de prioridad. La otra cola de prioridad queda vacia. */
    void fusionar(ColaDePrioridad &cola);

};
