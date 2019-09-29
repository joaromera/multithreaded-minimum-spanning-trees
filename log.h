#pragma once
#include <iostream>
#include <string>
#include <pthread.h>

using namespace std;

pthread_mutex_t logMutex;

void init_log() {
    pthread_mutex_init(&logMutex, NULL);
}

/** Función de logging */
static void log(const int threadID, const string m) {
    pthread_mutex_lock(&logMutex);
    cout << threadID << ": " << m << endl << flush;
    pthread_mutex_unlock(&logMutex);
}
