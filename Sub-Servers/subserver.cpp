//******************************************************************
// File:  servidor.cpp
// Authors:   Daniel González
//            NOMBRES
//            NOMBRES
//            NOMBRES
// Date:   Diciembre 2018
//*****************************************************************


#include "Socket.hpp"
#include "ServerControl.hpp"
#include <iostream>
#include <thread>
#include <cstring>
#include <cstdlib>
#include <signal.h>

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

int main(int argc, char * argv[]) {
  //Declaración de variables
  const string localhost = "localhost";
  int quiensoy;
  string ip_serv
  int port_localhost;
  int port_serv;


  //COMPROBACIONES
  if(argc == 5){
    quiensoy = atoi(argv[1]);
    port_localhost = atoi(argv[2]);
    ip_serv = argv[3];
    port_serv = atoi(argv[4]);
  } else {
    cout << "[x]Advertencia"<< endl;
    cout << "[x]Error: Número de argumentos invalido"<< endl;
    cout << "./<ejecutable> <quiensoy> <puerto local> <ip_serv> <puerto_serv>"<<endl;
    exit(0);
  }

  if(quiensoy != "1" && quiensoy != "2" && quiensoy != "2"){
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
  ###################
  sting miip = getmiip();

  Socket soc_serv(ip_serv,port_serv);

  //Connect
  int soc_serv_fd = soc_serv.Connect();
  if (soc_serv_fd == -1) {
    string mensError(strerror(errno));
      cerr << "[x]Error en el connect: " + mensError + "\n";
    exit(1);
  }

  // Si:IP=xxx.xxx.xxx.xxx-PORT=PPPPP
  soc_serv.Send(soc_serv_fd, "S" + quiensoy + ":IP=" + miip + "-PORT=" + argv[2]);


  cout << "[x] Fase 2 completada." << endl;
  //###################################################//
  //################## P H A S E : 3 ##################//
  //###################################################//
  //Servicio iniciado, comandos disponibles
  cout << "[x] Inicio Fase 3 . . ."  << endl;

  cout << "[x] Fase 3 en desarrollo :D"  << endl;

  /*
  //thread cliente; redeclaration
  int client_fd;
  while(end_mark == 0){
    client_fd = soc_pub.Accept();
    if(client_fd == -1 || (client_fd==0 && end_mark==1)) {
      if (end_mark == 1){
        cerr << "[x]Error en accept causado por señal; IGNORAR"  << endl;
        continue;
      } else {
        string mensError(strerror(errno));
        cerr << "--Error en el accept: " + mensError + "\n";
        // Cerramos el socket
        soc_pub.Close(soc_pub_fd);
        soc_priv.Close(soc_priv_fd);
        exit(1);
      }
    }

    cliente = thread(&contact, std::ref(soc_pub), client_fd, std::ref(ctrl));
    cliente.detach();
    client_fd=0;
  }

  ctrl.endPH3();
  */

  cout << "[x] Fin de Ejecución."  << endl;

  soc_pub.Close(soc_pub_fd);
  soc_priv.Close(soc_priv_fd);

}
