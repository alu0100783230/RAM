#include "instruccion.h"

//Constructor e la clase abstracta
instruccion::instruccion(short op, short tipo, int val):
  opcode(op),
  especial(tipo),
  value(val)
  {}
// Operador << sobrecargado
ostream& operator<<(ostream& os, const instruccion& ins){
  os << ins.opcode << "_" << ins.especial << '_' << ins.value;
  return os;
}

//Constructores de las clases derivadas
instruccionReg::instruccionReg(short op, short tipo, int value) : instruccion(op,tipo,value){}

instruccionCinta::instruccionCinta(short op, short tipo, int value) : instruccion(op,tipo,value){}

instruccionAlu::instruccionAlu(short op, short tipo, int value) :  instruccion(op,tipo,value){}

instruccionSalto::instruccionSalto(short op, short etiquetaSalida) :  instruccion(op,-1,etiquetaSalida) {}

instruccionHalt::instruccionHalt() : instruccion(-1,-1,-1){}

// Comportamiento de cada instrucción según de qué tipo sea

int instruccionAlu::valor(RAM* r){
  if(especial==NADA)      	       //NADA 1   → retorno el contenido de R1
    return r->getReg(value);
  else if(especial==IGUAL)         //IGUAL 1   → retorna 1
    return value;
  else if(especial==ESTRELLA)      //IGUAL* 1   → retorna el contenido de lo apuntado por R1
    return r->getReg(r->getReg(value));
}

bool instruccionAlu::ejecutar(RAM* r){
  switch (opcode) {
    case ADD :
      r->setAc(r->getAc() + valor(r));
    break;
    case SUB :
      r->setAc(r->getAc() - valor(r));
    break;
    case MUL :
      r->setAc(r->getAc() * valor(r));
    break;
    case DIV :
      r->setAc(r->getAc() / valor(r));
    break;
  }
}

bool instruccionCinta::ejecutar(RAM* r){
  switch (opcode) {
    case READ :                 //leer la cinta de entrada. _No se puede leer el acumulador_
      if(especial == NADA)      //READ 1   → lo que lea la cabeza de lectura a R1
        r->setReg(value,r->read());
      else                      //READ *1  → lo que lea la cabeza de lectura a la dir cotnenida en R1
        r->setReg(r->getReg(value),r->read());
    break;
    case WRITE:                 //escribir la cinta de salida
      if(especial == NADA)      //WRITE 1   → lo que está en R1 se escribe en dónde esté la cabeza de escritura
        r->write(r->getReg(value));
      else if(especial == IGUAL)//WRITE = 1 → escribir 1 en dónde esté la cabeza de escritura
        r->write(value);
      else                      //WRITE *1  → lo que esté en lo apuntado por R1 se escribe en la cinta de escritura
        r->write(r->getReg(r->getReg(value)));
    break;
  }
}

bool instruccionSalto::ejecutar(RAM* r){
  switch (opcode) {
    case JUMP :                   //Salto a etiqueta directo
      r->setPC(value);
    break;
    case JGTZ :                   //Salto a etiqueta si R0 > 0
      if(r->getAc() > 0)
        r->setPC(value);
    break;
    case JZERO :                  //Salto a etiqueta si R0 = 0
      if(r->getAc() == 0)
        r->setPC(value);
    break;
  }
}

bool instruccionReg::ejecutar(RAM* r){
  switch (opcode) {
    case LOAD:                     //cargar en R0 un valor
      if(especial == NADA)         //LOAD 3   → en R0 lo que haya en R3
        r->setAc(r->getReg(value));
      else if(especial == IGUAL)   //LOAD = 3 → en R0 el literal 3
        r->setAc(value);
      else                         //LOAD *3  → en R0 el contenido de lo apuntado por R3
        r->setAc(r->getReg(r->getReg(value)));
    break;
    case STORE:                    //guarda el contenido de R0 en un reg
      if(especial == NADA)         //STORE 3   → lo que está en R0 se pone en R3
        r->setReg(value,r->getAc());
      else                         //LOAD *3  → en R0 el contenido de lo apuntado por R3
        r->setReg(r->getReg(value),r->getAc());
    break;
  }
}

bool instruccionHalt::ejecutar(RAM* r){
  r->setHalt();
}

// Decodificación semántica [EXPERIMENTAL]

string instruccion::decodificacion(){
  if(especial == NADA)
    return "el registro R";
  else if(especial == IGUAL)
    return " ";
  else
    return "el resgistro al que apunta R";
}

string instruccion::queCosa(){
  if(especial == NADA || especial == IGUAL)
    return " ";
  else
    return "lo que haya en ";
}

string instruccionAlu::decodificacionSemantica(){
  switch (opcode) {
    case ADD :
      return "Leer Acumulador, sumarle "+queCosa()+decodificacion()+to_string(value)+"y guardar en el Acumulador";
    break;
    case SUB :
      return "Leer Acumulador, restarle "+queCosa()+decodificacion()+to_string(value)+"y guardar en el Acumulador";
    break;
    case MUL :
      return "Leer Acumulador, multiplicarlo por "+queCosa()+decodificacion()+to_string(value)+"y guardar en el Acumulador";
    break;
    case DIV :
      return "Leer Acumulador, dividirlo por "+queCosa()+decodificacion()+to_string(value)+"y guardar en el Acumulador";
    break;
  }
}

string instruccionReg::decodificacionSemantica(){
  switch (opcode) {
    case LOAD :
      return "Guardar en el Acumulador "+queCosa()+decodificacion()+to_string(value);
      break;
    case STORE :
      return "Guardar en "+decodificacion()+to_string(value)+" lo que haya en el Acumulador";
    break;
  }
}

string instruccionCinta::decodificacionSemantica(){
  switch (opcode) {
    case READ :
      if(especial == NADA)
        return "Guardar en el registro "+to_string(value)+" lo que lea la cabeza de lectura";
      else
        return "Guardar en "+decodificacion()+to_string(value)+" lo que lea la cabeza de lectura";
      break;
    case WRITE :
      return queCosa()+decodificacion()+to_string(value)+ " se envía a la cabeza de escritura";
    break;
  }
}

string instruccionSalto::decodificacionSemantica(){
  switch (opcode) {
    case JUMP :
      return "Salto absoluto a la dirección virtual "+to_string(value);
      break;
    case JGTZ :
      return "Salto si Acumulador > 0 a la dirección virtual "+to_string(value);
      break;
    case JZERO :
      return "Salto si Acumulador = 0 a la dirección virtual "+to_string(value);
      break;
  }
}

string instruccionHalt::decodificacionSemantica(){
  return "Fin de la ejecución";
}
