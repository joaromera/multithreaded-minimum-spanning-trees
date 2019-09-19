#pragma once
#include <vector>
#include <atomic>

using namespace std;

class ColorDeNodo {

  bool capturar(int nodo, int thread);
  bool cambiarColor(int nodo, int thread);
  int verColor(int nodo);
  void fusionar(int threadGanador, int threadPerdedor);

private:

  int threadId;
};