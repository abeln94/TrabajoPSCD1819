#include "LindaDriver.hpp"
#include <sstream>

#ifdef ALLOW_LOCAL
// alows running in local having a global scoreboard object
#include "Scoreboard.hpp"
Scoreboard _lindaDriver_scoreboard;
bool _lindaDriver_local = false;
#endif



LindaDriver::LindaDriver(string ip, int puerto) {
#ifdef ALLOW_LOCAL
  if (ip == "local" && puerto == 0) {
    _lindaDriver_local = true;
    return;
  }
#endif

  string mensaje, subIp, subPort;

  // conectamos con servidor
  Canal servidor(ip, puerto, 10, 1000);

  // recibimos mensaje
  // Buffer content: 
	// "000.000.000.000 XXXX 000.000.000.000 XXXX 000.000.000.000 XXXX"
  servidor >> mensaje;

  // parseamos mensaje
  stringstream data(mensaje);
  for (int i = 0; i < 3; i++) {
    data >> subIp >> subPort;
    canal[i] = new Canal(subIp, atoi(subPort.c_str()), 10, 1000);
  }

  cout << "inicializado" << endl;
  // la conexión con el servidor se cierra automáticamente
}

LindaDriver::~LindaDriver() {
#ifdef ALLOW_LOCAL
  if (_lindaDriver_local) {
    return;
  }
#endif

  // Cerramos conexiones restantes
  for (int i = 0; i < 3; ++i) {
    delete (canal[i]);
  }
}

void LindaDriver::PN(Tupla tupla) {
#ifdef ALLOW_LOCAL
  if (_lindaDriver_local) {
    _lindaDriver_scoreboard.PN(tupla);
    return;
  }
#endif

  try {
    Canal& subserver = *getCanal(tupla.size());

    // Enviamos comando P
    subserver << "P" + tupla.to_string();
    

    // recibimos respuesta, aunque la omitimos
    string respuesta;
    subserver >> respuesta;
  }
  catch (...) {
    cerr << "Subservidor desconectado" << endl;
    exit(1);
  }
}

Tupla LindaDriver::RN(Tupla tupla) {
#ifdef ALLOW_LOCAL
  if (_lindaDriver_local) {
    return _lindaDriver_scoreboard.RN(tupla);
  }
#endif

  try {
    Canal& subserver = *getCanal(tupla.size());

    // Enviamos comando R
    subserver << "R" + tupla.to_string();

    // recibimos respuesta
    string respuesta;
    subserver >> respuesta;

    tupla.from_string(respuesta);
    return tupla;
  }
  catch (...) {
    cerr << "Subservidor desconectado" << endl;
    exit(1);
  }
}

Tupla LindaDriver::readN(Tupla tupla) {
#ifdef ALLOW_LOCAL
  if (_lindaDriver_local) {
    return _lindaDriver_scoreboard.readN(tupla);
  }
#endif

  try {
    Canal& subserver = *getCanal(tupla.size());

    // Enviamos comando r
    subserver << "r" + tupla.to_string();
    

    // recibimos respuesta
    string respuesta;
    subserver >> respuesta;

    tupla.from_string(respuesta);
    return tupla;
  }
  catch (...) {
    cerr << "Subservidor desconectado" << endl;
    exit(1);
  }
}

Canal* LindaDriver::getCanal(const Tupla& note) {
  switch (note.size()) {
    case 1:
    case 2:
    case 3:
      return canal[0];
    case 4:
    case 5:
      return canal[1];
    case 6:
      return canal[2];
  }
}
