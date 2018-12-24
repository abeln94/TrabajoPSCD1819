//******************************************************************
// File:  non_defined
// Authors:   Daniel González
//            NOMBRES
//            NOMBRES
//            NOMBRES
// Date:   Diciembre 2018
//*****************************************************************

#include "ServerControl.hpp"



ControlSys::ControlSys(int priv_p, int pub_p){
  private_port = priv_p;
  public_port = pub_p;
  s1_b = false;
  s2_b = false;
  s3_b = false;
  /*
  soc_priv = new Socket(priv_p);
  soc_publ = new Socket(pub_p);

  //Bind
  soc_priv_fd = soc_priv -> Bind();
  if (soc_priv_fd == -1) {
    string mensError(strerror(errno));
      cerr << "--Error en el bind: " + mensError + "\n";
    exit(1);
  }

  //Listen
  int errcode1 = soc_priv -> Listen(5);
  if (errcode1 == -1) {
    string mensError(strerror(errno));
      cerr << "--Error en el listen: " + mensError + "\n";
    // Cerramos el socket
    soc_priv -> Close(soc_priv_fd);
    exit(1);
  }


  //Bind
  soc_publ_fd = soc_publ -> Bind();
  if (soc_publ_fd == -1) {
    string mensError(strerror(errno));
      cerr << "--Error en el bind: " + mensError + "\n";
    exit(1);
  }

  // Listen
  int errcode2 = soc_publ -> Listen(100);
  if (errcode2 == -1) {
    string mensError(strerror(errno));
      cerr << "--Error en el listen: " + mensError + "\n";
    // Cerramos el socket
    soc_publ -> Close(soc_publ_fd);
    exit(1);
  }*/
}

ControlSys::~ControlSys(){

}


void ControlSys::fill(int n, int fd, int port, string ip){
  unique_lock<mutex> lck(mtx);
  switch(n){
    case 1:
      if(!s3_b){
        s1_fd = fd;
        port_s1 = port;
        ip_s1 = ip;
        s1_b = true;
      }
      break;
    case 2:
      if(!s3_b){
        s2_fd = fd;
        port_s2 = port;
        ip_s2 = ip;
        s2_b = true;
      }
      break;
    case 3:
      if(!s3_b){
        s3_fd = fd;
        port_s3 = port;
        ip_s3 = ip;
        s3_b = true;
      }
      break;
    default:
      cout << "[x] Error: Value assigned is not correct: fill(" << n << ",?,?,?)" << endl;
  }
}

void ControlSys::safe_print(string c){
  unique_lock<mutex> lck(mtx);
  cout << c << endl;
}

void ControlSys::err_safe_print(string c){
  unique_lock<mutex> lck(mtx);
  cerr << c << endl;
}

bool ControlSys::ready(){
  return s1_b&&s2_b&&s3_b;
}
