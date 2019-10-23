#pragma once

#include "thread.h"
#include "colores.h"
#include <vector>
#include <atomic>

// -----------------------------------------------------------------------------
// Variables globales
// -----------------------------------------------------------------------------

// Imprimir el grafo resultado durante los experimentos
bool imprimirResultado = true;

// Se sugieren usar variables (unas atómicas y otras no) para:

// Contener el estado global de la estructura de threads.
std::vector<Thread> threadData;

// Para coordinar el número de cada thread durante la inizializacion de threads.
atomic<int> thread_counter {0};

// Para para coordinar el id de cada thread durante la inizializacion y
// reinicializacion de threads.
std::vector<pthread_t> pthread_id;

// Estructura atomica que registra los colores de los nodos.
Colores colores;
