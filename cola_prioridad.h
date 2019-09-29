#pragma once

#include <vector>
#include <queue>

using namespace std;

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

    /** Quita todos los elementos de la otra cola de prioridad y los inserta en
     * esta cola de prioridad. La otra cola de prioridad queda vacia. */
    void fusionar(ColaDePrioridad cola);

};
