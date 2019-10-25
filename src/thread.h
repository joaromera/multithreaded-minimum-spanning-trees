#pragma once

#include <algorithm>
#include "grafo.h"
#include "log.h"
#include "cola_prioridad.h"

using namespace std;

// Estructura que debe contener los colores de los vértices (actual y vecinos).
// Las distancias, el árbol, y la herramientas de sincronización necesarias
// para evitar race conditions y deadlocks.
struct Thread
{
    // Cada thread tiene su propio AGM para ir coloreando
    Grafo agm;

    // Cola de prioridad para obtener el eje alcanzable mas corto.
    ColaDePrioridad ejesVecinos;

    pthread_mutex_t fusion_req;   // Me bloquea para fusionarme
    pthread_mutex_t fusion_ack;   // Acepto la fusion de quien tenga mi `lock`
    pthread_mutex_t fusion_ready; // Terminaron de fusionarme

    // Constructor nulo. Cada thread se encarga de inicializar el estado
    // correspondiente pasandole el grafo como parámetro.
    Thread();

    void add_ejes_alcanzables(Grafo *g, const int nodo);

    /** Reinicia el estado interno del thread.
     * Se llama al inicializar o luego de fusionar. Esta función existe pues se
     * necesita el constructor nulo para inicializar el vector<Thread>,
     * pero se requiere tambien saber la cantidad de vertices en el grafo
     */
    void reset(size_t nVertices);
};
