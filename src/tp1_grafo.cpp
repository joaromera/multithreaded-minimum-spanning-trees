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

enum StatusBuscarNodo { Ok, AgmCompleto, NoHayNodosDisponibles };

// Retorna el nodo alcanzable a menor distancia.
// Si el thread todavia no tiene nodos, busca uno libre.
// Si no hay nodos libres, avisa para luego terminar.
StatusBuscarNodo buscarNodo(int thread, Eje &out) {

    // Si no hay ejes en la queue del thread, hay que buscar un nuevo nodo
    if ( threadData[thread].ejesVecinos.empty() ) {
        const int nodo = colores.buscarNodoLibre(thread);

        if (nodo != -1) {
            out = Eje(nodo, nodo, 0);
            return Ok;
        } else {
            // al no encontrar ninguno libre, termina
            return NoHayNodosDisponibles;
        }

    } else {
        // Hay ejes en la queue del thread, busca el más cercano
        while ( ! threadData[thread].ejesVecinos.empty() ) {
            Eje e = threadData[thread].ejesVecinos.top();

            // Si todavía no lo capturó, devuelve el encontrado
            if (! colores.esDueno(e.nodoDestino, thread)) {
                out = e;
                return Ok;
            }

            // Quita este eje de los ejesVecinos
            threadData[thread].ejesVecinos.pop();
        }

        // Sin ejes disponibles y la queue vacía, el agm está completo
        return AgmCompleto;
    }
}

// Gestión principal del thread para completar el AGM
void* mstParaleloThread(void *p) {

    // Grafo compartido entre todos los threads
    Grafo* g = (Grafo*) p;

    // Se obtiene el numero de thread y se inicializan sus estructuras
    int this_thread_id = thread_counter++;
    Thread &this_thread = threadData[this_thread_id];
    this_thread.reset(g->numVertices);

    Eje eje_actual;

    // Empieza a armar el AGM
    while (true) {
        // Atiendo pedidos de fusion recibidos
        // Luego libero el mutex para poder recibir mas
        thread_attend_fusion_requests(this_thread_id);
        pthread_mutex_unlock(&this_thread.fusion_req);

        // Busco nodo libre
        StatusBuscarNodo status = buscarNodo(this_thread_id, eje_actual);

        if (status == NoHayNodosDisponibles) {

            // Antes de terminar, atiende los pedidos de fusión pendientes
            // avisando a esos threads que ya no tenemos el nodo deseado.
            // No libera el lock 'fusion_req' para no recibir más pedidos.
            thread_attend_fusion_requests(this_thread_id);
            return NULL;
        }

        if (status == AgmCompleto) {
            break;
        }

        // Al haber un nodo disponible intenta capturarlo
        // El valor devuelto es el ID de su dueño
        int other_thread_id = colores.capturarNodo(eje_actual.nodoDestino, this_thread_id);
        Thread &other_thread = threadData[other_thread_id];

        // Si se capturó con exito, es el mismo ID que el de este thread
        if (other_thread_id == this_thread_id) {

            // Se añade el eje al AGM del thread
            if (eje_actual.esValido()) {
                this_thread.agm.insertarEje(eje_actual);
            }

            // Se agregan los ejes alcanzables por el nuevo nodo a la queue
            this_thread.add_ejes_alcanzables(g, eje_actual.nodoDestino);

        } else {
            // Caso contrario será el ID del thread con el que debe fusionarse
            // Se hace un pedido de fusión, primero evito recibir nuevos pedidos
            if ( pthread_mutex_trylock(&this_thread.fusion_req) != 0 ) {
                continue;
            }

            // Intento pedir la fusion al thread dueño del nodo
            if ( pthread_mutex_trylock(&other_thread.fusion_req) != 0 ) {
                pthread_mutex_unlock(&this_thread.fusion_req);
                continue;
            }

            // Espero ack del otro thread
            pthread_mutex_lock(&other_thread.fusion_ack);

            // Verifico que el nodo siga perteneciendo al mismo thread
            // Si es así, resuelvo la fusión
            if ( colores.esDueno(eje_actual.nodoDestino, other_thread_id) ) {

                this_thread.agm.insertarEje(eje_actual);

                // Debe predominar el thread de menor id
                if (this_thread_id < other_thread_id) {
                    fuse(this_thread_id, other_thread_id,
                         eje_actual.nodoOrigen, eje_actual.nodoDestino);
                } else {
                    fuse(other_thread_id, this_thread_id,
                         eje_actual.nodoDestino, eje_actual.nodoOrigen);
                }
            }

            // Desbloqueo el otro thread.
            pthread_mutex_unlock(&other_thread.fusion_req);
            pthread_mutex_unlock(&this_thread.fusion_req);
            pthread_mutex_unlock(&other_thread.fusion_ready);
        }
    }

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
    pthread_id.resize(cantThreads); // pthread_id de cada thread.
    threadData.resize(cantThreads); // Data interna de cada thread.
    colores.reset(g->numVertices);  // Cada nodo empieza sin dueño

    // Inicializa cada uno de los threads.
    for (int i = 0; i < cantThreads; ++i) {
        pthread_create(&pthread_id[i], NULL, mstParaleloThread, (void*) g);
    }

    // Espera a que termine cada uno de los threads.
    // En este caso el orden no importa.
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
