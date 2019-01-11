//*****************************************************************
// File:  server.cpp
// Authors:   GONZÁLEZ VILLA, DANIEL
//            NAYA FORCANO, ABEL
//            GONZÁLEZ GORRADO, JESÚS ÁNGEL
//            GARCÍA DÍAZ, ÁLVARO
// Date:   Diciembre 2018-Enero 2019
//*****************************************************************

#include "Socket.hpp"
#include "ServerControl.hpp"
#include <iostream>
#include <thread>
#include <cstring>
#include <cstdlib>
#include <signal.h>

using namespace std;

sig_atomic_t end_mark = 0;

void contact(int client_fd, Socket& soc, ControlSys& sys){
  sys.sumPH3(1);
  string buffer;

  sys.safe_print("[x] Cliente conectado.");
  if (soc.Send(client_fd, sys.ips_to_string()) == -1) {
    sys.err_safe_print("[x] Error al enviar datos: Finalización del subservidor inesperada.");
  } else if (soc.Recv(client_fd, buffer, 15) == -1) {
    sys.err_safe_print("[x] Error al recibir datos: Finalización del subservidor inesperada.");
  }

  //End process
  sys.sumPH3(-1);
  soc.Close(client_fd);
}

void conection(int subserv_fd, Socket& soc, ControlSys &sys){
  const int maxlength = 150;
  string buffer = "", res = "", ip;
  bool err = false;
  int rcv_bytes = 0, port, id;
  ssize_t snd_bytes = 0;

  rcv_bytes = soc.Recv(subserv_fd,buffer,maxlength);
  if (rcv_bytes == -1 || rcv_bytes == 0) {
    sys.err_safe_print(buffer);
    sys.err_safe_print("[x] datos: Finalización del subservidor inesperada.");
    err = true;
    if(soc.Close(subserv_fd) == -1){
      sys.err_safe_print("[x] Fallo al cerrar socket corrupto: " + string(strerror(errno)));
    }
    if(err){
      sys.safe_print("[x] Ending process with errors.");
    }
    return;
  }

  //Must receive a buffer like "Si:IP=xxx.xxx.xxx.xxx-PORT=PPPPP"
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

  snd_bytes = soc.Send(subserv_fd,res);
  if (snd_bytes == -1) {
    sys.err_safe_print("[x] Error al enviar datos: Finalización del subservidor inesperada.");
    err = true;
    if(soc.Close(subserv_fd) == -1){
      sys.err_safe_print("[x] Fallo al cerrar socket corrupto: " + string(strerror(errno)));
    }
    if(err){
      sys.safe_print("[x] Ending process with errors.");
    }
    return;
  }

  if(err){
    sys.safe_print("[x] Ending process with errors.");
  } else {
    sys.fill(id,subserv_fd,port,ip);
    sys.safe_print("[x] Subservidor " + to_string(id) + " conectado (" + ip +
                    ":" + to_string(port) + ")." );
  }
  return;
}

void commands(int puerto, Socket& soc_priv, ControlSys &sys){
	//sys.sumPH3(1);
	int strange_fd;
  while(end_mark == 0){
    strange_fd = soc_priv.Accept();
    if(strange_fd == -1 || (strange_fd==0 && end_mark==1)) {
      if (end_mark == 1){
        //Bugfix
        sys.err_safe_print("[x] Señal de finalización, terminar");
        break;
      } else {
        sys.err_safe_print("[x]Error en accept: " + string(strerror(errno)));
        exit(1);
				break;
      }
    }
    
		string buffer;
		const int maxlength = 150;
		int rcv_bytes;
		rcv_bytes = soc_priv.Recv(strange_fd,buffer,maxlength);
		if (rcv_bytes == -1 || rcv_bytes == 0) {
			// ha muerto? pues ok, lo omitimos
			continue;
		}
		
		//tratar mensajes
		if(buffer == "END"){
			//finalizar
			raise(SIGTSTP);
			
			//lanzamos un mensaje al otro thread para que se entere
			Socket temp("localhost",puerto);
			int temp_fd = temp.Connect();
			if (temp_fd == -1) {
				string mensError(strerror(errno));
				cerr << "[x]Error en el connect: " + mensError + "\n";
				return;
			}
			temp.Close(temp_fd);
			
			break;
		}
  }
	
	//sys.sumPH3(-1);
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
  cerr << "[x]Advertencia: Es posible que aun se ejecuten algunas sentencias..." <<endl;
  cerr << endl;
}

int main(int argc, char * argv[]) {
  //Declaración de variables
  const string SERVER_ADDRESS = "localhost";
  int SERVER_PORT_PUBLIC;
  int SERVER_PORT_PRIVATE;


  //Comprobación de parámetros
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
  cout << ">> SERVIDOR PRINCIPAL" << endl;
  cout << ">> PUERTO PÚBLICO = " << SERVER_PORT_PUBLIC << endl;
  cout << ">> PUERTO PÚBLICO = " << SERVER_PORT_PRIVATE << endl;
  cout << ">>" << endl;

  //###################################################//
  //################## P H A S E : 1 ##################//
  //###################################################//
  //Creación de sockets y protección ante señales
  cout << "[x] Inicio Fase 1: Creación de sockets y protección ante señales" << endl;

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
  Socket soc_priv(SERVER_PORT_PRIVATE);

  //Bind
  int soc_priv_fd = soc_priv.Bind();
  if (soc_priv_fd == -1) {
	string mensError(strerror(errno));
	cerr << "--Error en el bind: " + mensError + "\n";
	exit(1);
  }

  //Listen
  int errcode1 = soc_priv.Listen(5);
  if (errcode1 == -1) {
	string mensError(strerror(errno));
	cerr << "--Error en el listen: " + mensError + "\n";
	// Cerramos el socket
	soc_priv.Close(soc_priv_fd);
	exit(1);
  }

  //Conexión con clientes:
  Socket soc_pub(SERVER_PORT_PUBLIC);

  //Bind
  int soc_pub_fd = soc_pub.Bind();
  if (soc_pub_fd == -1) {
	string mensError(strerror(errno));
   	cerr << "--Error en el bind: " + mensError + "\n";
	exit(1);
  }

  // Listen
  int errcode2 = soc_pub.Listen(100);
  if (errcode2 == -1) {
	string mensError(strerror(errno));
   	cerr << "--Error en el listen: " + mensError + "\n";
	// Cerramos el socket
	soc_pub.Close(soc_pub_fd);
	exit(1);
  }

  ctrl.safe_print("[x] Fase 1 completada.");
  
  //###################################################//
  //################## P H A S E : 2 ##################//
  //###################################################//
  //Conexión con subs
  
  ctrl.safe_print("[x] Inicio Fase 2: Conexión con sub-servidores");
  ctrl.safe_print("[x] >>Localizando sub-servidores. (en espera de conexión)");

  int strange_fd;
  while(!ctrl.ready() && end_mark == 0){
    strange_fd = soc_priv.Accept();
    if(strange_fd == -1 || (strange_fd==0 && end_mark==1)) {
      if (end_mark == 1){
        ctrl.err_safe_print("[x]Error en accept causado por señal; IGNORAR");
        break;
      } else {
        ctrl.err_safe_print("[x]Error en accept: " + string(strerror(errno)));
        // Cerramos el socket
        soc_priv.Close(soc_priv_fd);
        exit(1);
      }
    }
    if (!ctrl.ready()){
      conection(strange_fd, soc_priv, ctrl);
    }
  }

  if (end_mark == 1){
    ctrl.end(soc_priv);
    ctrl.safe_print("[x] Fin de Ejecución.");
    soc_pub.Close(soc_pub_fd);
    soc_priv.Close(soc_priv_fd);
    exit(0);
  }
	
	thread(&commands, SERVER_PORT_PUBLIC, ref(soc_priv), ref(ctrl)).detach();

  ctrl.safe_print("[x] Todos los sub-servidores conectados.");
  ctrl.safe_print("[x] Fase 2 completada.");
  
  //###################################################//
  //################## P H A S E : 3 ##################//
  //###################################################//
  //Servicio iniciado, comandos disponibles
  ctrl.safe_print("[x] Inicio Fase 3: Clientes");

  int client_fd;
  thread cliente;
  while(end_mark == 0){
    client_fd = soc_pub.Accept();
    if(client_fd == -1 || (client_fd==0 && end_mark==1)) {
      if (end_mark == 1){
        ctrl.err_safe_print("[x]Error en accept causado por señal; IGNORAR");
        break;
      } else {
        string mensError(strerror(errno));
        cerr << "[x] Error en el accept: " + mensError + "\n";
        // Cerramos el socket
        soc_pub.Close(soc_pub_fd);
        soc_priv.Close(soc_priv_fd);
        exit(1);
      }
    }

    cliente = thread(&contact, client_fd, std::ref(soc_pub), std::ref(ctrl));
    cliente.detach();
    client_fd=0;
  }

  if(ctrl.sumPH3(0)!=0){
    ctrl.endPH3();
  }
	
  ctrl.end(soc_priv);

  ctrl.safe_print("[x] Fin de Ejecución.");

  soc_pub.Close(soc_pub_fd);
  soc_priv.Close(soc_priv_fd);
}
//-------------------------------------------------------------
