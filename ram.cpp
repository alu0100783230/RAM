#include "ram.h"

RAM::RAM(cintaIn* cintain, cintaOut* cintaout, parVectorMap par):
  programCounter(0),
  halt(false),
  program(par.vectorInstrucciones),
  etiqueta_indice(par.etiqueta_indice),
  tapeIn(cintain),
  tapeOut(cintaout),
  acumulador(0),
  salto(false),
  bancoReg(REG_SIZE)
  {}

bool RAM::run(bool traza){
  char c;
  scanf("%c",&c);
  while(!halt){
    salto = false;
    cout<<"[Program counter] "<<programCounter<<endl;
    if(traza)
      mostrar();
    program[programCounter]->ejecutar(this);
    if(!salto && !halt)
      programCounter++;
  }
  cout<<"Fin de la ejecución, enter para continuar"<<endl;
  scanf("%c",&c);
  system("clear");
}


// Muestra la máquina formateada
void RAM::mostrar(){
  char c;
  cout<<"Cinta de entrada : "<<*tapeIn<<"\n"
        "Cinta de salida  : "<<*tapeOut<<"\n\n"
        "REGS: "<<endl;
  printf("%10s|","Acumulador");
  for(int i=0;i<REG_SIZE;i++){
    printf("%3d|",i+1);
  }
  cout<<endl;
  printf("%10d|",acumulador);
  for(int i=0;i<REG_SIZE;i++){
    printf("%3d|",bancoReg[i]);
  }
  //Direcciones virtuales
  cout<<"\n\nMMU: "<<endl;
  for(int i = 0; i<etiqueta_indice.size();i++){
    cout<<-1-i<<"/"<<etiqueta_indice[-i-1]<<", ";
  }
  // Bloque para extraer una sección del programa de un máximo de 10 líneas e imprimirla
  int min = programCounter-5, max = programCounter+5;
  for(int i=0;i<5;i++){
    if(min<0){
      min++;
      if(max<program.size()-1)
        max++;
    }
    if(max>program.size()-1){
      max--;
      if(min>1)
        min--;
    }
  }
  cout<<"\n\nMEM:\n";
  if(min!=0)
    cout<<"\t\t. . ^ . ."<<endl;
  else
    cout<<"\t\t_________"<<endl;
  for(unsigned i = min; i<=max; i++){
    if(i==programCounter)
      cout<<"\t["<<i<<"]\t"<<*program[i]<<endl;
    else
      cout<<"\t "<<i<<" \t"<<*program[i]<<endl;
  }
  if(max!=program.size()-1)
    cout<<"\t\t. . v . ."<<endl;
  else
    cout<<"\t\t_________"<<endl;
  cout<<"\n\nDECODIFICACIÓN SEMÁNTICA:\n\n"<<program[programCounter]->decodificacionSemantica()<<endl;
  scanf("%c",&c);
  system("clear");
}

void RAM::reset(){
  cout<<"Reset"<<endl;
  for(unsigned i = 1; i < REG_SIZE + 1; i++){
    setReg(i,0);
  }
  halt = false;
  salto = false;
  programCounter = 0;
  setAc(0);
}
