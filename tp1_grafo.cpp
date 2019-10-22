#include <algorithm>
#include <atomic>
#include <cassert>
#include <chrono>
#include <limits>
#include <pthread.h>
#include <queue>
#include <semaphore.h>
#include <stack>
#include <stdlib.h>
#include <time.h>
#include <vector>

#include "cola_prioridad.h"
#include "colores.h"
#include "experimentacion.h"
#include "fusiones.h"
#include "globales.h"
#include "grafo.h"
#include "log.h"
#include "thread.h"

using namespace std;

using id_thread = int32_t;

// -----------------------------------------------------------------------------
// Funciones
// -----------------------------------------------------------------------------

enum StatusBuscarNodo { Ok, AgmCompleto, NoHayNodosDisponibles };

// Retorna el nodo alcanzable a menor distancia.
// Si el thread todavia no tiene nodos, busca uno libre.
// Si no hay nodos libres, avisa para luego terminar.
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

// Gestión principal del thread. Contiene el ciclo que le permite a cada thread hacer sus funciones.
void* mstParaleloThread(void *p) {

    // Grafo global (esto es para cambiar el tipo de void* a Grafo*)
    Grafo* g = (Grafo*) p;

    // Se obtiene el numero de thread y se inicializan sus estructuras
    int this_thread_id = thread_counter++;
    threadData[this_thread_id].reset(g->numVertices);

    Eje eje_actual;

    log("Comienza thread %lu", this_thread_id);

    // Ciclo principal de cada thread
    while(true){
        // Atiendo pedidos de fusion recibidos y libero el mutex para poder recibir mas
        thread_attend_fusion_requests(this_thread_id);
        pthread_mutex_unlock(&threadData[this_thread_id].fusion_req);

        // Busco nodo libre
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
            threadData[this_thread_id].add_ejes_alcanzables(g, hijo);

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

    if (cantThreads < 1) {
        cerr << "El número de threads debe ser igual o mayor a 1" << endl;
    }

    if (g->numVertices == 0) {
        if (imprimirResultado) {
            cout << endl << "********** RESULTADO *********** " << endl;
            Grafo().imprimirGrafo();
        }
        return;
    }

    // Se inicializan las estructuras globales segun cant threads
    pthread_id.resize(cantThreads); // pthread_id de c/u thread.
    threadData.resize(cantThreads); // Data interna de c/u thread.
    colores.reset(g->numVertices); // Cada nodo empieza sin dueño

    // Inicializa cada uno de los threads.
    for (int i = 0; i < cantThreads; ++i) {
        pthread_create(&pthread_id[i], NULL, mstParaleloThread, (void*) g);
    }

    // Espera a que termine cada uno de los threads. En este caso el orden no
    // importa.
    for (int i = 0; i < cantThreads; ++i) {
        pthread_join(pthread_id[i], NULL);
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
