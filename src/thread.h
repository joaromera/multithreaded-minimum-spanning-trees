#pragma once

#include <algorithm>
#include "grafo.h"
#include "log.h"
#include "cola_prioridad.h"

using namespace std;

// Estructura que contiene el AGM que va creando un thread y que organiza
// los pedidos de fusiones.
struct Thread
{
    // Cada thread tiene su propio AGM para ir coloreando
    Grafo agm;

    // Cola de prioridad para obtener el eje alcanzable de menor peso
    ColaDePrioridad ejesVecinos;

    pthread_mutex_t fusion_req;   // Me bloquea para fusionarme
    pthread_mutex_t fusion_ack;   // Acepto la fusion de quien tenga mi 'lock'
    pthread_mutex_t fusion_ready; // Terminaron de fusionarme

    // Constructor nulo. Cada thread se encarga de inicializar el estado
    // correspondiente pasando el grafo como parámetro.
    Thread();

    void add_ejes_alcanzables(Grafo *g, const int nodo);

    // Reinicia el estado del thread al inicializar y luego de fusionar
    void reset(size_t nVertices);
};
