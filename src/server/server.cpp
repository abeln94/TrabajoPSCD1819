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

using namespace std;

//Se puede cambiar por otra cosa??
sig_atomic_t end_mark = 0;

void contact(Canal cliente, ControlSys& sys){
  sys.sumPH3(1);
  string buffer;
	
	try{

		sys.safe_print("[x] Cliente conectado.");
		
		cliente << sys.ips_to_string();
		
		cliente >> buffer;
	
	}catch(...){
		sys.err_safe_print("[x] Error al enviar/recibir datos: Finalización del subservidor inesperada.");
	}
	

  //End process
  sys.sumPH3(-1);
}

void conection(Canal subserver, ControlSys &sys){
  const int maxlength = 150;
	string buffer = "", res = "", ip;
  bool err = false;
  int rcv_bytes = 0, port, id;
  ssize_t snd_bytes = 0;

	try{
		//recibir el mensaje
		subserver >> buffer;
		
		//Must recive a buffer like "Si:IP=xxx.xxx.xxx.xxx-PORT=PPPPP"
		//else, conection corrupted
		if (buffer.length() >= 22){

			//SUBSERVER ID
			string sub_id = buffer.substr(0,3);
			sub_id.erase(0,1);
			id = atoi(sub_id.c_str());
			if(id == 0){
				err = true;
			}

			//SUBSERVER PORT
			int portpos = buffer.find("PORT=",0);
			if(portpos == -1){
				err = true;
			} else {
				port = atoi(buffer.substr(portpos+5,buffer.length()).c_str());
			}

			//SUBSERVER IP
			int ippos = buffer.find("IP=",0);
			if(ippos == -1){
				err = true;
			} else if (portpos != -1){
				ip = buffer.substr(ippos+3,portpos-1-(ippos+3));
			}

		} else {
			err = true;
		}

		if(err){
			res = "ERROR";
		}else{
			res = "OK";
		}
	
		subserver << res;
		
		
	}catch(...){
		sys.err_safe_print(buffer);
    sys.err_safe_print("[x]  datos: Finalización del subservidor inesperada.");
    err = true;
	}
	
	if(err){
		sys.safe_print("[x] Ending process with errors.");
	} else {
		sys.fill(id,0,port,ip);//el segundo parametro no se usa para nada, asi que le pongo un cero y ale
		sys.safe_print("[x] Subservidor " + to_string(id) + " conectado (" + ip +
										":" + to_string(port) + ")." );
	}
}

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
  const string SERVER_ADDRESS = "localhost";
  int SERVER_PORT_PUBLIC;
  int SERVER_PORT_PRIVATE;


  //COMPROBACIONES
  if(argc == 3){
    SERVER_PORT_PUBLIC = atoi(argv[1]);
    SERVER_PORT_PRIVATE = atoi(argv[2]);
  } else {
    cout << "[x]Advertencia"<< endl;
    cout << "[x]Error: Número de argumentos invalido"<< endl;
    cout << "./<ejecutable> <puerto publico> <puerto privado>"<<endl;
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

  ControlSys ctrl(SERVER_PORT_PRIVATE, SERVER_PORT_PUBLIC);

  //Conexión con servidores:
	Servidor server_priv(SERVER_PORT_PRIVATE, 5);


  //Conexión con clientes:
	Servidor server_pub(SERVER_PORT_PUBLIC, 100);

  ctrl.safe_print("[x] Fase 1 completada.");
  //###################################################//
  //################## P H A S E : 2 ##################//
  //###################################################//
  //Conexión con subs
  ctrl.safe_print("[x] Inicio Fase 2 . . .");
  ctrl.safe_print("[x] >>Localizando sub-servidores. (en espera de conexión)");

  int strange_fd;
  while(!ctrl.ready() && end_mark == 0){
		try{
			Canal& subserver = server_priv.getCliente();
			
			conection(subserver, ctrl);
			
		}catch(...){
			if (end_mark == 1){
				//Bugfix
				ctrl.err_safe_print("[x]Error en accept causado por señal; IGNORAR");
				break;
			} else {
				ctrl.err_safe_print("[x]Error en accept: " + string(strerror(errno)));
				// El socket se cierra solo
				exit(1);
			}
		}
    
  }

  if (end_mark == 1){
    ctrl.safe_print("[x] Fin de Ejecución.");
		//los sockets se cierran solos
    exit(0);
  }

  ctrl.safe_print("[x] Todos sub-servidores conectados.");
  ctrl.safe_print("[x] Fase 2 completada.");
  //###################################################//
  //################## P H A S E : 3 ##################//
  //###################################################//
  //Servicio iniciado, comandos disponibles
  ctrl.safe_print("[x] Inicio Fase 3 . . .");

  thread cliente;
  while(end_mark == 0){
		
		try{
			Canal& canalCliente = server_pub.getCliente();
			
			cliente = thread(&contact, ref(canalCliente), ref(ctrl));
			cliente.detach();
			
		}catch(...){
			if (end_mark == 1){
				ctrl.err_safe_print("[x]Error en accept causado por señal; IGNORAR");
				break;
			} else {
				string mensError(strerror(errno));
				cerr << "--Error en el accept: " + mensError + "\n";
				// Sockets se cierran solos
				exit(1);
			}
		}
  }

  if(ctrl.sumPH3(0)!=0){
    ctrl.endPH3();
  }

  ctrl.safe_print("[x] Fin de Ejecución.");

	//Sockets se cierran solos
}
//-------------------------------------------------------------
