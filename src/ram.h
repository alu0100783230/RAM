#ifndef RAM_H
#define RAM_H

#include <vector>
#include <map>
#include <iostream>

#include "./cintain.h"
#include "./cintaout.h"
#include "./instruccion.h"

using namespace std;

#define REG_SIZE 32

class instruccion;
class program;

struct parVectorMap {
  map<short, int> etiqueta_indice;
  vector<instruccion*> vectorInstrucciones;
};

class RAM {
 private:
  cintaIn* tapeIn;
  cintaOut* tapeOut;
  vector<instruccion*> program;
  vector<int> bancoReg;
  int acumulador;
  map<short, int> etiqueta_indice;  // similar a una MMU
  unsigned programCounter;
  bool halt;
  bool salto;

 public:
  RAM(cintaIn* cintain, cintaOut* cintaout, parVectorMap par);
  bool run(bool traza = 0);
  void mostrar();
  inline const void write(int v) { tapeOut->opCinta(v); }
  inline const int read() { return tapeIn->opCinta(); }
  inline int getReg(int i) {
    if (i > 0 && i < REG_SIZE + 1) {
      return bancoReg[i - 1];
    } else {
      cout << "Excepción, intento de acceso fuera de la memoria. Reg [" << i
           << "]" << endl;
      halt = true;
    }
  }
  inline void setReg(int i, int v) { bancoReg[i - 1] = v; }
  inline const int getAc() { return acumulador; }
  inline int setAc(int v) { acumulador = v; }
  void setPC(int v) {
    if (etiqueta_indice.find(v) != etiqueta_indice.end()) {
      programCounter = etiqueta_indice.find(v)->second;
      salto = true;
    } else {
      cout << "Excepción, se ha producido un salto inválido" << endl;
      halt = true;
    }
  }
  inline void setHalt() { halt = true; }
  void reset();
};

#endif  // SRC_RAM_H_
