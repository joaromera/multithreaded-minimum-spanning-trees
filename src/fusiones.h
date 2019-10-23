#pragma once

#include "grafo.h"
#include "globales.h"

void fuse_agm(Grafo &g1, Grafo &g2)
{
    for (int i = 0; i < g1.numVertices; i++)
    {
        log("fuse_agm: pasando ejes del nodo %d", i);
        for (auto &e : g2.listaDeAdyacencias[i])
        {
            g1.listaDeAdyacencias[i].push_back(e);
        }
    }
    g1.numEjes += g2.numEjes;

    // Reiniciar g2 FIXME
}

void fuse(const int tid1, const int tid2, const int nodo1, const int nodo2)
{
    assert(tid1 < tid2);
    assert(colores.esDueno(nodo1, tid1));
    assert(colores.esDueno(nodo2, tid2));

    log("fuse: iniciando fusion %d->%d sobre (%d, %d)",
        tid1, tid2, nodo1, nodo2);

    log("fuse: pasando nodos de %d a %d", tid2, tid1);
    colores.fusionarThreads(tid1, tid2);
    log("fuse: pasando AGM de %d a %d", tid2, tid1);
    fuse_agm(threadData[tid1].agm, threadData[tid2].agm);
    log("fuse: añadiendo (%d, %d) al AGM", nodo1, nodo2);
    // FIXME
    log("fuse: volcando cola de prioridad %d a %d", tid2, tid1);
    threadData[tid1].ejesVecinos.fusionar(threadData[tid2].ejesVecinos);
    log("fuse: reiniciando thread %d", tid2);
    threadData[tid2].reset(threadData[tid1].agm.numVertices);
    log("fuse: finish");
}

/** Atender pedidos de fusion recibidos
 * Al terminar, el mutex `fusion_req` de este thread quedará cerrado, por lo
 * que deberá desbloquearse si se desea que el thread pueda seguir recibiendo
 * pedidos de fusión.
 */
void thread_attend_fusion_requests(int tid)
{
    log("thread_attend_fusion_requests: atendiendo.", tid);
    // Mientras exista un fusion_req entrante,
    while (pthread_mutex_trylock(&threadData[tid].fusion_req))
    {
        log("thread_attend_fusion_requests: Dando ack.", tid);
        pthread_mutex_unlock(&threadData[tid].fusion_ack);
        pthread_mutex_lock(&threadData[tid].fusion_ready);
        log("thread_attend_fusion_requests: Recibido el fusion_ready.", tid);
    }
    log("thread_attend_fusion_requests: se termino de atender.", tid);
}