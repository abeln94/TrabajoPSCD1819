//******************************************************************
// File:  servidor.cpp
// Authors:   Daniel González
//            NOMBRES
//            NOMBRES
//            NOMBRES
// Date:   Diciembre 2018
//*****************************************************************


#include "Socket.hpp"
#include "Scoreboard.hpp"
#include "Tupla.hpp"
#include <iostream>
#include <sstream>
#include <thread>
#include <cstring>
#include <cstdlib>
#include <signal.h>

#include <mutex>
#include <condition_variable>


using namespace std;

sig_atomic_t end_mark = 0;

//###################################################################
//Pequeño controlador de exclusion mutua (no mas de 40 lineas)
class SafeSYS {
  public:
    //Constructor
    SafeSYS(){
      counter = 0;
    }

    void endPH3(){
      unique_lock<mutex> lck(mtx);
      while(counter != 0){
        end.wait(lck);
      }
    }

    int sum(int n){
      unique_lock<mutex> lck(mtx);
      counter = counter + n;
      if(counter == 0){
        end.notify_one();
      }
      return counter;
    }

    void safe_print(string c){
      unique_lock<mutex> lck(mtx);
      cout << c << endl;
    }

    void err_safe_print(string c){
      unique_lock<mutex> lck(mtx);
      cerr << c << endl;
    }

  private:
    int counter;
    mutex mtx;
    condition_variable end;
};
//###################################################################
//Funciones encargadas de la comunicación exterior

/*
void control(Socket& soc, int& socket_fd, SafeSYS& sys){
  // en espera
}
*/

void newclient(Canal cliente, SafeSYS& sys, Scoreboard& pizarra){
  sys.sum(1);
	
	string mensaje, respuesta;
	try{
		bool err = false, crear = false;
		while(!err){
			//repetimos indefinidamente hasta que el cliente se vaya
			
			//esperamos el mensaje
			cliente >> mensaje;

			// Tratado de msg del tipo "P1[bla]"
			
			Tupla tupla(mensaje[1]-'0');//tamaño
			tupla.from_string(mensaje.substr(2)); //tupla
			
			switch(mensaje[0]){ //operacion
				case 'P':
					pizarra.PN(tupla);
					respuesta = "OK";
					break;
				case 'R':
					respuesta = pizarra.RN(tupla).to_string();
					break;
				case 'r':
					respuesta = pizarra.readN(tupla).to_string();
					break;
			}
			
			cliente << respuesta;
		}
  }catch(...){
		//el cliente se ha desconectado
		cout << "[x] cliente desconectado" << endl;
	}
  sys.sum(-1);
}

//###################################################################
//Función encargada del tratamiento de señales
void sig_handler(int signo){
  end_mark = 1;
  cerr << endl;
  if (SIGINT == signo){
    cerr << "[x]Señal de interrupción capturada, cerrando servidor..." << endl;
  } else if(SIGTSTP == signo){
    cerr << "[x]Señal de finalización capturada, cerrando servidor..." << endl;
  } else if(SIGQUIT == signo){
    cerr << "[x]Señal de cierre capturada, cerrando servidor..." << endl;
  } else {
    cerr << "[x]Señal desconocida capturada, cerrando servidor..." << endl;
  }
  cerr << endl;
  cerr << "[x]Advertencia: Es posible que aun se ejecuten algunas sentencias..."<<endl;
  cerr << endl;
  //signal(signo,sig_handler);
}

//###################################################################
//Main func.
int main(int argc, char * argv[]) {
  //Declaración de variables
  const string localhost = "localhost";
  string quiensoy;
  string ip_serv, ipmia;
  int port_localhost;
  int port_serv;


  //COMPROBACIONES
  if(argc == 6){
    quiensoy = argv[1];
    ipmia = argv[2];
    port_localhost = atoi(argv[3]);
    ip_serv = argv[4];
    port_serv = atoi(argv[5]);
  } else {
    cout << "[x]Advertencia"<< endl;
    cout << "[x]Error: Número de argumentos invalido"<< endl;
    cout << "./<ejecutable> <quiensoy> <ip_mia> <puerto local> <ip_serv> <puerto_serv>"<<endl;
    exit(0);
  }

  if(quiensoy != "1" && quiensoy != "2" && quiensoy != "3"){
    cout << "[x]Advertencia"<< endl;
    cout << "[x]Error: Valor para <quiensoy> invalido"<< endl;
    cout << "./<ejecutable> <quiensoy> <puerto local> <ip_serv> <puerto_serv>"<<endl;
    exit(0);
  }

  cout << ">>" << endl;
  cout << ">> INICIANDO SERVICIO" << endl;
  cout << ">>" << endl;

  //###################################################//
  //################## P H A S E : 1 ##################//
  //###################################################//
  //Creacion de sockets y protección ante señales
  cout << "[x] Inicio Fase 1 . . ." << endl;

  //Terminacion controlada
  //Protección frente "^C", "^\", "^Z"
  signal(SIGPIPE, SIG_IGN);
  struct sigaction sig_han;
  sig_han.sa_handler = sig_handler;
  sigemptyset(&sig_han.sa_mask);
  sig_han.sa_flags = 0;
  sigaction(SIGINT, &sig_han, NULL);
  sigaction(SIGTSTP, &sig_han, NULL);
  sigaction(SIGQUIT, &sig_han, NULL);

  //Conexión con clientes:
  Servidor localServer(port_localhost, 100);


  cout << "[x] Fase 1 completada." << endl;
  //###################################################//
  //################## P H A S E : 2 ##################//
  //###################################################//
  //Conexión con serv
  cout << "[x] Inicio Fase 2 . . ."  << endl;
  //###################
  //sting miip = getmiip();

	Canal servidor(ip_serv,port_serv, 10, 1000);

  // i xxx.xxx.xxx.xxx PPPPP
	servidor << quiensoy + " " + ipmia + " " + to_string(port_localhost);
	
	//no es necesaria una respuesta


  cout << "[x] Fase 2 completada." << endl;
  //###################################################//
  //################## P H A S E : 3 ##################//
  //###################################################//
  //Servicio iniciado, comandos disponibles
  cout << "[x] Inicio Fase 3 . . ."  << endl;

  SafeSYS system;
  Scoreboard board;

  cout << "[x] Fase 3 en desarrollo :D"  << endl;

  //thread cntrl(&control,ref(soc_serv),ref(soc_serv_fd), ref(system));

  thread cliente;
	bool correcto = true;
  while(correcto){
		
		try{
			//esperamos a un nuevo cliente
			Canal& c = localServer.getCliente(); // al ponerlo como 'Cliente&' el objeto no se destruye automáticamente al finalizar el loop

			cliente = thread(&newclient, ref(c), ref(system), ref(board));
			cliente.detach();
			
		}catch(...){
			
			if (end_mark == 1){
        system.err_safe_print("[x]Error en accept causado por señal; IGNORAR");
        break;
      } else {
        string mensError(strerror(errno));
        system.err_safe_print("[x] -- Error en el accept: " + mensError);
				// el socket se cierra automáticamente
        exit(1);
			}
		}
  }

  //Nos aseguramos que el proceso de control termina
  if(system.sum(0)!=0){
    system.endPH3();
  }


  cout << "[x] Fin de Ejecución."  << endl;
}
