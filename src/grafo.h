#ifndef GRAFO_H_
#define GRAFO_H_

#include <iostream>
#include <vector>
#include <map>

#define BLANCO -10
#define GRIS -20
#define NEGRO -30


using namespace std;

/* Modificaciones realizadas:
 * Se agrego el nodoOrigen y un constructor
 */
typedef struct Eje {
    int nodoOrigen;
    int nodoDestino;
    int peso;

    Eje(int nodoOrigen, int nodoDestino, int peso) :
        nodoOrigen(nodoOrigen),
        nodoDestino(nodoDestino),
        peso(peso) {}

    Eje() : nodoOrigen(0), nodoDestino(0), peso(0) {}

} Eje;

/* Modificaciones realizadas:
 * Se agrego un constructor por copia y los metodos
 * insertarEje (con distintos argumentos), insertarNodo
 * y getPeso.
 */
class Grafo {

public:

  int numVertices;
  int numEjes;

  map<int,vector<Eje>> listaDeAdyacencias;

  Grafo() {
    numVertices = 0;
    numEjes = 0;
  }

  Grafo(const Grafo &other)
      : numVertices(other.numVertices),
        numEjes(other.numEjes),
        listaDeAdyacencias(other.listaDeAdyacencias) {}

  int inicializar(string nombreArchivo);
  void imprimirGrafo();
  bool esConexo();
  vector<Eje>::iterator vecinosBegin(int num);
  vector<Eje>::iterator vecinosEnd(int num);
  void insertarEje(int nodoA, int nodoB, int peso);
  void insertarEje(const Eje &e);

  /** Registra un nuevo nodo
   *
   * Fue necesario para poder crear un grafo sin vertices
   */
  void insertarNodo(int nodo);

  /** Devuelve el peso de un eje */
  int getPeso(const int nodoA, const int nodoB);

private:

  void incrementarTotalEjes();
};

#endif
