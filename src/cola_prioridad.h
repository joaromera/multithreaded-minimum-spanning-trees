#pragma once

#include <string>
#include <vector>
#include <queue>
#include "grafo.h"

using namespace std;

/* Estructura para obtener el eje de menor peso
 * En todo momento, 'top()' debe devolver el eje de menor peso.
 * Cuando el thread expande su AGM, cada eje agregado al mismo
 * debe eliminarse de la cola usando 'pop()'
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

    // Ejes alcanzables por el thread.
    std::priority_queue<Eje, vector<Eje>, GreaterEje> ejesVecinos;

public:

    // Añade un eje a la cola de prioridad
    void addEje(const Eje &e);

    // Devuelve el par de nodos mas cercano en la cola de prioridad
    Eje top() const;

    // Devuelve true si la cola de prioridad esta vacia
    bool empty() const;

    // Reinicia el estado de la cola de prioridad
    void clear();

    // Quita todos los elementos de la otra cola de prioridad y los inserta en
    // esta cola de prioridad. La otra cola de prioridad queda vacia
    void fusionar(ColaDePrioridad &cola);

    // Elimina el elemento de menor peso
    void pop();

    // Devuelve un string mostrando todos los ejes
    std::string toString() const;
};
