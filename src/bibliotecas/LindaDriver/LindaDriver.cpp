//******************************************************************
// File:  LindaDriver.cpp
// Authors:   NOMBRES
//            NOMBRES
//            NOMBRES
//            NOMBRES
// Date:   Diciembre 2018-Enero 2019
//*****************************************************************

#include "LindaDriver.hpp"


#ifdef ALLOW_LOCAL
	//alows running in local having a global scoreboard object
	#include "Scoreboard.hpp"
	Scoreboard _lindaDriver_scoreboard;
	bool _lindaDriver_local = false;
#endif

//...

LindaDriver::LindaDriver(string ip, int puerto){
	#ifdef ALLOW_LOCAL
		if(ip == "local" && puerto == 0){
			_lindaDriver_local = true;
			return;
		}
	#endif
	

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
    string mensError = strerror(errno);
    cerr << "[Linda] Error al recibir/enviar datos: " + mensError + "\n";
    soc_serv -> Close(serv_fd);
    exit(1);
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
  /*
  int snd_bytes = soc_serv -> Send(serv_fd, "OK");
  if(snd_bytes == -1){
    string mensError = strerror(errno);
    cerr << "[Linda] Error al recibir/enviar datos: " + mensError + "\n";
    soc_serv -> Close(serv_fd);
    exit(1);
  }
  */
  s1_ready = false;
  s2_ready = false;
  s3_ready = false;

  soc_serv -> Close(serv_fd);
  cout << "inicializado" << endl;
}

LindaDriver::~LindaDriver(){
	#ifdef ALLOW_LOCAL
		if(_lindaDriver_local){
			return;
		}
	#endif

  //Cerramos conexiones restantes
	/*
  if(s1_ready){
    int snd_bytes = soc_s1 -> Send(s1_fd, "END");
    if(snd_bytes == -1){
      string mensError = strerror(errno);
      cerr << "[Linda] Error al enviar marca finalización: " + mensError + "\n";
    }
    soc_s1 -> Close(s1_fd);
  }
  if(s2_ready){
    int snd_bytes = soc_s2 -> Send(s2_fd, "END");
    if(snd_bytes == -1){
      string mensError = strerror(errno);
      cerr << "[Linda] Error al enviar marca finalización: " + mensError + "\n";
    }
    soc_s2 -> Close(s2_fd);
  }
  if(s3_ready){
    int snd_bytes = soc_s3 -> Send(s3_fd, "END");
    if(snd_bytes == -1){
      string mensError = strerror(errno);
      cerr << "[Linda] Error al enviar marca finalización: " + mensError + "\n";
    }
    soc_s3 -> Close(s3_fd);
  }*/

  //END
  delete[] soc_serv;
  delete[] soc_s1;
  delete[] soc_s2;
  delete[] soc_s3;
}

//Pendiente de usar, igual la quito (SOLO SE USA EN CONSTRUCTOR)
void LindaDriver::rellenar(int id, string ip, int port){
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

void LindaDriver::test_server(int numTuplas){
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

void LindaDriver::PN(Tupla mensaje){
        #ifdef ALLOW_LOCAL
		if(_lindaDriver_local){
			_lindaDriver_scoreboard.PN(mensaje);
			return;
		}
	    #endif
        test_server(mensaje.size());
    //Añadimos "1" para que el subservidor sepa que accion realizar, y el tamaño de la tupla para que pueda tratar el mensaje.
    string mens = mensaje.to_string() + to_string(mensaje.size()) + "1"; 
    if(mensaje.size() <=3){
        int snd_bytes = soc_s1->Send(s1_fd, mens);
        if(snd_bytes == -1){
            string mensError = strerror(errno);
            cerr << "[Linda] Error al enviar mensaje al subservidor 1: " + mensError + "\n";
            //Cerrar o no socket?
        }
        else{
            string buffer;
            int rcv_bytes = soc_s1->Recv(s1_fd, buffer, 1000);
            if (rcv_bytes == -1){
               string mensError = strerror(errno);
                cerr << "[Linda] Eror al recibir mensaje del subservidor 1: " + mensError + "\n";
            }
            else{
                if(buffer == "OK"){
                    cout << "OK" << endl;
                }
                else{
                    cout << "Error en PN " << mens << endl;
                }
            }
        }

    }
    else if(mensaje.size() || mensaje.size() == 5){
        int snd_bytes = soc_s2->Send(s2_fd, mens);
        if(snd_bytes == -1){
            string mensError = strerror(errno);
            cerr << "[Linda] Error al enviar mensaje al subservidor 2: " + mensError + "\n";
            //Cerrar o no socket?
        }    
        else{
            string buffer;
            int rcv_bytes = soc_s2->Recv(s2_fd, buffer, 1000);
            if (rcv_bytes == -1){
                string mensError = strerror(errno);
                cerr << "[Linda] Eror al recibir mensaje del subservidor 2: " + mensError + "\n";
            }
            else{
                if(buffer == "OK"){
                    cout << "OK" << endl;
                }
                else{
                    cout << "Error en PN " << mens << endl;
                }
            }            
        }        
    }
    else{
        int snd_bytes = soc_s3->Send(s3_fd,mens);
        if(snd_bytes == -1){
            string mensError = strerror(errno);
            cerr << "[Linda] Error al enviar mensaje al subservidor 3: " + mensError + "\n";
            //Cerrar o no socket?
        }   
        else{
            string buffer;
            int rcv_bytes = soc_s3->Recv(s3_fd, buffer, 1000);
            if (rcv_bytes == -1){
                string mensError = strerror(errno);
                cerr << "[Linda] Eror al recibir mensaje del subservidor 3: " + mensError + "\n";
            }
            else{
                if(buffer == "OK"){
                    cout << "OK" << endl;
                }
                else{
                    cout << "Error en PN " << mens << endl;
                }
            }
        }             
    }
}

Tupla LindaDriver::RN(Tupla mensaje){
	#ifdef ALLOW_LOCAL
		if(_lindaDriver_local){
			return _lindaDriver_scoreboard.RN(mensaje);
		}
	#endif

        test_server(mensaje.size());
    //Añadimos "2" para que el subservidor sepa que accion realizar, y el tamaño de la tupla para que pueda tratar el mensaje.
    string mens = mensaje.to_string() + to_string(mensaje.size()) + "2";
    if(mensaje.size() <=3){
        int snd_bytes = soc_s1->Send(s1_fd, mens);
        if(snd_bytes == -1){
            string mensError = strerror(errno);
            cerr << "[Linda] Error al enviar mensaje al subservidor 1: " + mensError + "\n";
            //Cerrar o no socket?
        }
        else{
            string buffer;
            int rcv_bytes = soc_s1->Recv(s1_fd, buffer, 1000);
            if (rcv_bytes == -1){
                string mensError = strerror(errno);
                cerr << "[Linda] Eror al recibir mensaje del subservidor 1: " + mensError + "\n";
            }
            else{
                Tupla mens_final(mensaje.size());
                bool crear = mens_final.from_string(buffer);
                if(crear){
                    cout << "OK" << endl;
                    return mens_final;
                }
                else{
                    cout << "Error en la tupla enviado por el subservidor 1" << endl;
                }
            }
        }

    }
    else if(mensaje.size() == 4 || mensaje.size() == 5){
        int snd_bytes = soc_s2->Send(s2_fd, mens);
        if(snd_bytes == -1){
            string mensError = strerror(errno);
            cerr << "[Linda] Error al enviar mensaje al subservidor 2: " + mensError + "\n";
            //Cerrar o no socket?
        }    
        else{
            string buffer;
            int rcv_bytes = soc_s2->Recv(s2_fd, buffer, 1000);
            if (rcv_bytes == -1){
                string mensError = strerror(errno);
                cerr << "[Linda] Eror al recibir mensaje del subservidor 2: " + mensError + "\n";
            }
            else{
                Tupla mens_final(mensaje.size());
                bool crear = mens_final.from_string(buffer);
                if(crear){
                    cout << "OK" << endl;
                    return mens_final;
                }
                else{
                    cout << "Error en la tupla enviado por el subservidor 2" << endl;
                }
            }
        }        
    }
    else{
        int snd_bytes = soc_s3->Send(s3_fd,mens);
        if(snd_bytes == -1){
            string mensError = strerror(errno);
            cerr << "[Linda] Error al enviar mensaje al subservidor 3: " + mensError + "\n";
            //Cerrar o no socket?
        }   
        else{
            string buffer;
            int rcv_bytes = soc_s3->Recv(s3_fd, buffer, 1000);
            if (rcv_bytes == -1){
                string mensError = strerror(errno);
                cerr << "[Linda] Eror al recibir mensaje del subservidor 3: " + mensError + "\n";
            }
            else{
                Tupla mens_final(mensaje.size());
                bool crear = mens_final.from_string(buffer);
                if(crear){
                    cout << "OK" << endl;
                    return mens_final;
                }
                else{
                    cout << "Error en la tupla enviado por el subservidor 3" << endl;
                }
            }
        }             
    }
}

Tupla LindaDriver::readN(Tupla mensaje){
        #ifdef ALLOW_LOCAL
		if(_lindaDriver_local){
			return _lindaDriver_scoreboard.readN(mensaje);
		}
	    #endif
        
        test_server(mensaje.size());
    
    //Añadimos "2" para que el subservidor sepa que accion realizar, y el tamaño de la tupla para que pueda tratar el mensaje.
    string mens = mensaje.to_string() + to_string(mensaje.size()) + "3";
    if(mensaje.size() <=3){
        int snd_bytes = soc_s1->Send(s1_fd, mens);
        if(snd_bytes == -1){
            string mensError = strerror(errno);
            cerr << "[Linda] Error al enviar mensaje al subservidor 1: " + mensError + "\n";
            //Cerrar o no socket?
        }
        else{
            string buffer;
            int rcv_bytes = soc_s1->Recv(s1_fd, buffer, 1000);
            if (rcv_bytes == -1){
                string mensError = strerror(errno);
                cerr << "[Linda] Eror al recibir mensaje del subservidor 1: " + mensError + "\n";
            }
            else{
                Tupla mens_final(mensaje.size());
                bool crear = mens_final.from_string(buffer);
                if(crear){
                    cout << "OK" << endl;
                    return mens_final;
                }
                else{
                    cout << "Error en la tupla enviado por el subservidor 1" << endl;
                }
            }
        }
    }
    else if(mensaje.size() == 4 || mensaje.size() == 5){
        int snd_bytes = soc_s2->Send(s2_fd, mens);
        if(snd_bytes == -1){
            string mensError = strerror(errno);
            cerr << "[Linda] Error al enviar mensaje al subservidor 2: " + mensError + "\n";
            //Cerrar o no socket?
        }    
        else{
            string buffer;
            int rcv_bytes = soc_s2->Recv(s2_fd, buffer, 1000);
            if (rcv_bytes == -1){
                string mensError = strerror(errno);
                cerr << "[Linda] Eror al recibir mensaje del subservidor 2: " + mensError + "\n";
            }
            else{
                Tupla mens_final(mensaje.size());
                bool crear = mens_final.from_string(buffer);
                if(crear){
                    cout << "OK" << endl;
                    return mens_final;
                }
                else{
                    cout << "Error en la tupla enviado por el subservidor 2" << endl;
                }
            }
        }        
    }
    else{
        int snd_bytes = soc_s3->Send(s3_fd,mens);
        if(snd_bytes == -1){
            string mensError = strerror(errno);
            cerr << "[Linda] Error al enviar mensaje al subservidor 3: " + mensError + "\n";
            //Cerrar o no socket?
        }   
        else{
            string buffer;
            int rcv_bytes = soc_s3->Recv(s3_fd, buffer, 1000);
            if (rcv_bytes == -1){
                string mensError = strerror(errno);
                cerr << "[Linda] Eror al recibir mensaje del subservidor 3: " + mensError + "\n";
            }
            else{
                Tupla mens_final(mensaje.size());
                bool crear = mens_final.from_string(buffer);
                if(crear){
                    cout << "OK" << endl;
                    return mens_final;
                }
                else{
                    cout << "Error en la tupla enviado por el subservidor 3" << endl;
                }
            }
        }             
    }
}

