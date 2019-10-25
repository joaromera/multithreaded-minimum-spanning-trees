#include "thread.h"

Thread::Thread()
{
    pthread_mutex_init(&fusion_req, NULL);

    pthread_mutex_init(&fusion_ack, NULL);
    pthread_mutex_trylock(&fusion_ack);

    pthread_mutex_init(&fusion_ready, NULL);
    pthread_mutex_trylock(&fusion_ready);
}

void Thread::add_ejes_alcanzables(Grafo *g, const int nodo)
{
    std::for_each(g->vecinosBegin(nodo), g->vecinosEnd(nodo),
                  [&](const Eje &e) {
                      ejesVecinos.addEje(e);
                  });
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