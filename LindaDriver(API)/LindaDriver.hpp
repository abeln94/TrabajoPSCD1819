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
#include <Socket.hpp>
//#include <string>
//#include <mutex>
//#include <condition_variable>

//using namespace std;

//...

struct LindaDriver{
  public:
    LindaDriver(string ip, int puerto);
    ~LindaDriver();

    //...

  private:
    string ip_serv, ip_s1, ip_s2, ip_s3;
    int port_serv, port_s1, port_s2, port_s3;
    Socket *soc_serv;
    Socket *soc_s1, *soc_s2, *soc_s3;



}

#endif
