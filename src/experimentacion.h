#pragma once

#include "grafo.h"
#include "globales.h"
#include <chrono>
#include <time.h>
#include <fstream>

// Definida en tp1_grafo.cpp
void mstParalelo(Grafo *g, int cantThreads);

// Reinicia las estructuras globales para experimentación
void resetExperimentacion()
{
    threadData.clear();
    pthread_id.clear();
    colores.reset(0);
    thread_counter = 0;
    fusion_counter = 0;
    idle_counter = 0;
}

// Procedimiento para realizar las pruebas o test mínimo de la cátedra
void experimentacion() {
    imprimirResultado = false;
    ofstream outfile;
    outfile.open("file.csv", std::ofstream::out | std::ofstream::trunc);
    outfile << "instancia,n,grafo,threads, tiempo" << endl;
    int instancia = 0;
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

                    outfile << instancia << "," << n << "," << grafo << "," << 1 << ","
                              << std::chrono::duration <double, std::milli> (end-start).count()
                              << std::endl;
                    instancia++;
                    resetExperimentacion();
                }

                if (k == 1) {
                    grafo = "ralo";
                    auto start = std::chrono::steady_clock::now();
                    mstParalelo(&g, 1);
                    auto end = std::chrono::steady_clock::now();

                    outfile << instancia << "," << n << "," << grafo << "," << 1 << ","
                              << std::chrono::duration <double, std::milli> (end-start).count()
                              << std::endl;
                    instancia++;
                    resetExperimentacion();
                }

                if (k == 2) {
                    grafo = "completo";
                    auto start = std::chrono::steady_clock::now();
                    mstParalelo(&g, 1);
                    auto end = std::chrono::steady_clock::now();

                    outfile << instancia << "," << n << "," << grafo << "," << 1 << ","
                              << std::chrono::duration <double, std::milli> (end-start).count()
                              << std::endl;
                    instancia++;
                    resetExperimentacion();
                }

                for (int threads = 2; threads <= 128; threads *= 2) {
                    if (k == 0) {
                        grafo = "arbol";
                        auto start = std::chrono::steady_clock::now();
                        mstParalelo(&g, threads);
                        auto end = std::chrono::steady_clock::now();

                        outfile << instancia << "," << n << "," << grafo << "," << threads << ","
                                  << std::chrono::duration <double, std::milli> (end-start).count()
                                  << std::endl;
                        instancia++;
                        resetExperimentacion();
                    }

                    if (k == 1) {
                        grafo = "ralo";
                        auto start = std::chrono::steady_clock::now();
                        mstParalelo(&g, threads);
                        auto end = std::chrono::steady_clock::now();

                        outfile << instancia << "," << n << "," << grafo << "," << threads << ","
                                  << std::chrono::duration <double, std::milli> (end-start).count()
                                  << std::endl;
                        instancia++;
                        resetExperimentacion();
                    }

                    if (k == 2) {
                        grafo = "completo";
                        auto start = std::chrono::steady_clock::now();
                        mstParalelo(&g, threads);
                        auto end = std::chrono::steady_clock::now();

                        outfile << instancia << "," << n << "," << grafo << "," << threads << ","
                                  << std::chrono::duration <double, std::milli> (end-start).count()
                                  << std::endl;
                        instancia++;
                        resetExperimentacion();
                    }
                }
            }
        }
    }
    outfile.close();
}

void experimentacion_fusion_time() {
    imprimirResultado = false;
    ofstream outfile;
    outfile.open("fusiones_tiempo.csv", std::ofstream::out | std::ofstream::trunc);
    outfile << "instancia,n,grafo,threads,tiempo" << endl;
    int instancia = 0;
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
                    float diff = (end - start).count();
                    outfile << instancia << "," << n << "," << grafo << "," << 1 << ","
                              << (float) idle_counter / diff
                              << std::endl;
                    instancia++;
                    resetExperimentacion();
                }

                if (k == 1) {
                    grafo = "ralo";
                    auto start = std::chrono::steady_clock::now();
                    mstParalelo(&g, 1);
                    auto end = std::chrono::steady_clock::now();
                    float diff = (end - start).count();
                    outfile << instancia << "," << n << "," << grafo << "," << 1 << ","
                            << (float) idle_counter / diff
                            << std::endl;
                    instancia++;
                    resetExperimentacion();
                }

                if (k == 2) {
                    grafo = "completo";
                    auto start = std::chrono::steady_clock::now();
                    mstParalelo(&g, 1);
                    auto end = std::chrono::steady_clock::now();
                    float diff = (end - start).count();
                    outfile << instancia << "," << n << "," << grafo << "," << 1 << ","
                            << (float) idle_counter / diff
                            << std::endl;
                    instancia++;
                    resetExperimentacion();
                }

                for (int threads = 2; threads <= 128; threads *= 2) {
                    if (k == 0) {
                        grafo = "arbol";
                        auto start = std::chrono::steady_clock::now();
                        mstParalelo(&g, threads);
                        auto end = std::chrono::steady_clock::now();
                        float diff = (end - start).count();
                        outfile << instancia << "," << n << "," << grafo << "," << threads << ","
                                << (float) idle_counter / threads / diff
                                << std::endl;
                        instancia++;
                        resetExperimentacion();
                    }

                    if (k == 1) {
                        grafo = "ralo";
                        auto start = std::chrono::steady_clock::now();
                        mstParalelo(&g, threads);
                        auto end = std::chrono::steady_clock::now();
                        float diff = (end - start).count();
                        outfile << instancia << "," << n << "," << grafo << "," << threads << ","
                                << (float) idle_counter / threads / diff
                                << std::endl;
                        instancia++;
                        resetExperimentacion();
                    }

                    if (k == 2) {
                        grafo = "completo";
                        auto start = std::chrono::steady_clock::now();
                        mstParalelo(&g, threads);
                        auto end = std::chrono::steady_clock::now();
                        float diff = (end - start).count();
                        outfile << instancia << "," << n << "," << grafo << "," << threads << ","
                                << (float) idle_counter / threads / diff
                                << std::endl;
                        instancia++;
                        resetExperimentacion();
                    }
                }
            }
        }
    }
    outfile.close();
}

// Procedimiento para realizar experimento de cantidad de fusiones
void experimentacion_fusiones() {
    imprimirResultado = false;
    ofstream outfile;
    outfile.open("fusiones.csv", std::ofstream::out | std::ofstream::trunc);
    outfile << "instancia,n,grafo,threads,fusiones,tiempo" << endl;
    int instancia = 0;
    string grafo;

    for (int n = 0; n < 100; n++) {
        for (int k = 0; k <= 2; k++) {
            Grafo g;
            if (k == 0) {
                if (g.inicializar("test/experimentacion/fusiones/arbol" + to_string(n) +".txt") != 1) {
                    cerr << "No se pudo cargar el grafo correctamente" << endl;
                    return;
                }
            }
            if (k == 1) {
                if (g.inicializar("test/experimentacion/fusiones/completo" + to_string(n) + ".txt") != 1)
                {
                    cerr << "No se pudo cargar el grafo correctamente" << endl;
                    return;
                }
            }

            for (int i = 0; i < 10; i++) {
                for (int threads = 1; threads <= 40; threads++) {
                    if (k == 0) {
                        grafo = "arbol";
                        auto start = std::chrono::steady_clock::now();
                        mstParalelo(&g, threads);
                        auto end = std::chrono::steady_clock::now();

                        outfile << instancia << "," << n << "," << grafo << "," << threads << "," << fusion_counter << ","
                                << std::chrono::duration <double, std::milli> (end-start).count()
                                << std::endl;
                        instancia++;
                        resetExperimentacion();
                    }

                    if (k == 1) {
                        grafo = "completo";
                        auto start = std::chrono::steady_clock::now();
                        mstParalelo(&g, threads);
                        auto end = std::chrono::steady_clock::now();

                        outfile << instancia << "," << n << "," << grafo << "," << threads << "," << fusion_counter << ","
                                << std::chrono::duration <double, std::milli> (end-start).count()
                                << std::endl;
                        instancia++;
                        resetExperimentacion();
                    }
                }
            }
        }
    }
    outfile.close();
}
