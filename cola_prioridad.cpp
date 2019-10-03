#include "cola_prioridad.h"


void ColaDePrioridad::addEje(const Eje &e) {
    ejesVecinos.push(e);
}

Eje ColaDePrioridad::top() const {
    return ejesVecinos.top();
}

bool ColaDePrioridad::empty() const {
    return ejesVecinos.empty();
}

void ColaDePrioridad::reset() {
    // Asigna una cola vacia reiniciando el estado de la cola de prioridad.
    ejesVecinos = std::priority_queue<Eje, vector<Eje>, GreaterEje>();
}

void ColaDePrioridad::fusionar(ColaDePrioridad &cola) {
    while (! cola.empty() ) {
        this->addEje( cola.ejesVecinos.top() );
        cola.ejesVecinos.pop();
    }
}

void ColaDePrioridad::pop() {
    ejesVecinos.pop();
}
