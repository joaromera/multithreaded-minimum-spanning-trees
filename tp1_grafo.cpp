#include "grafo.h"
#include <vector>
#include <stdlib.h>
#include <time.h>
#include <algorithm>
#include <limits>
#include <stack>
#include <atomic>
#include <queue>
#include <pthread.h>
#include <semaphore.h>
#include <chrono>
#include "lista_de_colores.h"
#include "cola_prioridad.h"
#include "colores.h"

using namespace std;

using id_thread = int32_t;

// Estructura para los datos básicos de un thread.
struct ThreadInfo {
    int thread; // Número de thread.
    int threadId; // ID del thread.

    ThreadInfo() : thread(-1), threadId(-1) {}

    ThreadInfo(const int thread, const int threadId)
        : thread(thread), threadId(threadId) {}
};

// Datos para agregar a la cola de fusiones de cada thread.
struct FusionInfo {
    int thread;  // thread que se debe fusionar

    // El eje o arista a agregar:
    int fromNode;
    int toNode;
    int weight;
};

struct Thread {
    // TO DO
    // Estructura que debe contener los colores de los vértices (actual y vecinos).
    // Las distancias, el árbol, y la herramientas de sincronización necesarias
    // para evitar race conditions y deadlocks.
    vector<int> agm;

    // Cola de prioridad para obtener el eje alcanzable mas corto.
    ColaDePrioridad ejesVecinos;

    // Constructor nulo. Cada thread se encarga de inicializar el estado
    // correspondiente pasandole el grafo como parámetro.
    Thread() {};

    /** Reinicia el estado interno del thread.
     *
     * Se llama al inicializar o luego de fusionar. Esta función existe pues se
     * necesita el constructor nulo para inicializar el vector<ThreadInfo>,
     * pero se requiere tambien saber la cantidad de vertices en el grafo
     */
    void reset(Grafo *g) {

        // Inicializa AGM vacio.
        agm.resize(g->numVertices);
        std::fill(agm.begin(), agm.end(), -1);

        // Reinicia 
        ejesVecinos.reset();

    }

};

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

// Para coordinar las modificaciones de los colores.
vector<id_thread> colored_nodes;

// Estructura atomica que registra los colores de los nodos.
Colores colores;

// Para contener la estructura global que indica el estado actual de cada nodo. 

// -----------------------------------------------------------------------------
// Funciones
// -----------------------------------------------------------------------------

// Retorna el nodo alcanzable a menor distancia. Si el thread todavia no tiene
// nodos, busca un nodo libre
std::pair<int, int> buscarNodo(int thread) {

    if ( threadData[thread].ejesVecinos.empty() ) {

        // El thread no conoce ningun nodo todavía. busca uno libre.
        const int nodo = colores.buscarNodoLibre();
        return std::make_pair(nodo, nodo);

    } else {

        // Quita el nodo mas cercano del priority queue
        return threadData[thread].ejesVecinos.popEje();

    }

}

// Se pinta el nodo de negro para indicar que fue colocado en el árbol
void pintarNodo(int num, int thread) {
   // TO DO
}

// Se pintan los vecinos de gris para marcar que son alcanzables desde el árbol (salvo los que ya son del árbol)
void pintarVecinos(Grafo *g, int num, int thread) {
   // TO DO
}

//Reinicia las estructuras de un thread.
void reiniciarThread(int thread, Grafo* g) {
    threadData[thread].reset(g);
}

// Iniciar un thread.
int initThread(Grafo *g) {
    int thread = thread_counter++;
    reiniciarThread(thread, g);
    return thread;
}

// Trata de reservar el nodo que se pasa como parametro para el thread
id_thread tomarNodo(int nodo, int thread) {
    colored_nodes[nodo] = thread;
    return thread; // FIXME cuando lo hagamos concurrente devuelve el dueño
}

void add_ejes_alcanzables(const id_thread thread, Grafo* g, const int nodo) {

    ColaDePrioridad &cola = threadData[thread].ejesVecinos;

    std::for_each(g->vecinosBegin(nodo), g->vecinosEnd(nodo),
        [&](const Eje &e){
            cola.addEje(nodo, e.nodoDestino, e.peso);
        });

}

// Procurar agregar el thread con mayor id a la cola de fusiones del thread con menor id
void requestFuse(int thread, ThreadInfo other, int node) {
    // TO DO

    // Se deben evitar race conditions, en los siguietes casos:
    // Un nodo hijo no puede estar en la cola de fusiones de otro nodo.
    // Solo se pueden agregar a la cola si el padre no está siendo fusionado por otro thread. 
}

// Realizar la fusión
void fuse(int parent, Grafo *g) {

    // TO DO

    //Se determina el thread que tengo que fusionar

    //Se espera a que el thread esté listo para fusionarse

    //Se fusionan las colas de fusiones del hijo
 
    //Se fusionan los colores del hijo

    //Se fusionan las distancias del hijo

    //Fusionar Arboles

    //Se notifica al hijo que se termino la fusion
}

/* Imprime el AGM formado por el thread */
void print_agm(const int thread, Grafo* g) {

    vector<int> &agm = threadData[thread].agm;
    Grafo out;

    for (size_t i = 0; i < agm.size(); ++i) {
        out.insertarNodo(i);
    }

    for (size_t i = 0; i < agm.size(); ++i) {
        int padre = agm[i];
        int hijo = i;
        // Toma la precaución de no añadir un eje que no va al grafo.
        if (padre == -1 or padre == hijo) continue;
        int peso = g->getPeso(padre, hijo);
        out.insertarEje(padre, hijo, peso);
    }

    out.imprimirGrafo();

}


// Gestión principal del thread. Contiene el ciclo que le permite a cada thread hacer sus funciones.
void* mstParaleloThread(void *p) {

    // Grafo global (esto es para cambiar el tipo de void* a Grafo*)
    Grafo* g = (Grafo*) p;

    // Se obtiene el numero de thread y se inicializan sus estructuras
    id_thread this_thread_id = initThread(g);

    std::pair<int, int> eje_actual(-1, -1);

    // Ciclo principal de cada thread
    while(true){

        // Si el thread está en la cola de fusiones de otro thread, lo notifica que puede fusionarse. 

        // Se deben usar mecanismos de sincronización.

        // TO DO

        // Si otro thread me está fusionando, esperar a que termine.

        // Reinicializo las estructuras del thread y arranco de nuevo.

        // Si tiene elementos en la cola de fusion, debe fusionarlos.

        // Se busca el nodo más cercano que no esté en el árbol, pero que sea alcanzable
        eje_actual = buscarNodo(this_thread_id);

        // Si ya no existen nodos libres sale del loop de expandirse.
        if (eje_actual == std::make_pair(-1, -1)) break; // FIXME si hay tiempo hacer fc

        // Si el nodo ya pertenece a este thread, saltea intentar tomarlo.
        if (colores.esDueno(eje_actual.second, this_thread_id)) continue;

        // Intenta capturar el nodo buscado. El valor deuvelto es el dueño del
        // nodo:
        // - Si se capturó con exito, es el mismo ID que el de este thread.
        // - Caso contrario será el ID del thread con el que debe fusionarse.
        id_thread thread_info = colores.capturarNodo(eje_actual.second,
                                                     this_thread_id);

        // Si se logra tomar, se procesa.
        if (thread_info == this_thread_id) {

            // Nodos padre e hijo (en la jerarquia del AGM) del eje a añadir.
            int padre = eje_actual.first;
            int hijo = eje_actual.second;

            // Se añade nodo al AGM.
            threadData[this_thread_id].agm[hijo] = padre;

            // Se añaden a la cola de prioridad, los ejes alcanzables por el
            // nuevo nodo.
            add_ejes_alcanzables(this_thread_id, g, hijo);

        } else {
            // FIXME Si el nodo lo tiene otro thread, se tiene que fusionar
        }


        // requestFuse(.....);
    }

    // Al terminar el loop, se imprime el resultado y se termina el thread.
    print_agm(this_thread_id, g);

    return NULL;

}

void mstParalelo(Grafo *g, int cantThreads) {
    // Verificar cantidad de threads para ejecutar el algoritmo
    if (cantThreads < 1) {
        cerr << "El número de threads debe ser igual o mayor a 1" << endl;
    }

    // Si el numero de vertices del grafo es 0, imprimir el grafo vacio
    if (g->numVertices == 0) {
        if (imprimirResultado) {
            cout << endl << "********** RESULTADO *********** " << endl;
            Grafo().imprimirGrafo();
        }
        return;
    }

    // Se inicializan las estructuras globales

    pthread_id.resize(cantThreads); // pthread_id de c/u thread.
    threadData.resize(cantThreads); // Data interna de c/u thread.
    // Cada nodo empieza sin dueño
    colored_nodes.resize(g->numVertices);
    std::fill(colored_nodes.begin(), colored_nodes.end(), -1);
    colores.reset(g->numVertices);


    // Inicializa cada uno de los threads.
    for (int i = 0; i < cantThreads; ++i) {
        // Inicializa un thread corriendo `mstParaleloThread(g)`.
        pthread_create(&pthread_id[i], NULL, mstParaleloThread, (void*) g);
    }

    // Espera a que termine cada uno de los threads. En este caso el orden no
    // importa.
    for (int i = 0; i < cantThreads; ++i) {
        pthread_join(pthread_id[i], NULL);
    }
}

//Reinicia la experimentación.
void resetExperimentacion() {
   // TO DO 
}

//Procedimiento para realizar las pruebas o test mínimo de la cátedra.
void experimentacion() {
    imprimirResultado = false;
    cout << "instancia,n,grafo,threads, tiempo" << endl;
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
                    auto end = std::chrono::steady_clock::now();

                    std::cout << instancia << "," << n << "," << grafo << "," << 1 << ","
                              << std::chrono::duration <double, std::milli> (end-start).count()
                              << std::endl;
                    instancia++;
                    resetExperimentacion();
                }

                if (k == 1) {
                    grafo = "ralo";
                    auto start = std::chrono::steady_clock::now();
                    auto end = std::chrono::steady_clock::now();

                    std::cout << instancia << "," << n << "," << grafo << "," << 1 << ","
                              << std::chrono::duration <double, std::milli> (end-start).count()
                              << std::endl;
                    instancia++;
                    resetExperimentacion();
                }

                if (k == 2) {
                    grafo = "completo";
                    auto start = std::chrono::steady_clock::now();
                    auto end = std::chrono::steady_clock::now();

                    std::cout << instancia << "," << n << "," << grafo << "," << 1 << ","
                              << std::chrono::duration <double, std::milli> (end-start).count()
                              << std::endl;
                    instancia++;
                    resetExperimentacion();
                }

                for (int threads = 2; threads <= 32; threads *= 2) {
                    if (k == 0) {
                        grafo = "arbol";
                        auto start = std::chrono::steady_clock::now();
                        mstParalelo(&g, threads);
                        auto end = std::chrono::steady_clock::now();

                        std::cout << instancia << "," << n << "," << grafo << "," << threads << ","
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

                        std::cout << instancia << "," << n << "," << grafo << "," << threads << ","
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

                        std::cout << instancia << "," << n << "," << grafo << "," << threads << ","
                                  << std::chrono::duration <double, std::milli> (end-start).count()
                                  << std::endl;
                        instancia++;
                        resetExperimentacion();
                    }
                }
            }
        }
    }
}

int main(int argc, char const * argv[]) {
    if (argc <= 1) {
        cerr << "Introduzca el nombre del archivo o el parámetro \"-e\" para hacer varias pruebas " << endl;
        return 1;
    }

    if (string(argv[1]) == "-e") {
        experimentacion();
        return 0;
    }

    string nombre;
    nombre = string(argv[1]);

    string algoritmo;
    if (argc > 2) {
        algoritmo = string(argv[2]);
    }

    int threads = 1;
    if (argc > 3) {
        threads = atoi(argv[3]);
    }

    Grafo g;
    if (g.inicializar(nombre) == 1) {
        mstParalelo(&g, threads);
    } else {
        cerr << "Error: Grafo no cargado correctamente" << endl;
        return 1;
    }

    return 0;
}
