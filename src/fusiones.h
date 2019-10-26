#pragma once

#include "grafo.h"
#include "globales.h"

/* Atender pedidos de fusion recibidos
 * Al terminar, el mutex 'fusion_req' de este thread queda cerrado, por lo
 * que debe desbloquearse si desea que el thread pueda seguir recibiendo
 * pedidos para fusionarse.
 */
void thread_attend_fusion_requests(int tid)
{
    // Mientras exista un fusion_req entrante, los atiende
    while (pthread_mutex_trylock(&threadData[tid].fusion_req))
    {
        pthread_mutex_unlock(&threadData[tid].fusion_ack);
        pthread_mutex_lock(&threadData[tid].fusion_ready);
    }
}

// Copia el AGM g2 en g1, pasando todos sus ejes
void fuse_agm(Grafo &g1, Grafo &g2)
{
    for (int i = 0; i < g1.numVertices; i++)
    {
        for (auto &e : g2.listaDeAdyacencias[i])
        {
            g1.listaDeAdyacencias[i].push_back(e);
        }
    }
    g1.numEjes += g2.numEjes;
}

// Luego de fusionarse el thread con menor id (A) tiene los ejes encontrados
// por el otro thread (B) y la cola de prioridad de vecinos actualizada.
// Ademas, B es reseteado y sus nodos ahora tienen el id de A
void fuse(const int tid1, const int tid2, const int nodo1, const int nodo2)
{
    assert(tid1 < tid2);
    assert(colores.esDueno(nodo1, tid1));
    assert(colores.esDueno(nodo2, tid2));

    fusion_counter++;
    colores.fusionarThreads(tid1, tid2);
    fuse_agm(threadData[tid1].agm, threadData[tid2].agm);
    threadData[tid1].ejesVecinos.fusionar(threadData[tid2].ejesVecinos);
    threadData[tid2].reset(threadData[tid1].agm.numVertices);
}
