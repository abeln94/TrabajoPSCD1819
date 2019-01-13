//*****************************************************************
// File:  subserver.cpp
// Authors:   GONZÁLEZ VILLA, DANIEL
//            NAYA FORCANO, ABEL
//            GONZÁLEZ GORRADO, JESÚS ÁNGEL
//            GARCÍA DÍAZ, ÁLVARO
// Date:   Diciembre 2018-Enero 2019
//*****************************************************************

#include "Socket.hpp"
#include "Scoreboard.hpp"
#include "Tupla.hpp"
#include <iostream>
#include <thread>
#include <cstring>
#include <cstdlib>
#include <signal.h>

#include <mutex>
#include <condition_variable>

using namespace std;

sig_atomic_t end_mark = 0;

// Controlador de exclusion mutua
class SafeSYS {
 public:
  // Constructor
  SafeSYS() { counter = 0; }

  void endPH3() {
    unique_lock<mutex> lck(mtx);
    while (counter != 0) {
      end.wait(lck);
    }
  }

  int sum(int n) {
    unique_lock<mutex> lck(mtx);
    counter = counter + n;
    if (counter == 0) {
      end.notify_one();
    }
    return counter;
  }

  void safe_print(string c) {
    unique_lock<mutex> lck(mtx);
    cout << c << endl;
  }

  void err_safe_print(string c) {
    unique_lock<mutex> lck(mtx);
    cerr << c << endl;
  }

 private:
  int counter;
  mutex mtx;
  condition_variable end;
};

// Funciones encargadas de la comunicación exterior
void control(int puerto, Socket& soc, int& socket_fd, SafeSYS& sys,
             Scoreboard pizarras[]) {
  string buffer = "";
  while (buffer != "END") {
    soc.Recv(socket_fd, buffer, 10);
    if (buffer == "END") {
      break;
    } else if (buffer == "CLEAR") {
      for (int i = 0; i < 6; ++i) {
        pizarras[i].clear();
      }
    } else if (buffer == "SIZE") {
      int size = 0;
      for (int i = 0; i < 6; ++i) {
        size += pizarras[i].size();
      }
      soc.Send(socket_fd, to_string(size));
    }
  }

  raise(SIGTSTP);

  Socket temp("localhost", puerto);
  int temp_fd = temp.Connect();
  if (temp_fd == -1) {
    string mensError(strerror(errno));
    cerr << "[x]Error en el connect: " + mensError + "\n";
    return;
  }
  temp.Close(temp_fd);
}

void newclient(int socket_fd, Socket& soc, SafeSYS& sys,
               Scoreboard pizarras[]) {
  sys.sum(1);
  string mensaje;
  string buffer;
  bool err = false, crear = false;
  while (!err) {
    buffer = "ERR";
    int rec_bytes = soc.Recv(socket_fd, mensaje, 10000);
    if (rec_bytes == -1) {
      string mensError = strerror(errno);
      cerr << "[x] Error al recibir datos de LindaDriver: " + mensError + "\n";
      err = true;
      continue;
    }

    // Tratado de msg
    int tam = mensaje.length();
    if (mensaje[tam - 1] == '1') {  // PN
      mensaje.erase(tam - 1);
      tam = mensaje.length();
      string lons = to_string(mensaje[tam - 1] - '0');
      int lon = stoi(lons);
      mensaje.erase(tam - 1);
      Tupla mens(lon);
      crear = mens.from_string(mensaje);
      if (crear) {
        pizarras[lon - 1].PN(mens);
        buffer = "OK";
      } else {
        cerr << "[x] Error al crear tupla, formato de mensaje incorrecto"
             << endl;
      }
    } else if (mensaje[tam - 1] == '2') {  // RN
      mensaje.erase(tam - 1);
      tam = mensaje.length();
      string lons = to_string(mensaje[tam - 1] - '0');
      int lon = stoi(lons);
      mensaje.erase(tam - 1);
      Tupla mens(lon);
      Tupla mens_fin(lon);
      crear = mens.from_string(mensaje);
      if (crear) {
        mens_fin = pizarras[lon - 1].RN(mens);
        buffer = mens_fin.to_string();
      } else {
        cerr << "[x] Error al crear tupla, formato de mensaje incorrecto"
             << endl;
      }
    } else if (mensaje[tam - 1] == '3') {  // readN
      mensaje.erase(tam - 1);
      tam = mensaje.length();
      string lons = to_string(mensaje[tam - 1] - '0');
      int lon = stoi(lons);
      mensaje.erase(tam - 1);
      Tupla mens(lon);
      Tupla mens_fin(lon);
      crear = mens.from_string(mensaje);
      if (crear) {
        mens_fin = pizarras[lon - 1].readN(mens);
        buffer = mens_fin.to_string();
      } else {
        cout << "[x] Error al crear tupla, formato de mensaje incorrecto"
             << endl;
      }
    }

    if (crear) {
      int send_bytes = soc.Send(socket_fd, buffer);
      if (send_bytes == -1) {
        cerr << "[x] Cliente cerrado\n";
        err = true;
        continue;
      }
    } else {
      int send_bytes = soc.Send(socket_fd, "ERR");
      if (send_bytes == -1) {
        string mensError = strerror(errno);
        cerr << "[x] Error al enviar datos a LindaDriver: " + mensError + "\n";
        err = true;
        continue;
      }
    }
  }
  sys.sum(-1);
}

// Función encargada del tratamiento de señales
void sig_handler(int signo) {
  if (SIGPIPE == signo) {
    return;
  }
  end_mark = 1;
  cerr << endl;
  if (SIGINT == signo) {
    cerr << "[x]Señal de interrupción capturada, cerrando servidor..." << endl;
  } else if (SIGTSTP == signo) {
    cerr << "[x]Señal de finalización capturada, cerrando servidor..." << endl;
  } else if (SIGQUIT == signo) {
    cerr << "[x]Señal de cierre capturada, cerrando servidor..." << endl;
  } else {
    cerr << "[x]Señal desconocida capturada, cerrando servidor..." << endl;
  }
  cerr << endl;
  cerr << "[x]Advertencia: Es posible que aun se ejecuten algunas sentencias..."
       << endl;
  cerr << endl;
}

// Main func.
int main(int argc, char* argv[]) {
  // Declaración de variables
  const string localhost = "localhost";
  string quiensoy;
  string ip_serv, ipmia;
  int port_localhost;
  int port_serv;

  // Comprabación de número de parámetros
  if (argc == 6) {
    quiensoy = argv[1];
    ipmia = argv[2];
    port_localhost = atoi(argv[3]);
    ip_serv = argv[4];
    port_serv = atoi(argv[5]);
  } else {
    cout << "[x]Advertencia" << endl;
    cout << "[x]Error: Número de argumentos invalido" << endl;
    cout << "./<ejecutable> <quiensoy> <ip_mia> <puerto local> <ip_serv> "
            "<puerto_serv>" << endl;
    exit(0);
  }

  if (quiensoy != "1" && quiensoy != "2" && quiensoy != "3") {
    cout << "[x]Advertencia" << endl;
    cout << "[x]Error: Valor para <quiensoy> invalido" << endl;
    cout << "./<ejecutable> <quiensoy> <puerto local> <ip_serv> <puerto_serv>"
         << endl;
    exit(0);
  }

  cout << ">>" << endl;
  cout << ">> INICIANDO SERVICIO" << endl;
  cout << ">> SUBSERVIDOR " << quiensoy << endl;
  cout << ">> IP = " << ipmia << endl;
  cout << ">> PUERTO = " << port_localhost << endl;
  cout << ">>" << endl;

  //###################################################//
  //################## P H A S E : 1 ##################//
  //###################################################//
  // Creación de sockets y protección ante señales
  cout << "[x] Inicio Fase 1: Creación de sockets y protección ante señales"
       << endl;

  // Terminacion controlada
  // Protección frente "^C", "^\", "^Z"
  struct sigaction sig_han;
  sig_han.sa_handler = sig_handler;
  sigemptyset(&sig_han.sa_mask);
  sig_han.sa_flags = 0;
  sigaction(SIGINT, &sig_han, NULL);
  sigaction(SIGTSTP, &sig_han, NULL);
  sigaction(SIGQUIT, &sig_han, NULL);
  sigaction(SIGPIPE, &sig_han, NULL);

  // Conexión con clientes:
  Socket soc_local(port_localhost);

  // Bind
  int soc_local_fd = soc_local.Bind();
  if (soc_local_fd == -1) {
    string mensError(strerror(errno));
    cerr << "--Error en el bind: " + mensError + "\n";
    exit(1);
  }

  // Listen
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
  // Conexión con serv
  cout << "[x] Inicio Fase 2: Conexión con servidor principal" << endl;

  Socket soc_serv(ip_serv, port_serv);

  // Connect
  int soc_serv_fd = soc_serv.Connect();
  if (soc_serv_fd == -1) {
    string mensError(strerror(errno));
    cerr << "[x]Error en el connect: " + mensError + "\n";
    exit(1);
  }

  // Si:IP=xxx.xxx.xxx.xxx-PORT=PPPPP
  soc_serv.Send(soc_serv_fd, "S" + quiensoy + ":IP=" + ipmia + "-PORT=" +
                                 to_string(port_localhost));

  string test;
  soc_serv.Recv(soc_serv_fd, test, 15);
  if (test != "OK") {
    cout << "[x] Error en respuesta de CENTRAL." << endl;
    soc_serv.Close(soc_serv_fd);
    soc_local.Close(soc_local_fd);
    exit(0);
  }

  cout << "[x] Fase 2 completada." << endl;

  //###################################################//
  //################## P H A S E : 3 ##################//
  //###################################################//
  // Servicio iniciado, comandos disponibles
  cout << "[x] Inicio Fase 3: Clientes" << endl;

  SafeSYS system;
  Scoreboard boards[6];

  thread cntrl(&control, port_localhost, ref(soc_serv), ref(soc_serv_fd),
               ref(system), ref(boards));

  thread cliente;
  int client_fd = 0;
  while (end_mark == 0) {
    client_fd = soc_local.Accept();
    if (client_fd == -1 || (client_fd == 0 && end_mark == 1)) {
      if (end_mark == 1) {  // end mark (señal local)
        system.err_safe_print("[x]Error en accept causado por señal; IGNORAR");
        break;
      } else {
        string mensError(strerror(errno));
        system.err_safe_print("[x] -- Error en el accept: " + mensError);
        // Cerramos el socket
        soc_local.Close(soc_serv_fd);
        soc_serv.Close(soc_serv_fd);
        exit(1);
      }
    }

    if (end_mark != 1) {  // Bugfix
      cliente = thread(&newclient, client_fd, std::ref(soc_local),
                       std::ref(system), std::ref(boards));
      cliente.detach();
      client_fd = 0;
    } else {  // end_mark a 1 y cliente del proceso de control
      soc_local.Close(client_fd);
    }
  }

  // Nos aseguramos que el proceso de control termina
  if (system.sum(0) != 0) {
    system.endPH3();
  }

  cntrl.join();

  soc_serv.Close(soc_serv_fd);
  soc_local.Close(soc_local_fd);

  cout << "[x] Fin de Ejecución." << endl;
}
