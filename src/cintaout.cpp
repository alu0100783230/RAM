#include "cintaout.h"
#include <iostream>

cintaOut::cintaOut(ifstream* fich) : cinta(fich){}

/* La función actúa como lector*/
int cintaOut::opCinta(int p){
    tape.push_back(p);
    moveHead();
}

cintaOut::~cintaOut(){}
