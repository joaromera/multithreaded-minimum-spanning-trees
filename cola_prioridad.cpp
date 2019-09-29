#include "cola_prioridad.h"


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

void ColaDePrioridad::fusionar(ColaDePrioridad cola) {
    while (! cola.empty() ) {
        _eje_t e = cola.ejesVecinos.top();
        cola.ejesVecinos.pop();
        this->addEje(e.nodo_conocido, e.nodo_alcanzable, e.peso);
    }
}
