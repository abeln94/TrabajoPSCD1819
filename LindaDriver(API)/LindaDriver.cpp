//******************************************************************
// File:  LindaDriver.cpp
// Authors:   NOMBRES
//            NOMBRES
//            NOMBRES
//            NOMBRES
// Date:   Diciembre 2018-Enero 2019
//*****************************************************************

#include "LindaDriver.hpp"

//...

LindaDriver::LindaDriver(string ip, int puerto){
  ip_serv = ip;
  port_serv = puerto;
  soc_serv = new Socket(ip, puerto);
  serv_fd = soc_serv -> Connect();

  cout << "[Linda] Iniciando conexión con servidores . . ." << endl;

  //AHAHAHAHHA
  string buffer;
  int length = 100;
  //Buffer content: 1:000.000.000.000,XXXX|2:000.000.000.000,XXXX|3:000.000.000.000,XXXX
  int rcv_bytes = soc_serv -> Recv(serv_fd, buffer, length);
  if(rcv_bytes == -1){
    mensError = strerror(errno);
    cerr << "[Linda] Error al recibir/enviar datos: " + mensError + "\n";
    soc_serv -> Close(serv_fd);
    return;
  }

  //Asumimos que el mensaje es correcto????
  for(int i = 0; i<3; i++){
    size_t found1 = buffer.find_first_of(",",0);
    size_t found2 = buffer.find_first_of("|",0);
    rellenar(buffer[0]-'0',buffer.substr(2,found1-2),atoi(buffer.substr(found1+1,found2-1).c_str()));
    buffer.erase(0,found2);
  }

  /*
  //Tratamiento msg
  bool correcto = true;
  //end
  if(!correcto){
    int snd_bytes = soc_serv -> Send(serv_fd, "ERROR");
    if(snd_bytes == -1){
      mensError = strerror(errno);
      cerr << "[Linda] Error al recibir/enviar datos: " + mensError + "\n";
    }
    soc_serv -> Close(serv_fd);
    return;
  }*/

  //corecto
  int snd_bytes = soc_serv -> Send(serv_fd, "OK");
  if(snd_bytes == -1){
    mensError = strerror(errno);
    cerr << "[Linda] Error al recibir/enviar datos: " + mensError + "\n";
    soc_serv -> Close(serv_fd);
    return;
    }
  s1_ready = false;
  s2_ready = false;
  s3_ready = false;

  soc_serv -> Close(serv_fd);
}

LindaDriver::~LindaDriver(){
  //Cerramos conexiones restantes
  if(s1_ready){
    int snd_bytes = soc_s1 -> Send(s1_fd, "END");
    if(snd_bytes == -1){
      mensError = strerror(errno);
      cerr << "[Linda] Error al enviar marca finalización: " + mensError + "\n";
    }
    soc_s1 -> Close(s1_fd);
  }
  if(s2_ready){
    int snd_bytes = soc_s2 -> Send(s2_fd, "END");
    if(snd_bytes == -1){
      mensError = strerror(errno);
      cerr << "[Linda] Error al enviar marca finalización: " + mensError + "\n";
    }
    soc_s2 -> Close(s2_fd);
  }
  if(s3_ready){
    int snd_bytes = soc_s3 -> Send(s3_fd, "END");
    if(snd_bytes == -1){
      mensError = strerror(errno);
      cerr << "[Linda] Error al enviar marca finalización: " + mensError + "\n";
    }
    soc_s3 -> Close(s3_fd);
  }

  //END
  delete[] soc_serv;
  delete[] soc_s1;
  delete[] soc_s2;
  delete[] soc_s3;
}

//Pendiente de usar, igual la quito (SOLO SE USA EN CONSTRUCTOR)
LindaDriver::void rellenar(int id, string ip, int port){
  if(id == 1){
    port_s1 = port;
    ip_s1 = ip;
  } else if (id == 2){
    port_s2 = port;
    ip_s2 = ip;
  } else if (id == 3){
    port_s3 = port;
    ip_s3 = ip;
  }
  return;
}

LindaDriver::void test_server(int numTuplas);{
  if(numTuplas == 1 || numTuplas == 2 || numTuplas == 3){
    if(!s1_ready){
      s1_ready = true;
      soc_s1 = new Socket(ip_s1, port_s1);
      s1_fd = soc_s1 -> Connect();
    }
  } else if (numTuplas == 4 || numTuplas == 5){
    if(!s2_ready){
      s2_ready = true;
      soc_s2 = new Socket(ip_s2, port_s2);
      s2_fd = soc_s2 -> Connect();
    }
  } else if (numTuplas == 6){
    if(!s3_ready){
      s3_ready = true;
      soc_s3 = new Socket(ip_s3, port_s3);
      s3_fd = soc_s3 -> Connect();
    }
  } else {
    cout << "[Linda] Error en test_server: numTuplas > 6 || numTuplas < 1" << endl;
  }
  return;
}
