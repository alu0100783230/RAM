#include "./cintain.h"

cintaIn::cintaIn(ifstream* fich) : cinta(fich) {}
/* La función actúa como lector*/
int cintaIn::opCinta(int p) {
  int v = tape.at(indexHead);
  moveHead();
  return v;
}
cintaIn::~cintaIn() {}
