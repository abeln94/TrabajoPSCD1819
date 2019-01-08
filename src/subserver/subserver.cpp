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
#include "Tuplas.hpp"
#include <iostream>
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

void control(Socket& soc, int& socket_fd){
  // en espera
}

void newclient(int socket_fd, Socket& soc, SafeSYS& sys, Scoreboard& pizarra){
  sys.sum(1);

  //.......... codigo???


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
  Socket soc_local(port_localhost);

  //Bind
	int soc_local_fd = soc_local.Bind();
	if (soc_local_fd == -1) {
		string mensError(strerror(errno));
    	cerr << "--Error en el bind: " + mensError + "\n";
		exit(1);
	}

	//Listen
	int errcode1 = soc_local.Listen(100);
	if (errcode1 == -1) {
		string mensError(strerror(errno));
    	cerr << "--Error en el listen: " + mensError + "\n";
		// Cerramos el socket
		soc_local.Close(soc_local_fd);
		exit(1);
	}


  cout << "[x] Fase 1 completada." << endl;
  //###################################################//
  //################## P H A S E : 2 ##################//
  //###################################################//
  //Conexión con serv
  cout << "[x] Inicio Fase 2 . . ."  << endl;
  //cout << "[x]Localizando sub-servidores. (en espera de conexión)"  << endl;
  //###################
  //sting miip = getmiip();

  Socket soc_serv(ip_serv,port_serv);

  //Connect
  int soc_serv_fd = soc_serv.Connect();
  if (soc_serv_fd == -1) {
    string mensError(strerror(errno));
      cerr << "[x]Error en el connect: " + mensError + "\n";
    exit(1);
  }

  // Si:IP=xxx.xxx.xxx.xxx-PORT=PPPPP
  soc_serv.Send(soc_serv_fd, "S" + quiensoy + ":IP=" + ipmia + "-PORT=" + argv[2]);
  // Si:PORT=PPPPP
  //soc_serv.Send(soc_serv_fd, "S" + quiensoy + ":PORT=" + argv[2]);


  cout << "[x] Fase 2 completada." << endl;
  //###################################################//
  //################## P H A S E : 3 ##################//
  //###################################################//
  //Servicio iniciado, comandos disponibles
  cout << "[x] Inicio Fase 3 . . ."  << endl;

  SafeSYS system;
  Scoreboard board;

  cout << "[x] Fase 3 en desarrollo :D"  << endl;

  thread cntrl(&control,ref(soc_serv),ref(soc_serv_fd), ref(system));

  thread cliente;
  int client_fd = 0;
  while(end_mark==0){
    client_fd = soc_local.Accept();
    if(client_fd == -1 || (client_fd==0 && end_mark==1)) {
      if (end_mark == 1){
        system.err_safe_print("[x]Error en accept causado por señal; IGNORAR");
        continue;
      } else {
        string mensError(strerror(errno));
        system.err_safe_print("[x] -- Error en el accept: " + mensError);
        // Cerramos el socket
        soc_local.Close(soc_serv_fd);
        soc_serv.Close(soc_serv_fd);
        exit(1);
      }
    }

    cliente = thread(&newclient, client_fd, std::ref(soc_local), std::ref(system), std::ref(board));
    cliente.detach();
    client_fd=0;
  }

  //Nos aseguramos que el proceso de control termina
  system.endPH3();

  cout << "[x] Fin de Ejecución."  << endl;
}
