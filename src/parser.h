#ifndef PARSER_H
#define PARSER_H

#include <fstream>
#include <iostream>
#include <vector>
#include <regex>
#include <string>
#include <map>

#include "./instruccion.h"
#include "./ram.h"

class instruccion;

using namespace std;

class parser {
 private:
  vector<instruccion*> vectorInstrucciones;
  map<string, short> hash;
  map<short, int> etiqueta_indice;  // pares etiqueta / índice de instrucción
  int errores;
  int countLine;
  short estado;
  short etiquetasCompletamenteDefinidas;  // etiquetas que tienen instrucción
                                          // llamante y receptora
  short etiquetasPendientes;  // etiqetas que aún no tienen receptor

 public:
  parser();
  ~parser();
  parVectorMap parsear(ifstream* fich, bool v);

 private:
  short traducirOpcode(string op);
  short traducirEtiqueta(string et);
  void errorOut(string s);
  instruccion* crearInstruccion(instruccionTemp);
};

#endif  // SRC_PARSER_H_
