#include "cinta.h"
#include <iostream>

using namespace std;

/* Introduce los números del fichero en el vector.*/

cinta::cinta(ifstream* fich):
  indexHead(0)
  {
  int aux;
  while(*fich>>aux){
    tape.push_back(aux);
  }
}

void cinta::moveHead(){
    if(indexHead<tape.size()-1)
      indexHead++;
}

ostream& operator<<(ostream& os, const cinta& cint){
  for(int i = 0;i < cint.tape.size(); i++){
    if(i==cint.indexHead)
      os << '[' << cint.tape[i] << "] ";
    else
      os << cint.tape[i]<<" ";
  }
  return os;
}

cinta::~cinta(){}
