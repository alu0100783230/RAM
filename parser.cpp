#include "parser.h"

parser::parser()  {}


/* El cometido de esta función es analizar sintáctica y semánticamente
 * el programa a fin de encontrar posibles errores y evitar operaciones
 * ilegales en la máquina. Para ello lee el fichero de entrada por lineas, las
 * cuales a su vez subdivide en tokens que son analizados individualmente.
 * Un buffer es empleado en ciertas situaciones de incertidumbre para uardar
 * tokens que en el momento podrían ser tanto opcodes como etiquetas.
 *
 * Funciona como un dfa con la siguietne tabla de transiciones:
 *
 | Estado | [jJ]* | :     | /d    | [*,=] | Otro caso            | Consideraciones                                          |
 |--------+-------+-------+-------+-------+----------------------+----------------------------------------------------------|
 |      0 |     2 | 3     | Error | Error | Guardar en buffer, 1 | Buff puede contener un opcode o una etiqueta de entrada  |
 |      1 |     2 | 3     | 0     | 4     | Error                | Ya hemos averiguado qué es el buffer                     |
 |      2 | Error | Error | Error | Error | 0                    | El token contiene la etiqueta de salida, Fin instrucción |
 |      3 |     2 | Error | Error | Error | 5                    | El token contiene el opcode                              |
 |      4 | Error | Error | 0     | Error | Error                | Fin instrucción                                          |
 |      5 | Error | Error | 0     | 4     | Error                | La instrucción no puede ser de salto                     |

 * Además, para cuaquier estado en caso de encontrar ';' se aborta el análisis de toda la línea.
 * Esta arquitectura permite dotar de una mayor robustez al programa al tiempo que
 * que facilita al usuario la detección de errores mediante el uso de un debugger
 * muy primitivo asociado a la misma. De igual forma, provee al programa de la capacidad
 * de trabajar con instrucciones con cierto espaciado ambiguo, p.e.  LOAD =1 vs LOAD = 1 ó etiqueta: vs etiqueta :
 *
 * La función devuelve un struct con un vector de instrucciones (traducidas a un lennguaje máquina
 * interno basado en números enteros en vez de cadenas para un mejor aprovechamiento de los recursos
 * del computador) y un map de pares etiquetas/etiquetas codificadas como enteros, en este caso, negativos.
 * La existencia de este objeto map se justifica en una pseudo implementación de MMU
 * capaz de detectar saltos a etiquetas sin definir
 *
 * En caso de que el análisis falle, el vector devuelto tendrá tamaño 0
 */

parVectorMap parser::parsear(ifstream* fich, bool verbose){
  //Inicialización de variables implicadas
  errores = 0;
  countLine = 0;
  estado = 0;
  etiquetasCompletamenteDefinidas = 0;
  etiquetasPendientes = 0;
  traducirEtiqueta("string para ocupar la posición 0, no tiene utilidad");
  bool comentario = false;
  //Inicialización de las expresiones regulares implicadas
  regex coments ("(;)(.*)");
  regex igual("^(.*)(=)(.*)$");
  regex estrella("(\'*')");
  regex tag("^(.*)(:)$");
  regex salto("^[jJ](.*)");
  regex digito("^(.*)[[:digit:]]+");
  regex halt("(halt)|(HALT)");
  //Declaración de variables auxiliares
  string buff;
  string linea;
  instruccionTemp tmpIns;
  while(getline(*fich,linea)){
    countLine++;
    istringstream flujo(linea);
    string token;
    tmpIns=instruccionTemp();
    while(flujo>>token){
      bool palabraReservada = false;
      if(verbose){cout<<endl<<"'"<<token<<"' -> ";}
      if(regex_match(token,coments)){
          //si encontramos ';' saltar a la siguiente línea
          if(verbose){cout<<"Comentario, saltando línea";}
          comentario = true;
          break;
      }else if(regex_match(token,halt)){
        if(verbose){cout<<"Fin del programa";}
        tmpIns.opcode = HALT;
      }else{
        if(regex_match(token,igual) || regex_match(token,estrella)){
          if(estado==1 || estado==5){
            if(estado==1)
              tmpIns.opcode=traducirOpcode(buff);
            //Lo siguiente será un número
            if(token[0]=='='){
              if(tmpIns.opcode!=1 && tmpIns.opcode!=2)
                tmpIns.especial=IGUAL;
              else{
                cout<<"[!] Error, la codificación '=' es inválida para las instrucciones READ Y STORE\n";
                break;
              }
            }else
              tmpIns.especial=ESTRELLA;
            if(verbose){cout<<"Instrucción compleja '"<<token[0]<<"' ";}
            token = token.substr(1);
            palabraReservada = true;
            estado=4;
          }else
            errorOut(token);
        }
        if(regex_match(token,digito)){
          if(estado==1){
            //Fin de instrucción simple
            tmpIns.operando=atoi(token.c_str());
            if(verbose){cout<<"Instrucción simple, '"<<buff<<"' fue el opcode "<<traducirOpcode(buff);}
            tmpIns.opcode = traducirOpcode(buff);
            estado=0;
            palabraReservada = true;
          }else if(estado==5 || estado==4){
            //Fin de instrucción compuesta
            tmpIns.operando=atoi(token.c_str());
            if(verbose){cout<<" de valor "<<atoi(token.c_str())<<", '"<<buff<<"' fue el opcode "<<traducirOpcode(buff);}
            tmpIns.opcode = traducirOpcode(buff);
            estado=0;
            palabraReservada = true;
          }else
            errorOut(token);
        }
        if(regex_match(token,tag)){
          if(estado==1){
            //Lo siguiente será el descriptor de etiqueta
            etiqueta_indice.insert(pair<short,int>(traducirEtiqueta(buff),(vectorInstrucciones.size())));
            if(verbose){cout<<"Ahora espero un opcode, '"<<buff<<"' era la etiqueta de entrada "<<traducirEtiqueta(buff);}
            estado=3;
            palabraReservada = true;
          }else if(estado==0){
            //lo siguiente será un opcode
            string aux = token.substr(0, token.size()-1);
            etiqueta_indice.insert(pair<short,int>(traducirEtiqueta(aux),(vectorInstrucciones.size())));
            if(verbose){cout<<"Ahora espero un opcode, '"<<aux<<"' es la etiqueta de entrada "<<traducirEtiqueta(buff);}
            estado=3;
            palabraReservada = true;
          }else
            errorOut(token);
        }
        if(regex_match(token,salto)){
          if(estado==1 || estado==3 || estado==0){
            //Lo siguiente será el descriptor de etiqueta
            tmpIns.opcode = traducirOpcode(token);
            etiquetasPendientes++;
            if(verbose){cout<<"Esto es un salto, ahora espero una etiqueta a la que saltar";}
            estado=2;
            palabraReservada = true;
          }else
            errorOut(token);
        }
        if(!palabraReservada){
          if(estado==0){
            //cargamos en el buffer el token en espera de averiguar si es una instrucción o una etiqueta
            if(verbose){cout<<"Guardado en buffer";}
            estado=1;
            buff=token;
          }else if(estado==3){
            //lo obtenido es el opcode de la instrucción
            if(verbose){cout<<"Este es el opcode "<<traducirOpcode(token);}
            tmpIns.opcode = traducirOpcode(token);
            estado=5;
            buff = token;
          }else if(estado==2){
            //lo obtenido es la etiqueta a la que se salta
            tmpIns.tagSalida = traducirEtiqueta(token);
            if(verbose){cout<<"Esta es la etiqueta de salida "<<tmpIns.tagSalida<<endl;}
            estado=0;
          }else
            errorOut(token);
        }
      }
    }
    //construir instrucción
    //cout<<tmpIns.opcode<<endl;
    if(tmpIns.opcode>-1)          //!comentario
      vectorInstrucciones.push_back(crearInstruccion(tmpIns));
    comentario=false;
  }
  parVectorMap resultado;
  if(etiquetasCompletamenteDefinidas<etiquetasPendientes){
    cout<<"[!] Error, existen saltos a etiquetas sin definir"<<endl;
    errores++;
  }
  if(errores==0)
    resultado  = {etiqueta_indice,vectorInstrucciones};
  else
    resultado = {etiqueta_indice,vector<instruccion*>(0)};
  return resultado;
}

parser::~parser(){}

/* Comprueba si existe la clave string en el mapa,
 * si no es así la crea. En cualquier cualquier caso
 * retorna la etiqueta codificada                 */
short parser::traducirEtiqueta(string et){
  //Se emplean números negativos para las etiquetas
  if(hash.count(et)){
    etiquetasCompletamenteDefinidas++;
    return hash.find(et)->second;
  }else{
    hash.insert(pair<string,short>(et,-hash.size()));
    return -hash.size()+1;
  }
}

// Devuelve la instrucción codificada al lenguaje máquina interno
short parser::traducirOpcode(string op){
  if(op=="LOAD"||op=="load")
      return LOAD;
  else if(op=="STORE"||op=="store")
      return STORE;
  else if(op=="READ"||op=="read")
      return READ;
  else if(op=="WRITE"||op=="write")
      return WRITE;
  else if(op=="ADD"||op=="add")
      return ADD;
  else if(op=="SUB"||op=="sub")
      return SUB;
  else if(op=="MUL"||op=="mul")
      return MUL;
  else if(op=="DIV"||op=="div")
      return DIV;
  else if(op=="JUMP"||op=="jump")
      return JUMP;
  else if(op=="JGTZ"||op=="jgtz")
      return JGTZ;
  else if(op=="JZERO"||op=="jzero")
      return JZERO;
  else if(op=="HALT"||op=="halt")
      return HALT;
  else
    cout<<"[!] Error, no existe el opcode "<<op;
    return -1;
}

// Debugger, en función del estado en el que esté el dfa deduce que tipo de error ha ocurrido
void parser::errorOut(string s){
  cout<<"[!] Error, no se esperaba el símbolo "<<s<<" en la línea "<<countLine<<".\n";
  switch (estado) {
    case 0:
      cout<<"Se esperaba un opcode o una etiqueta de entrada\n";
    break;
    case 1:
      cout<<"Se esperaba un opcode de salto, un entero, ':' , '*' o '='\n";
    break;
    case 2:
      cout<<"Se esperaba una etiqueta de salida\n";
    break;
    case 3:
      cout<<"Se esperaba un opcode\n";
    break;
    case 4:
      cout<<"Se esperaba un entero\n";
    break;
    case 5:
      cout<<"Se esperaba un entero, '*' o '='\n";
    break;
  }
  errores++;
}

/* Convierte el struct de instrucción temporal que ha creado el dfa en
 * una instrucción completa */
instruccion* parser::crearInstruccion(instruccionTemp tmp){
  short op = tmp.opcode;
  if(op == LOAD || op==STORE)
    return new instruccionReg(tmp.opcode, tmp.especial,tmp.operando);
  else if(op == READ || op == WRITE)
    return new instruccionCinta(tmp.opcode, tmp.especial,tmp.operando);
  else if(op == ADD || op == SUB || op == MUL || op == DIV)
    return new instruccionAlu(tmp.opcode, tmp.especial,tmp.operando);
  else if(op == JUMP || op == JGTZ || op == JZERO)
    return new instruccionSalto(tmp.opcode, tmp.tagSalida);
  else
    return new instruccionHalt();
}
