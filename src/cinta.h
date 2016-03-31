#ifndef CINTA_H
#define CINTA_H

#include <vector>
#include <fstream>

using namespace std;

/*Clase abstracta de la que heredarán la cinta de entrada y de salida*/

class cinta {
 protected:
  int indexHead;
  vector<int> tape;

 public:
  explicit cinta(ifstream* fich);
  ~cinta();
  virtual int opCinta(int p = 0) = 0;

 protected:
  friend ostream& operator<<(ostream& os, const cinta& tape);
  virtual void moveHead();
};

#endif  // CINTA_H
