//******************************************************************
// File:  LindaDriver.hpp
// Authors:   NOMBRES
//            NOMBRES
//            NOMBRES
//            NOMBRES
// Date:   Diciembre 2018-Enero 2019
//*****************************************************************

#ifndef LINDA_DRIVER
#define LINDA_DRIVER

#include <iostream>
#include "Socket.hpp"
#include "Tupla.hpp"
#include <string>

using namespace std;

//...

class LindaDriver {
 public:
  LindaDriver(string ip, int puerto);

  ~LindaDriver();
  // Operaciones de Linda
  void PN(Tupla mensaje);
  Tupla RN(Tupla mensaje);
  Tupla readN(Tupla mensaje);

 private:
  Canal* getCanal(const Tupla& note);

  Canal* canal[3];
};

#endif
