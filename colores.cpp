#include "colores.h"

Colores::Colores() {}

void Colores::reset(const size_t n) {

    _colores.resize(n);
    std::fill(_colores.begin(), _colores.end(), -1);

    // Elimina los locks preexistentes
    for (size_t i = 0; i < _locks.size(); ++i) {
        pthread_mutex_destroy(&_locks[i]);
    }
    // Crea n locks
    _locks.resize(n);
    for (size_t i = 0; i < n; ++i) {
        pthread_mutex_init(&_locks[i], NULL);
    }

}

int Colores::capturarNodo(const int nodoID, const int threadID) {
    int ans;

    pthread_mutex_lock(&_locks[nodoID]);

        if (_colores[nodoID] == -1) {
            _colores[nodoID] = threadID; 
        }

        ans = _colores[nodoID];

    pthread_mutex_unlock(&_locks[nodoID]);

    return ans;
}
