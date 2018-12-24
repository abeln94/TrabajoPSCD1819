//******************************************************************
// File:  non_defined
// Authors:   Daniel González
//            NOMBRES
//            NOMBRES
//            NOMBRES
// Date:   Diciembre 2018
//*****************************************************************

#ifndef SERVER_CONTROL
#define SERVER_CONTROL

#include <iostream>
#include <Socket.hpp>
#include <string>
#include <mutex>
//#include <condition_variable>

using namespace std;

class ControlSys {
  public:
    //Constructor
    ControlSys(int priv_p, int pub_p);

    //Destructor
    ~ControlSys();

    void fill(int n, int fd, int port, string ip);

    void safe_print(string c);

    void err_safe_print(string c);

    bool ready();

    //Socket *soc_priv;
    //Socket *soc_publ;
    //int soc_publ_fd, soc_priv_fd ;

  private:
    int private_port, public_port;
    mutex mtx;
    int s1_fd, s2_fd, s3_fd;
    string ip_s1, ip_s2, ip_s3;
    int port_s1, port_s2, port_s3;
    bool s1_b, s2_b, s3_b;
};

#endif
