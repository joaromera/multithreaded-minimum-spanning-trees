#include <string>
#include <sstream>
#include "cola_prioridad.h"
#include "log.h"


void ColaDePrioridad::addEje(const Eje &e) {
    // log("ColaDePrioridad::addEje");
    ejesVecinos.push(e);
}

Eje ColaDePrioridad::top() const {
    // log("ColaDePrioridad::top: Estado del heap %s", toString().c_str());
    Eje top = ejesVecinos.top();
    // // log("Eje top es: (%d, %d, %d)", top.nodoOrigen, top.nodoDestino, top.peso);
    return top;
}

bool ColaDePrioridad::empty() const {
    // log("ColaDePrioridad::empty");
    return ejesVecinos.empty();
}

void ColaDePrioridad::reset() {
    // log("ColaDePrioridad::reset");
    // Asigna una cola vacia reiniciando el estado de la cola de prioridad.
    ejesVecinos = std::priority_queue<Eje, vector<Eje>, GreaterEje>();
}

void ColaDePrioridad::fusionar(ColaDePrioridad &cola) {
    // log("ColaDePrioridad::fusionar");
    while (! cola.empty() ) {
        this->addEje( cola.ejesVecinos.top() );
        cola.ejesVecinos.pop();
    }
}

void ColaDePrioridad::pop() {
    // log("ColaDePrioridad::pop");
    ejesVecinos.pop();
}

std::string ColaDePrioridad::toString() const {
    std::priority_queue<Eje, vector<Eje>, GreaterEje> tmp = ejesVecinos;
    std::stringstream buf;

    buf << "[ ";

    while ( ! tmp.empty() ) {
        const Eje e = tmp.top();
        buf << "(";
        buf << e.nodoOrigen << ",";
        buf << e.nodoDestino << ",";
        buf << e.peso << ") ";
        tmp.pop();
    }
    buf << "]";

    return buf.str();
}
