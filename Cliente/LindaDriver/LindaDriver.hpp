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
#include "Tuplas.hpp"
#include <string>
//#include <mutex>
//#include <condition_variable>

using namespace std;

//...

class LindaDriver{
  public:
    LindaDriver(string ip, int puerto);

    ~LindaDriver();
    //Operaciones de Linda
    void PN (Tupla mensaje);
    Tupla RN (Tupla mensaje);
    Tupla readN (Tupla mensaje);


    //...

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
