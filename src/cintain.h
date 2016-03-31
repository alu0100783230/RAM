#ifndef CINTAIN_H
#define CINTAIN_H

#include "./cinta.h"

class cintaIn : public cinta {
 public:
  explicit cintaIn(ifstream* fich);
  int opCinta(int = 0);
  ~cintaIn();
};

#endif  // CINTAIN_H
