#include "cola_de_fusiones.h"

ColaDeFusiones::ColaDeFusiones() {
    pthread_mutex_init(&_lock, NULL);
}

bool ColaDeFusiones::notEmpty() const {
    bool ans;
    pthread_mutex_lock(&_lock);
        ans = ! _q.empty();
    pthread_mutex_unlock(&_lock);
    return ans;
}

void ColaDeFusiones::requestFusion() {

    // Inicializo un mutex cerrado para esperar a que el thread con el que me
    // quiero fusionar me de el ok.
    pthread_mutex_t mutex;
    pthread_mutex_init(&mutex, NULL);
    pthread_mutex_trylock(&mutex); // cierra el mutex

    // Pongo ese mutex en la cola
    pthread_mutex_lock(&_lock);
        _q.push(&mutex);
    pthread_mutex_unlock(&_lock);

    // espero a que el thread me de el ok para fusionarme.
    pthread_mutex_lock(&mutex); // wait
    pthread_mutex_destroy(&mutex); // Cierro el mutex x las dudas

}

void ColaDeFusiones::acknowledgeFusion() {
    pthread_mutex_lock(&_lock);

        pthread_mutex_t* p_mutex = _q.front();
        _q.pop();

        // Le indica al otro thread que se quedo esperando que puede continuar
        // la fusion.
        pthread_mutex_unlock(p_mutex);

    pthread_mutex_unlock(&_lock);
}
