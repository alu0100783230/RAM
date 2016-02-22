#ifndef CINTAOUT_H
#define CINTAOUT_H

#include "cinta.h"

class cintaOut : public cinta{
public:
    cintaOut(ifstream* fich);
    int opCinta(int = 0);
    ~cintaOut();
};

#endif // CINTAOUT_H
