//*****************************************************************
// File:  LindaDriver.hpp
// Authors:   GONZÁLEZ VILLA, DANIEL
//            NAYA FORCANO, ABEL
//            GONZÁLEZ GORRADO, JESÚS ÁNGEL
//            GARCÍA DÍAZ, ÁLVARO
// Date:   Diciembre 2018-Enero 2019
//*****************************************************************

#ifndef LINDA_DRIVER
#define LINDA_DRIVER

#include <iostream>
#include "Socket.hpp"
#include "Tupla.hpp"
#include <string>

using namespace std;

class LindaDriver{
  public:
    LindaDriver(string ip, int puerto);

    ~LindaDriver();
	
    //------Operaciones de Linda
	/**
	Añade una copia de la tupla al tablón
	*/
    void PN (Tupla mensaje);
	
	/**
	Elimina una nota del tablón que coincide con {nota}.
	Devuelve la nota eliminada.
	Si no existe ninguna nota que coincida, la función se bloquea hasta que haya
	una.

	Para que dos notas coincidan se debe cumplir:
	  - Deben tener el mismo número de elementos.
	  - Para cada elemento de {note} que no comience con "?" o sea vacío, la
		nota obtenida contendrá exactamente el mismo elemento.
	  - Para todos los elementos de {note} que comiencen con "?" y sean
		iguales, la nota obtenida contendrá el mismo elemento en todos ellos.
	  - Los elementos de {note} que sean vacíos o la string "?" coinciden
	con cualquier elemento.
	*/
    Tupla RN (Tupla mensaje);
	
	/**
	Idéntico a RN pero no elimina la nota del tablón, devuelve una copia.
	*/
    Tupla readN (Tupla mensaje);

  private:
    string ip_serv, ip_s1, ip_s2, ip_s3;
    int port_serv, port_s1, port_s2, port_s3;
    int serv_fd, s1_fd, s2_fd, s3_fd;
    bool s1_ready, s2_ready, s3_ready;
    Socket *soc_serv;
    Socket *soc_s1, *soc_s2, *soc_s3;

    void rellenar(int id, string ip, int port);
    void test_server(int numTuplas);
};

#endif
