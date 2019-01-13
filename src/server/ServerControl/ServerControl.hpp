//*****************************************************************
// File:  ServerControl.hpp
// Authors:   GONZÁLEZ VILLA, DANIEL
//            NAYA FORCANO, ABEL
//            GONZÁLEZ GORRADO, JESÚS ÁNGEL
//            GARCÍA DÍAZ, ÁLVARO
// Date:   Diciembre 2018-Enero 2019
//*****************************************************************

#ifndef SERVER_CONTROL
#define SERVER_CONTROL

#include <iostream>
#include <Socket.hpp>
#include <string>
#include <mutex>
#include <condition_variable>

using namespace std;

class ControlSys {
 public:
  // Constructor
  ControlSys(int priv_p, int pub_p);

  // Destructor
  ~ControlSys();

  void fill(int n, int fd, int port, string ip);

  string ips_to_string();

  void safe_print(string c);

  void err_safe_print(string c);

  bool ready();

  void end(Socket &priv);

  void endPH3();

  int sumPH3(int n);

  void clear(Socket &priv);

  int size(Socket &priv);

 private:
  int private_port, public_port;
  mutex mtx;
  int s1_fd, s2_fd, s3_fd;
  string ip_s1, ip_s2, ip_s3;
  int port_s1, port_s2, port_s3;
  bool s1_b, s2_b, s3_b;
  int cntrPH3;
  condition_variable endPH3_cv;
};

#endif
