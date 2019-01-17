
#include <iostream>
#include <thread>
#include <string>
#include <Socket.hpp>
#include "Scoreboard.hpp"

using namespace std;

//-----------------------------------------------------
// un thread del servidor que se encarga de tratar con un cliente concreto
void t_serverthread(Canal cliente, Scoreboard& scb) {
  try {
    string msg;
    Tupla tuple(1);
    while (true) {
      // repetimos indefinidamente hasta que el cliente se vaya

      // esperamos el mensaje
      cliente >> msg;
      cout << "Recibido mensaje " + msg + "\n";

      tuple.from_string(msg.substr(1));

      if (msg[0] == 'P') {
        scb.PN(tuple);
      } else if (msg[0] == 'R') {
        tuple = scb.RN(tuple);
      } else if (msg[0] == 'r') {
        tuple = scb.readN(tuple);
      }

      msg = tuple.to_string();
      cliente << msg;
      cout << "Enviada respuesta " + msg + "\n";
    }
  }
  catch (...) {
    // algún error (el cliente se ha desconectado)
    cout << "el cliente se ha ido :(" << endl;
  }
}
//-----------------------------------------------------
int main(int argc, char* argv[]) {

  if (argc != 2) {
    cout << "$run port" << endl;
    return 0;
  }

  int port = atoi(argv[1]);

  // inicializamos el servidor
  Servidor server(port, 10);

  Scoreboard scb;

  string res;

  while (true) {
    // repetimos hasta que recibamos la orden concreta

    // esperamos a un nuevo cliente
    Canal& c = server.getCliente();  // al ponerlo como 'Cliente&' el objeto no
                                     // se destruye automáticamente al finalizar
                                     // el loop

    // iniciamos un thread para tratarlo, y nos olvidamos de él
    thread(t_serverthread, ref(c), ref(scb)).detach();
  }

  return 0;
}