#pragma once

#include "thread.h"
#include "colores.h"
#include <vector>
#include <atomic>

// Imprimir el grafo resultado durante los experimentos
bool imprimirResultado = true;

// Estado de cada thread
std::vector<Thread> threadData;

// Contador para identificar los threads al inicializarlos
atomic<int> thread_counter {0};

// Contador de fusiones, para experimento
atomic<int> fusion_counter {0};

// Para coordinar la creación de cada thread y esperar a su finalización
std::vector<pthread_t> pthread_id;

// Estructura atomica que registra los colores (id de los dueños) de los nodos
Colores colores;
