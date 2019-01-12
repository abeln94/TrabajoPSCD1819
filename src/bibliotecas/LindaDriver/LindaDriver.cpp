//*****************************************************************
// File:  LindaDriver.cpp
// Authors:   GONZÁLEZ VILLA, DANIEL
//            NAYA FORCANO, ABEL
//            GONZÁLEZ GORRADO, JESÚS ÁNGEL
//            GARCÍA DÍAZ, ÁLVARO
// Date:   Diciembre 2018-Enero 2019
//*****************************************************************

#include "LindaDriver.hpp"

#ifdef ALLOW_LOCAL
	//alows running in local having a global scoreboard object
	#include "Scoreboard.hpp"
	Scoreboard _lindaDriver_scoreboards[6];
	bool _lindaDriver_local = false;
#endif


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

  //Buffer content: 1:000.000.000.000,XXXX|2:000.000.000.000,XXXX|3:000.000.000.000,XXXX
  string buffer;
  int length = 100;
  int rcv_bytes = soc_serv -> Recv(serv_fd, buffer, length);
  if(rcv_bytes == -1){
    string mensError = strerror(errno);
    cerr << "[Linda] Error al recibir/enviar datos: " + mensError + "\n";
    soc_serv -> Close(serv_fd);
    exit(1);
  }

  for(int i = 0; i<3; i++){
    size_t found1 = buffer.find_first_of(",",0);
    size_t found2 = buffer.find_first_of("|",0);
    rellenar(buffer[0]-'0',buffer.substr(2,found1-2),atoi(buffer.substr(found1+1,found2-1).c_str()));
    buffer.erase(0,found2+1);
  }

  int snd_bytes = soc_serv -> Send(serv_fd, "OK");
  if(snd_bytes == -1){
    string mensError = strerror(errno);
    cerr << "[Linda] Error al recibir/enviar datos: " + mensError + "\n";
    soc_serv -> Close(serv_fd);
    exit(1);
  }

  s1_ready = false;
  s2_ready = false;
  s3_ready = false;

  soc_serv -> Close(serv_fd);
  cout << "[Linda] Inicializado" << endl;
}

LindaDriver::~LindaDriver(){
	#ifdef ALLOW_LOCAL
		if(_lindaDriver_local){
			return;
		}
	#endif

  //Cerramos conexiones restantes
	if(s1_ready){
		soc_s1 -> Close(s1_fd);
	}
	if(s2_ready){
		soc_s2 -> Close(s2_fd);
	}
	if(s3_ready){
		soc_s3 -> Close(s3_fd);
	}

  delete soc_serv;
  delete soc_s1;
  delete soc_s2;
  delete soc_s3;
}

void LindaDriver::rellenar(int id, string ip, int port){
	switch (id) {
		case 1:
			port_s1 = port;
			ip_s1 = ip;
			break;
		case 2:
			port_s2 = port;
			ip_s2 = ip;
			break;
		case 3:
			port_s3 = port;
    	ip_s3 = ip;
			break;
		default:
			break;
	}
}

void LindaDriver::test_server(int numTuplas){
	switch (numTuplas) {
		case 1:
		case 2:
		case 3:
			if(!s1_ready){
      	s1_ready = true;
      	soc_s1 = new Socket(ip_s1, port_s1);
      	s1_fd = soc_s1 -> Connect();
    	}
    	break;
		case 4:
		case 5:
			if(!s2_ready){
				s2_ready = true;
				soc_s2 = new Socket(ip_s2, port_s2);
				s2_fd = soc_s2 -> Connect();
			}
			break;
		case 6:
			if(!s3_ready){
				s3_ready = true;
				soc_s3 = new Socket(ip_s3, port_s3);
				s3_fd = soc_s3 -> Connect();
			}
			break;
  	default:
    	cout << "[Linda] Error en test_server: numTuplas > 6 || numTuplas < 1" << endl;
  }
}

void LindaDriver::PN(Tupla mensaje){
		#ifdef ALLOW_LOCAL
	if(_lindaDriver_local){
		_lindaDriver_scoreboards[mensaje.size()-1].PN(mensaje);
		return;
	}
		#endif

	test_server(mensaje.size());
	int snd_bytes, rcv_bytes;

	//Añadimos "1" para que el subservidor sepa que accion realizar
	//		y el tamaño de la tupla para que pueda tratar el mensaje.
	string mens = mensaje.to_string() + to_string(mensaje.size()) + "1";
	switch (mensaje.size()) {
		case 1:
		case 2:
		case 3:
			snd_bytes = soc_s1->Send(s1_fd, mens);
			break;
		case 4:
		case 5:
			snd_bytes = soc_s2->Send(s2_fd, mens);
			break;
		case 6:
			snd_bytes = soc_s3->Send(s3_fd, mens);
			break;
		default:
			cout << "[Linda][PN] Numero de elementos de Tupla invalido" << endl;
			exit(0);
	}
	if(snd_bytes == -1){
			string mensError = strerror(errno);
			cerr << "[Linda][PN] Error al enviar mensaje al subservidor: " + mensError + "\n";
			exit(0);
	}
	string buffer;
	switch (mensaje.size()) {
		case 1:
		case 2:
		case 3:
			rcv_bytes = soc_s1->Recv(s1_fd, buffer, 1000);
			break;
		case 4:
		case 5:
			rcv_bytes = soc_s2->Recv(s2_fd, buffer, 1000);
			break;
		case 6:
			rcv_bytes = soc_s3->Recv(s3_fd, buffer, 1000);
			break;
		default:
			cout << "[Linda][PN] Numero de elementos de Tupla invalido" << endl;
			exit(0);
			break;
	}
	if (rcv_bytes == -1){
			string mensError = strerror(errno);
			cerr << "[Linda][PN] Error al recibir mensaje del subservidor: " + mensError + "\n";
			exit(0);
	}
	if(buffer != "OK"){
		cout << "[Linda][PN] Error en tupla" << mens << endl;
	}
}

Tupla LindaDriver::RN(Tupla mensaje){
		#ifdef ALLOW_LOCAL
	if(_lindaDriver_local){
		return _lindaDriver_scoreboards[mensaje.size()-1].RN(mensaje);
	}
		#endif

	test_server(mensaje.size());
	int snd_bytes, rcv_bytes;

	//Añadimos "2" para que el subservidor sepa que accion realizar
	//			 y el tamaño de la tupla para que pueda tratar el mensaje.
	string mens = mensaje.to_string() + to_string(mensaje.size()) + "2";
	switch (mensaje.size()) {
		case 1:
		case 2:
		case 3:
			snd_bytes = soc_s1->Send(s1_fd, mens);
			break;
		case 4:
		case 5:
			snd_bytes = soc_s2->Send(s2_fd, mens);
			break;
		case 6:
			snd_bytes = soc_s3->Send(s3_fd, mens);
			break;
		default:
			cout << "[Linda][RN] Numero de elementos de Tupla invalido" << endl;
			exit(0);
	}
	if(snd_bytes == -1){
			string mensError = strerror(errno);
			cerr << "[Linda][RN] Error al enviar mensaje al subservidor: " + mensError + "\n";
			exit(0);
	}
	string buffer;
	switch (mensaje.size()) {
		case 1:
		case 2:
		case 3:
			rcv_bytes = soc_s1->Recv(s1_fd, buffer, 1000);
			break;
		case 4:
		case 5:
			rcv_bytes = soc_s2->Recv(s2_fd, buffer, 1000);
			break;
		case 6:
			rcv_bytes = soc_s3->Recv(s3_fd, buffer, 1000);
			break;
		default:
			cout << "[Linda][RN] Numero de elementos de Tupla invalido" << endl;
			exit(0);
			break;
	}
	if (rcv_bytes == -1){
			string mensError = strerror(errno);
			cerr << "[Linda][RN] Error al recibir mensaje del subservidor: " + mensError + "\n";
			exit(0);
	}
	Tupla mens_final(mensaje.size());
	bool crear = mens_final.from_string(buffer);
	if(crear){
			return mens_final;
	} else {
			cout << "[Linda][RN] Error en la tupla enviado por el subservidor" << endl;
	}
}

Tupla LindaDriver::readN(Tupla mensaje){
		#ifdef ALLOW_LOCAL
	if(_lindaDriver_local){
		return _lindaDriver_scoreboards[mensaje.size()-1].readN(mensaje);
	}
		#endif

	test_server(mensaje.size());
	int snd_bytes, rcv_bytes;

	//Añadimos "3" para que el subservidor sepa que accion realizar
	//     y el tamaño de la tupla para que pueda tratar el mensaje.
	string mens = mensaje.to_string() + to_string(mensaje.size()) + "3";
	switch (mensaje.size()) {
		case 1:
		case 2:
		case 3:
			snd_bytes = soc_s1->Send(s1_fd, mens);
			break;
		case 4:
		case 5:
			snd_bytes = soc_s2->Send(s2_fd, mens);
			break;
		case 6:
			snd_bytes = soc_s3->Send(s3_fd, mens);
			break;
		default:
			cout << "[Linda][readN] Numero de elementos de Tupla invalido" << endl;
			exit(0);
	}
	if(snd_bytes == -1){
			string mensError = strerror(errno);
			cerr << "[Linda][readN] Error al enviar mensaje al subservidor: " + mensError + "\n";
			exit(0);
	}
	string buffer;
	switch (mensaje.size()) {
		case 1:
		case 2:
		case 3:
			rcv_bytes = soc_s1->Recv(s1_fd, buffer, 1000);
			break;
		case 4:
		case 5:
			rcv_bytes = soc_s2->Recv(s2_fd, buffer, 1000);
			break;
		case 6:
			rcv_bytes = soc_s3->Recv(s3_fd, buffer, 1000);
			break;
		default:
			cout << "[Linda][readN] Numero de elementos de Tupla invalido" << endl;
			exit(0);
			break;
	}
	if (rcv_bytes == -1){
			string mensError = strerror(errno);
			cerr << "[Linda][readN] Error al recibir mensaje del subservidor: " + mensError + "\n";
			exit(0);
	}
	Tupla mens_final(mensaje.size());
	bool crear = mens_final.from_string(buffer);
	if(crear){
			return mens_final;
	} else {
			cout << "[Linda][readN] Error en la tupla enviado por el subservidor" << endl;
	}

}
