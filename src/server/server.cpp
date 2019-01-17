#include <iostream>
#include <thread>
#include <string>
#include <Socket.hpp>

using namespace std;

//-----------------------------------------------------
int main(int argc, char* argv[]) {

  if (argc != 8) {
    cout << "$run port (subserverIp subserverPort)x3" << endl;
    return 0;
  }

  int port = atoi(argv[1]);

  string data = argv[0 + 2];
  for (int i = 1; i < 6; ++i) {
    data = data + " " + argv[i + 2];
  }

  // inicializamos el servidor
  Servidor server(port, 10);

  string res;

  while (true) {
    // esperamos a un nuevo cliente
    Canal c = server.getCliente();

    // le enviamos la información
    cout << "Hola! toma la info, ahora vete >:( " << endl;
    c << data;
  }

  return 0;
}
