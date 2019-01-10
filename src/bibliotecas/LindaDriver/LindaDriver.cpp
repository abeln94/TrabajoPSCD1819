//******************************************************************
// File:  LindaDriver.cpp
// Authors:   NOMBRES
//            NOMBRES
//            NOMBRES
//            NOMBRES
// Date:   Diciembre 2018-Enero 2019
//*****************************************************************

#include "LindaDriver.hpp"
#include <sstream>

#ifdef ALLOW_LOCAL
// alows running in local having a global scoreboard object
#include "Scoreboard.hpp"
Scoreboard _lindaDriver_scoreboard;
bool _lindaDriver_local = false;
#endif

//...

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
  // Buffer content: 000.000.000.000 XXXX 000.000.000.000 XXXX 000.000.000.000
  // XXXX
  servidor >> mensaje;

  // parseamos mensaje
  stringstream data(mensaje);
  for (int i = 0; i < 3; i++) {
    data >> subIp;
    data >> subPort;
    canal[i] = new Canal(subIp, atoi(subPort.c_str()), 10, 1000);
  }

  // no es necesario ningun mensaje de ok

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

  Canal& subserver = *getCanal(tupla.size());

  // Añadimos "P" para que el subservidor sepa que accion realizar, y el tamaño
  // de la tupla para que pueda tratar el mensaje.
  string mens = "P" + to_string(tupla.size()) + tupla.to_string();
  cout << mens << endl;
  subserver << mens;

  // recibimos respuesta, aunque la omitimos
  string respuesta;
  subserver >> respuesta;
}

Tupla LindaDriver::RN(Tupla tupla) {
#ifdef ALLOW_LOCAL
  if (_lindaDriver_local) {
    return _lindaDriver_scoreboard.RN(tupla);
  }
#endif

  Canal& subserver = *getCanal(tupla.size());

  // Añadimos "R" para que el subservidor sepa que accion realizar, y el tamaño
  // de la tupla para que pueda tratar el mensaje.
  string mens = "R" + to_string(tupla.size()) + tupla.to_string();
  cout << mens << endl;
  subserver << mens;

  // recibimos respuesta
  string respuesta;
  subserver >> respuesta;

  Tupla resultado(tupla.size());
  resultado.from_string(respuesta);
  return resultado;
}

Tupla LindaDriver::readN(Tupla tupla) {
#ifdef ALLOW_LOCAL
  if (_lindaDriver_local) {
    return _lindaDriver_scoreboard.readN(tupla);
  }
#endif

  Canal& subserver = *getCanal(tupla.size());

  // Añadimos "r" para que el subservidor sepa que accion realizar, y el tamaño
  // de la tupla para que pueda tratar el mensaje.
  string mens = "r" + to_string(tupla.size()) + tupla.to_string();
  cout << mens << endl;
  subserver << mens;

  // recibimos respuesta
  string respuesta;
  subserver >> respuesta;

  Tupla resultado(tupla.size());
  resultado.from_string(respuesta);
  return resultado;
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
