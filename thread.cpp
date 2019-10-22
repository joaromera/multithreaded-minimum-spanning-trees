#include "thread.h"

ThreadInfo::ThreadInfo() : thread(-1), threadId(-1) {}

ThreadInfo::ThreadInfo(const int thread, const int threadId)
    : thread(thread), threadId(threadId) {}

Thread::Thread()
{
    log("inicializando thread");
    pthread_mutex_init(&fusion_req, NULL);
    pthread_mutex_init(&fusion_ack, NULL);
    pthread_mutex_trylock(&fusion_ack);
    pthread_mutex_init(&fusion_ready, NULL);
    pthread_mutex_trylock(&fusion_ready);
    log("fin init thread");
}

void Thread::add_ejes_alcanzables(Grafo *g, const int nodo)
{
    std::for_each(g->vecinosBegin(nodo), g->vecinosEnd(nodo),
                  [&](const Eje &e) {
                      ejesVecinos.addEje(e);
                  });

    log("add_ejes_alcanzables: Cola de prioridad es %s",
        ejesVecinos.toString().c_str());
}

void Thread::reset(size_t nVertices)
{
    // Inicializa AGM vacio.
    agm = Grafo();

    for (size_t i = 0; i < nVertices; ++i)
    {
        agm.insertarNodo(i);
    }

    // Reinicia
    ejesVecinos.reset();
}