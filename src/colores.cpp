#include "colores.h"
#include "log.h"

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

int Colores::buscarNodoLibre(const int threadID) {
    bool encontrado = false;
    int ans = -1;
    for (size_t i = 0; ( i < _colores.size() ) && ( ! encontrado ); ++i) {
        pthread_mutex_lock(&_locks[i]);
            if ( _colores[i] == -1 ) {
                _colores[i] = threadID;
                ans = i;
                encontrado = true;
            }
        pthread_mutex_unlock(&_locks[i]);
    }
    return ans;
}

bool Colores::esDueno(const int nodoID, const int threadID) {
    int dueno;

    pthread_mutex_lock(&_locks[nodoID]);
        dueno = _colores[nodoID];
    pthread_mutex_unlock(&_locks[nodoID]);

    return dueno == threadID;
}

void Colores::fusionarThreads(const int threadID1, const int threadID2) {

    if (threadID2 < threadID1) {
        fusionarThreads(threadID2, threadID1);
        return;
    }

    for (size_t i = 0; i < _colores.size(); ++i) {
        pthread_mutex_lock(&_locks[i]);
        if ( _colores[i] == threadID2 ) {
            _colores[i] = threadID1;
        }
        pthread_mutex_unlock(&_locks[i]);
    }

}
