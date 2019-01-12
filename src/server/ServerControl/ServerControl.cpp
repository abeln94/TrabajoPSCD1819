//*****************************************************************
// File:  ServerControl.cpp
// Authors:   GONZÁLEZ VILLA, DANIEL
//            NAYA FORCANO, ABEL
//            GONZÁLEZ GORRADO, JESÚS ÁNGEL
//            GARCÍA DÍAZ, ÁLVARO
// Date:   Diciembre 2018-Enero 2019
//*****************************************************************

#include "ServerControl.hpp"

ControlSys::ControlSys(int priv_p, int pub_p){
  private_port = priv_p;
  public_port = pub_p;
  s1_b = false;
  s2_b = false;
  s3_b = false;
  cntrPH3 = 0;
}

ControlSys::~ControlSys(){

}

void ControlSys::fill(int n, int fd, int port, string ip){
  unique_lock<mutex> lck(mtx);
  switch(n){
    case 1:
      if(!s1_b){
        s1_fd = fd;
        port_s1 = port;
        ip_s1 = ip;
        s1_b = true;
      }
      break;
    case 2:
      if(!s2_b){
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

string ControlSys::ips_to_string(){
  return "1:" + ip_s1 + "," + to_string(port_s1) + "|2:" + ip_s2 + "," +
          to_string(port_s2) + "|3:" + ip_s3 + "," + to_string(port_s3);
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
  if( s1_b && s2_b && s3_b){
    return true;
  }
  return false;
}

void ControlSys::end(Socket &priv){
  if(s1_b){
    priv.Send(s1_fd,"END");
    priv.Close(s1_fd);
  }
  if(s2_b){
    priv.Send(s2_fd,"END");
    priv.Close(s2_fd);
  }
  if(s3_b){
    priv.Send(s3_fd,"END");
    priv.Close(s3_fd);
  }
}

void ControlSys::clear(Socket &priv){
	string cmd = "CLEAR";
  if(s1_b){
    priv.Send(s1_fd,cmd);
  }
  if(s2_b){
    priv.Send(s2_fd,cmd);
  }
  if(s3_b){
    priv.Send(s3_fd,cmd);
  }
}

void ControlSys::endPH3(){
  unique_lock<mutex> lck(mtx);
  while(cntrPH3 != 0){
    endPH3_cv.wait(lck);
  }
}

int ControlSys::sumPH3(int n){
  unique_lock<mutex> lck(mtx);
  cntrPH3 = cntrPH3 + n;
  if(cntrPH3 == 0){
    endPH3_cv.notify_one();
  }
  return cntrPH3;
}
