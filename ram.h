#ifndef RAM_H
#define RAM_H

#include "cintain.h"
#include "cintaout.h"
#include "instruccion.h"

#include <vector>
#include <map>
#include <iostream>

using namespace std;

class instruccion;
class program;

struct parVectorMap{
  map<short,int> etiqueta_indice;
  vector<instruccion*> vectorInstrucciones;
};

class RAM{
private:
  cintaIn*    tapeIn;
  cintaOut*   tapeOut;
  vector<instruccion*>    program;
  vector<int> bancoReg;
  int acumulador;
  map<short,int> etiqueta_indice;                       //similar a una MMU
  int programCounter;
  bool halt;
  bool salto;
public:
  RAM(cintaIn* cintain, cintaOut* cintaout, parVectorMap par);
  bool run(bool traza = 0);
  void mostrar();
  inline const void write(int v){ tapeOut->opCinta(v); }
  inline const int read(){ return tapeIn->opCinta(); }
  inline const int getReg(int i){ return bancoReg[i-1]; }
  inline void setReg(int i, int v){ bancoReg[i-1] = v; }
  inline const int getAc(){ return acumulador;}
  inline int setAc(int v){ acumulador = v;}
  inline void setPC(int v){ programCounter = etiqueta_indice.find(v)->second; salto = true;}
  inline void setHalt(){ halt = true; }
};

#endif // RAM_H
