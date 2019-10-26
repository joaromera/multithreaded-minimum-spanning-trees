#include <string>
#include <sstream>
#include "cola_prioridad.h"
#include "log.h"

void ColaDePrioridad::addEje(const Eje &e) {
    ejesVecinos.push(e);
}

Eje ColaDePrioridad::top() const {
    Eje top = ejesVecinos.top();
    return top;
}

bool ColaDePrioridad::empty() const {
    return ejesVecinos.empty();
}

void ColaDePrioridad::clear() {
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
