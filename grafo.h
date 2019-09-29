#ifndef GRAFO_H_
#define GRAFO_H_

#include <iostream>
#include <vector>
#include <map>

#define BLANCO -10
#define GRIS -20
#define NEGRO -30


using namespace std;

typedef struct Eje {
  int nodoDestino;
  int peso;

  Eje(int nodoDestino, int peso) {
    this->nodoDestino = nodoDestino;
    this->peso = peso;
  }

  Eje() : nodoDestino(0), peso(0) {}
} Eje;

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
