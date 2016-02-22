#ifndef INSTRUCCION_H
#define INSTRUCCION_H

#include <vector>
#include <string>
#include "ram.h"

using namespace std;


class RAM;

enum ins{
    LOAD,
    STORE,
    READ,
    WRITE,
    ADD,
    SUB,
    MUL,
    DIV,
    JUMP,
    JGTZ,
    JZERO,
    HALT
};

enum esp{
  NADA,
  IGUAL,
  ESTRELLA
};

struct instruccionTemp{
    int tagSalida;
    int opcode;
    int operando;
    int especial;
};

class instruccion{                            //Clase instrucción abstracta
protected:
  short especial;
  short opcode;
  int value;
public:
  instruccion(short op, short tipo, int value);
  ~instruccion();
  virtual bool ejecutar(RAM* r) = 0;
  virtual string decodificacionSemantica() = 0;
protected:
  friend ostream& operator<<(ostream& os, const instruccion& ins);
  string decodificacion();
  string queCosa();
};

class instruccionSalto : public instruccion{  //JUMP, JGTZ, JZERO
public:
  instruccionSalto(short op, short etiquetaSalida);
  bool ejecutar(RAM* r);
  string decodificacionSemantica();
};

class instruccionAlu : public instruccion{    //ADD, SUB, MUL, DIV
public:
  instruccionAlu(short op, short tipo, int value);
  bool ejecutar(RAM* r);
  string decodificacionSemantica();
  int valor(RAM* r);                                 //decodifica la información de '=' y '*' y retorna el valor con el que se trabaja
};

class instruccionReg : public instruccion{    //LOAD, STORE
public:
  instruccionReg(short op, short tipo, int value);
  bool ejecutar(RAM* r);
  string decodificacionSemantica();
};

class instruccionCinta : public instruccion{  //READ, WRITE
public:
  instruccionCinta(short op, short tipo, int value);
  bool ejecutar(RAM* r);
  string decodificacionSemantica();
};

class instruccionHalt : public instruccion{   //HALT
public:
  instruccionHalt();
  bool ejecutar(RAM* r);
  string decodificacionSemantica();
};

#endif // PROGRAM_H
