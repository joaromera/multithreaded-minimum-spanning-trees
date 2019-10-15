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
#include "log.h"
#include <cassert>

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
    Grafo agm;

    // Cola de prioridad para obtener el eje alcanzable mas corto.
    ColaDePrioridad ejesVecinos;

    pthread_mutex_t fusion_req; // Me bloquea para fusionarme
    pthread_mutex_t fusion_ack; // Acepto la fusion de quien tenga mi `lock`
    pthread_mutex_t fusion_ready; // Terminaron de fusionarme

    // Constructor nulo. Cada thread se encarga de inicializar el estado
    // correspondiente pasandole el grafo como parámetro.
    Thread() {
        log("inicializando thread");
        pthread_mutex_init(&fusion_req, NULL);
        pthread_mutex_init(&fusion_ack, NULL);
        pthread_mutex_trylock(&fusion_ack);
        pthread_mutex_init(&fusion_ready, NULL);
        pthread_mutex_trylock(&fusion_ready);
        log("fin init thread");
    };

    /** Reinicia el estado interno del thread.
     *
     * Se llama al inicializar o luego de fusionar. Esta función existe pues se
     * necesita el constructor nulo para inicializar el vector<ThreadInfo>,
     * pero se requiere tambien saber la cantidad de vertices en el grafo
     */
    void reset(size_t nVertices) {

        // Inicializa AGM vacio.
        agm = Grafo();
        for (size_t i = 0; i < nVertices; ++i) { agm.insertarNodo(i); }

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

// Estructura atomica que registra los colores de los nodos.
Colores colores;

// Para contener la estructura global que indica el estado actual de cada nodo. 

// -----------------------------------------------------------------------------
// Funciones
// -----------------------------------------------------------------------------

enum StatusBuscarNodo { Ok, AgmCompleto, NoHayNodosDisponibles };

// Retorna el nodo alcanzable a menor distancia. Si el thread todavia no tiene
// nodos, busca un nodo libre
StatusBuscarNodo buscarNodo(int thread, Eje &out) {

    if ( threadData[thread].ejesVecinos.empty() ) {
        log("buscarNodo: buscando el primer nodo del trhead");

        const int nodo = colores.buscarNodoLibre(thread);
        if (nodo != -1) {
            out = Eje(nodo, nodo, 0);
            return Ok;
        } else {
            return NoHayNodosDisponibles;
        }

    } else {

        log("buscarNodo: Va a devolver el nodo alcanzable mas cercano.");
        while ( ! threadData[thread].ejesVecinos.empty() ) {
            Eje e = threadData[thread].ejesVecinos.top();
            log("buscarNodo: probando eje (%d, %d)", e.nodoOrigen, e.nodoDestino);
            if (! colores.esDueno(e.nodoDestino, thread)) {
                out = e;
                return Ok;
            }

            // Quita este eje de los ejesVecinos
            threadData[thread].ejesVecinos.pop();

        }

        return AgmCompleto;
    }
}

//Reinicia las estructuras de un thread.
void reiniciarThread(int thread, Grafo* g) {
    threadData[thread].reset(g->numVertices);
}

// Iniciar un thread.
int initThread(Grafo *g) {
    int thread = thread_counter++;
    reiniciarThread(thread, g);
    return thread;
}


void add_ejes_alcanzables(const id_thread thread, Grafo* g, const int nodo) {

    ColaDePrioridad &cola = threadData[thread].ejesVecinos;
    std::for_each(g->vecinosBegin(nodo), g->vecinosEnd(nodo),
        [&](const Eje &e){
            cola.addEje(e);
        });

    log("add_ejes_alcanzables: Cola de prioridad es %s",
            threadData[thread].ejesVecinos.toString().c_str());

}

void fuse_agm(Grafo &g1, Grafo &g2) {
    for (int i = 0; i < g1.numVertices; i++) {
        log("fuse_agm: pasando ejes del nodo %d", i);
        for (auto &e : g2.listaDeAdyacencias[i]) {
            g1.listaDeAdyacencias[i].push_back(e);
        }
    }
    g1.numEjes += g2.numEjes;

    // Reiniciar g2
}

void fuse(const int tid1, const int tid2, const int nodo1, const int nodo2) {

    assert(tid1 < tid2);
    assert(colores.esDueno(nodo1, tid1));
    assert(colores.esDueno(nodo2, tid2));

    log("fuse: iniciando fusion %d->%d sobre (%d, %d)",
        tid1, tid2, nodo1, nodo2);

    log("fuse: pasando nodos de %d a %d", tid2, tid1);
    colores.fusionarThreads(tid1, tid2);
    log("fuse: pasando AGM de %d a %d", tid2, tid1);
    fuse_agm(threadData[tid1].agm, threadData[tid2].agm);
    log("fuse: añadiendo (%d, %d) al AGM", nodo1, nodo2);
    // FIXME
    log("fuse: volcando cola de prioridad %d a %d", tid2, tid1);
    threadData[tid1].ejesVecinos.fusionar(threadData[tid2].ejesVecinos);
    log("fuse: reiniciando thread %d", tid2);
    threadData[tid2].reset(threadData[tid1].agm.numVertices);
    log("fuse: finish");

}

/** Atender pedidos de fusion entreantes a un thread `tid`.
 *
 * Al terminar, el mutex `fusion_req` de este thread quedará cerrado, por lo
 * que deberá desbloquearse si se decea que el thread pueda seguir recibiendo
 * pedidos de fusión. */
void thread_attend_fusion_requests(int tid) {
    log("thread_attend_fusion_requests: atendiendo.", tid);
    // Mientras exista un fusion_req entrante, 
    while ( pthread_mutex_trylock(&threadData[tid].fusion_req) ) {
        log("thread_attend_fusion_requests: Dando ack.", tid);
        pthread_mutex_unlock(&threadData[tid].fusion_ack);
        pthread_mutex_lock(&threadData[tid].fusion_ready);
        log("thread_attend_fusion_requests: Recibido el fusion_ready.", tid);
    }
    log("thread_attend_fusion_requests: se termino de atender.", tid);
}

// Gestión principal del thread. Contiene el ciclo que le permite a cada thread hacer sus funciones.
void* mstParaleloThread(void *p) {

    // Grafo global (esto es para cambiar el tipo de void* a Grafo*)
    Grafo* g = (Grafo*) p;

    // Se obtiene el numero de thread y se inicializan sus estructuras
    id_thread this_thread_id = initThread(g);

    Eje eje_actual;

    log("Comienza thread %lu", this_thread_id);

    // Ciclo principal de cada thread
    while(true){

        log("Vuelve inicio del while");

        // Si otro thread esta en la cola de fusiones de este thread lo
        // notifica que puede fusionarse.

        // FIXME nadie me esta pidiendo 
        log("Atendiendo pedidos de fusion si los hay.");
        thread_attend_fusion_requests(this_thread_id);
        log("listo atendiendo pedidos de fusion.");
        pthread_mutex_unlock(&threadData[this_thread_id].fusion_req);

        log("Buscando nodo mas cercano");

        // Se busca el nodo más cercano que no esté en el árbol, pero que sea
        // alcanzable
        StatusBuscarNodo status = buscarNodo(this_thread_id, eje_actual);
        if (status == NoHayNodosDisponibles) {
            log("Terminando proceso. ya no hay nodos disponibles");

            // Al terminar el thread, es posible tener otro thread pidiendo una
            // fusión. Le da acknoweledge a esos threads para que sepan que ya
            // no tenemos el nodo deseado.
            log("Limpiando mi cola de fusiones");
            thread_attend_fusion_requests(this_thread_id);
            // Esta vez no libero el `lock` de este thread para que no me pidan mas fusiones.
            return NULL;
        }
        if (status == AgmCompleto) {
            log("AGM completo!");
            break;
        };

        log("obtuve prox eje, (%d, %d)",
            eje_actual.nodoOrigen,
            eje_actual.nodoDestino);

        // Intenta capturar el nodo buscado. El valor deuvelto es el dueño del
        // nodo:
        // - Si se capturó con exito, es el mismo ID que el de este thread.
        // - Caso contrario será el ID del thread con el que debe fusionarse.
        id_thread thread_info = colores.capturarNodo(eje_actual.nodoDestino,
                                                     this_thread_id);

        // Si se logra tomar, se procesa.
        if (thread_info == this_thread_id) {
            log("Nodo fue capturado. Agregando a mi AGM");

            // Nodos padre e hijo (en la jerarquia del AGM) del eje a añadir.
            int padre = eje_actual.nodoOrigen;
            int hijo = eje_actual.nodoDestino;

            // Se añade el eje al AGM
            if (padre != hijo) {
                threadData[this_thread_id].agm.insertarEje(eje_actual);
            }

            // Se añaden a la cola de prioridad, los ejes alcanzables por el
            // nuevo nodo.
            add_ejes_alcanzables(this_thread_id, g, hijo);

            // Quita el eje de la cola de prioridad
            // threadData[this_thread_id].ejesVecinos.pop();

            log("Se termino de pintar el nodo libre %d", eje_actual.nodoDestino);

        } else {

            log("Me quiero fusionar con el thread %d por el nodo %d.", thread_info, eje_actual.nodoDestino);

            log("Trylock a mi mismo");
            if ( pthread_mutex_trylock(&threadData[this_thread_id].fusion_req) != 0 ) {
                log("No pude bloquearme a mi mismo. Vuelvo al principio.");
                continue;
            }

            if ( pthread_mutex_trylock(&threadData[thread_info].fusion_req) != 0 ) {
                // Me desbloqueo. Acepto fusiones y vuelvo a intentar con el
                // mismo nodo.
                pthread_mutex_unlock(&threadData[this_thread_id].fusion_req);
                log("No pude bloquear al thread %d. Me desbloqueo y vuelvo al principio.", thread_info);
                continue;
            }

            log("Esperando ack de %d para la fusion", thread_info);
            pthread_mutex_lock(&threadData[thread_info].fusion_ack);

            log("Fusionandome con %d", thread_info);

            // Verifica que el nodo sigue perteneciendo al mismo thread. Esto
            // puede cambiar si el otro thread aceptó una fusion entrante antes
            // de atender a este thread.
            if ( colores.esDueno(eje_actual.nodoDestino, thread_info) ) {

                threadData[this_thread_id].agm.insertarEje(eje_actual);
                if (this_thread_id < thread_info) {
                    fuse(this_thread_id, thread_info,
                         eje_actual.nodoOrigen, eje_actual.nodoDestino);
                } else {
                    fuse(thread_info, this_thread_id,
                         eje_actual.nodoDestino, eje_actual.nodoOrigen);
                }

            } else {
                log("Cuando llegué %d no era mas dueño %d", thread_info, eje_actual.nodoDestino);
            }

            // Desbloqueo el otro thread.
            log("Desbloqueo thread %d", thread_info);
            pthread_mutex_unlock(&threadData[thread_info].fusion_req);
            log("Desbloqueo thread %d", this_thread_id);
            pthread_mutex_unlock(&threadData[this_thread_id].fusion_req);
            log("Doy fusion_ready a %d", thread_info);
            pthread_mutex_unlock(&threadData[thread_info].fusion_ready);
            log("Termina fusion");

        }

    }

    log("AGM listo!");

    // Al terminar el loop, se imprime el resultado y se termina el thread.
    threadData[this_thread_id].agm.imprimirGrafo();

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

    log_init();

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
