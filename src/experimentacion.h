#pragma once

#include "grafo.h"
#include "globales.h"
#include <chrono>
#include <time.h>

// Definida en tp1_grafo.cpp
void mstParalelo(Grafo *g, int cantThreads);

// Reinicia las estructuras globales para experimentación
void resetExperimentacion()
{
    threadData.clear();
    pthread_id.clear();
    colores.reset(0);
    thread_counter = 0;
}

void writerow(std::string grafo, uint32_t n_threads, uint32_t n_nodes, std::chrono::duration<double, std::milli> timedelta) {
    cout << grafo << ","
         << n_threads << ","
         << n_nodes << ","
         << timedelta.count() << endl;
}

// Procedimiento para realizar las pruebas o test mínimo de la cátedra
void experimentacion() {
    imprimirResultado = false;
    cout << "grafo,threads,n,tiempo" << endl;
    string grafo;

    for (int n = 100; n <= 1000; n += 100) {
        for (int k = 0; k <= 2; k++) {
            Grafo g;
            if (k == 0) {
                if (g.inicializar("test/experimentacion/arbol/arbol" + to_string(n) +".txt") != 1) {
                    cerr << "No se pudo cargar el grafo correctamente" << endl;
                    return;
                }
            }
            if (k == 1) {
                if (g.inicializar("test/experimentacion/ralo/ralo" + to_string(n) + ".txt") != 1) {
                    cerr << "No se pudo cargar el grafo correctamente" << endl;
                    return;
                }
            }
            if (k == 2) {
                if (g.inicializar("test/experimentacion/completo/completo" + to_string(n) + ".txt") != 1) {
                    cerr << "No se pudo cargar el grafo correctamente" << endl;
                    return;
                }
            }

            for (int i = 0; i < 10; i++) {
                if (k == 0) {
                    grafo = "arbol";
                    auto start = std::chrono::steady_clock::now();
                    mstParalelo(&g, 1);
                    auto end = std::chrono::steady_clock::now();

                    auto timedelta = std::chrono::duration<double, std::milli>(end-start);
                    writerow(grafo, 1, n, timedelta);
                    resetExperimentacion();
                }

                if (k == 1) {
                    grafo = "ralo";
                    auto start = std::chrono::steady_clock::now();
                    mstParalelo(&g, 1);
                    auto end = std::chrono::steady_clock::now();

                    auto timedelta = std::chrono::duration<double, std::milli>(end-start);
                    writerow(grafo, 1, n, timedelta);
                    resetExperimentacion();
                }

                if (k == 2) {
                    grafo = "completo";
                    auto start = std::chrono::steady_clock::now();
                    mstParalelo(&g, 1);
                    auto end = std::chrono::steady_clock::now();

                    auto timedelta = std::chrono::duration<double, std::milli>(end-start);
                    writerow(grafo, 1, n, timedelta);
                    resetExperimentacion();
                }

                for (int threads = 2; threads <= 32; threads *= 2) {
                    if (k == 0) {
                        grafo = "arbol";
                        auto start = std::chrono::steady_clock::now();
                        mstParalelo(&g, threads);
                        auto end = std::chrono::steady_clock::now();

                        auto timedelta = std::chrono::duration<double, std::milli>(end-start);
                        writerow(grafo, threads, n, timedelta);
                        resetExperimentacion();
                    }

                    if (k == 1) {
                        grafo = "ralo";
                        auto start = std::chrono::steady_clock::now();
                        mstParalelo(&g, threads);
                        auto end = std::chrono::steady_clock::now();

                        auto timedelta = std::chrono::duration<double, std::milli>(end-start);
                        writerow(grafo, threads, n, timedelta);
                        resetExperimentacion();
                    }

                    if (k == 2) {
                        grafo = "completo";
                        auto start = std::chrono::steady_clock::now();
                        mstParalelo(&g, threads);
                        auto end = std::chrono::steady_clock::now();

                        auto timedelta = std::chrono::duration<double, std::milli>(end-start);
                        writerow(grafo, threads, n, timedelta);
                        resetExperimentacion();
                    }
                }
            }
        }
    }
}
