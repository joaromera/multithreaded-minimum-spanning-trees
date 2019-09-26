#pragma once

#include <queue>
#include <vector>

class ColaDePrioridad {

private:

    // -------------------------------------------------------------------------
    // Estructuras de datos aux
    // -------------------------------------------------------------------------

    // Estructura interna para representar los ejes.
    class _eje_t {
    public:
        int nodo_conocido;
        int nodo_alcanzable;
        int peso;

        _eje_t(const int nodo_conocido,
               const int nodo_alcanzable,
               const int peso) :
            nodo_conocido(nodo_conocido),
            nodo_alcanzable(nodo_alcanzable),
            peso(peso) {}

    };


    // Lambda para comparar el peso de los ejes en la cola de prioridad
    class GreaterEje {
    public:
        bool operator()(const _eje_t &lhs, const _eje_t &rhs) const {
            return lhs.peso > rhs.peso;
        }
    };

    // -------------------------------------------------------------------------
    // Datos internos
    // -------------------------------------------------------------------------

    // Ejes alcanzables por el thread.
    std::priority_queue<_eje_t, vector<_eje_t>, GreaterEje> ejesVecinos;

public:

    // -------------------------------------------------------------------------
    // Métodos publicos
    // -------------------------------------------------------------------------

    /** Añade un eje a la cola de prioridad */
    void addEje(const int nodo_conocido,
                const int nodo_alcanzable,
                const int peso);

    /** Devuelve el par de nodos mas cercano en la cola de prioridad. Lo
     * elimina de la estructura de datos. */
    std::pair<int,int> popEje();

    /** Devuelve true si la cola de prioridad esta vacia */
    bool empty() const;

    /** Reinicia el estado de la cola de prioridad */
    void reset();

};


void ColaDePrioridad::addEje(const int nodo_conocido,
                             const int nodo_alcanzable,
                             const int peso) {

    _eje_t e(nodo_conocido, nodo_alcanzable, peso);
    ejesVecinos.push(e);

}

std::pair<int,int> ColaDePrioridad::popEje() {

    _eje_t e = ejesVecinos.top();
    ejesVecinos.pop();
    return std::pair<int,int>(e.nodo_conocido, e.nodo_alcanzable);

}

bool ColaDePrioridad::empty() const {
    return ejesVecinos.empty();
}

void ColaDePrioridad::reset() {
    // Asigna una cola vacia reiniciando el estado de la cola de prioridad.
    ejesVecinos = std::priority_queue<_eje_t, vector<_eje_t>, GreaterEje>();
}
