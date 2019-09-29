#pragma once

#include <memory>
#include <queue>
#include <pthread.h>

class ColaDeFusiones {
public:
    ColaDeFusiones();

    /** Devuelve true si existe un thread que quiere fusionarse con este. */
    bool notEmpty() const;

    /* Bloquea hasta que el thread le da el ok para continuar la fusion.  */
    void requestFusion();

    /** Permite fusion con el proximo thread en la cola. Debe ser llamado solo
     *  por el thread dueño de esta clase.
     */
    void acknowledgeFusion();

private:
    mutable pthread_mutex_t _lock;
    std::queue<pthread_mutex_t*> _q;
};
