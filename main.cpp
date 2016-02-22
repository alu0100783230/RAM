#include <iostream>
#include "cinta.h"
#include "cintain.h"
#include "cintaout.h"
#include "instruccion.h"
#include "parser.h"
#include "ram.h"

#include <string>

using namespace std;

//devuelve true/false según el fichero existe o no
bool fileExist(string fich){
    fstream file(fich.c_str());
    return file.good();
}

int main(int argc, char *argv[]){
    system("clear");
    //Convertir los parámetros de entrada a string para un mejor análisis
    string* ficheros = new string[3];
    for(int i=0;i<argc-1 && i<3 ;i++){
        ficheros[i] = string(argv[i+1]);
    }
    //variables temporales
    cintaIn*  tapeIn;
    cintaOut* tapeOut;
    parser   prog;
    parVectorMap VM;
    //Comprobar si existe un fichero con el nombre requerido, en caso contrario solicitarlo
    //fichero cintaEntrada
    while(!fileExist(ficheros[0])){
        cout<<"Introduce un fichero válido para la cinta de entrada"<<endl;
        cin>>ficheros[0];
    }
    //Generar el objeto enviandole el fichero al constructor
    ifstream* fich = new ifstream(ficheros[0].c_str());
    tapeIn = new cintaIn(fich);
    fich->close();
    //fichero cintaSalida
    while(!fileExist(ficheros[1])){
        cout<<"Introduce un fichero válido para la cinta de salida"<<endl;
        cin>>ficheros[1];
    }
    fich = new ifstream(ficheros[1].c_str());
    tapeOut = new cintaOut(fich);
    fich->close();
    //fichero programa
    while(!fileExist(ficheros[2])){
        cout<<"Introduce un fichero válido con el programa"<<endl;
        cin>>ficheros[2];
    }
    fich = new ifstream(ficheros[2].c_str());
    short select = 0;
    while(select!=1 && select!=2){
      cout<<"¿Activar modo verbose para la compilación?\n"
            "[1] No\n"
            "[2] Si  "<<endl;
      cin>>select;
    }
    VM =  prog.parsear(fich,select-1);
    fich->close();
    //Construir máquina
    RAM maquina(tapeIn,tapeOut,VM);
    if(VM.vectorInstrucciones.size()>0){
      cout<<endl<<"La máquina ha sido creada correctamente"<<endl;
      char c;
      while(select!=4){
        select=-9;
        while((select<1 || select>3) && select!=4){
          cout<<"[1] Ver estado\n"
                "[2] Ejecutar\n"
                "[3] Ejecutar modo paso a paso\n"
                "[4] Salir"<<endl;
          cin>>select;
        }
        switch (select) {
          case 1 :
            system("clear");
            scanf("%c",&c);
            maquina.mostrar();
          break;
          case 2 :
            maquina.run();
            maquina.mostrar();
          break;
          case 3:
            system("clear");
            maquina.run(true);
          break;
        }
        ofstream fichout(ficheros[1].c_str());
        fichout<<(*tapeOut);
        fichout.close();
      }
    }
}
