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
  cntrPH2 = 0;
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

void ControlSys::endPH2(){
  unique_lock<mutex> lck(mtx);
  while(cntrPH2 != 0){
    endPH2_cv.wait(lck);
  }
}

void ControlSys::endPH3(){
  unique_lock<mutex> lck(mtx);
  while(cntrPH3 != 0){
    endPH3_cv.wait(lck);
  }
}

int ControlSys::sumPH2(int n){
  unique_lock<mutex> lck(mtx);
  cntrPH2 = cntrPH2 + n;
  if(cntrPH2 == 0){
    endPH2_cv.notify_one();
  }
  return cntrPH2;
}


int ControlSys::sumPH3(int n){
  unique_lock<mutex> lck(mtx);
  cntrPH3 = cntrPH3 + n;
  if(cntrPH3 == 0){
    endPH3_cv.notify_one();
  }
  return cntrPH3;
}
