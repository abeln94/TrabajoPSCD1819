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
	
	Canal servidor(ip, puerto, 10, 1000);
			
	string buffer;
	
  //Buffer content: 1:000.000.000.000,XXXX|2:000.000.000.000,XXXX|3:000.000.000.000,XXXX
	servidor >> buffer;

  //Asumimos que el mensaje es correcto????
  for(int i = 0; i<3; i++){
    size_t found1 = buffer.find_first_of(",",0);
    size_t found2 = buffer.find_first_of("|",0);
		canal[ buffer[0]-'0'-1 ] = new Canal(
					buffer.substr(2,found1-2),  // ip
					atoi(buffer.substr(found1+1,found2-1).c_str()),  // port
					10, 1000);
    
		buffer.erase(0,found2+1);
  }

  cout << "inicializado" << endl;
	// la conexión con el servidor se cierra automáticamente
}

LindaDriver::~LindaDriver(){
	#ifdef ALLOW_LOCAL
		if(_lindaDriver_local){
			return;
		}
	#endif

  //Cerramos conexiones restantes
	for(int i=0;i<3;++i){
		delete(canal[i]);
	}
}

void LindaDriver::PN(Tupla mensaje){
		#ifdef ALLOW_LOCAL
		if(_lindaDriver_local){
			_lindaDriver_scoreboard.PN(mensaje);
			return;
		}
		#endif
        
		
		string buffer;
		Canal& c = *getCanal(mensaje.size());
		
    //Añadimos "1" para que el subservidor sepa que accion realizar, y el tamaño de la tupla para que pueda tratar el mensaje.
    string mens = mensaje.to_string() + to_string(mensaje.size()) + "1";
		cout << mens << endl;
		c << mens;
		
		cout << "test" << endl;
		
		c >> buffer;
		if(buffer == "OK"){
				cout << "OK" << endl;
		}
		else{
				cout << "Error en PN " << mens << endl;
		}
}

Tupla LindaDriver::RN(Tupla mensaje){
	#ifdef ALLOW_LOCAL
		if(_lindaDriver_local){
			return _lindaDriver_scoreboard.RN(mensaje);
		}
	#endif

        //test_server(mensaje.size());
				
		
		string buffer;
		Canal& c = *getCanal(mensaje.size());
				
    //Añadimos "2" para que el subservidor sepa que accion realizar, y el tamaño de la tupla para que pueda tratar el mensaje.
    string mens = mensaje.to_string() + to_string(mensaje.size()) + "2";
		c << mens;
		
		c >> buffer;
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

Tupla LindaDriver::readN(Tupla mensaje){
		#ifdef ALLOW_LOCAL
			if(_lindaDriver_local){
				return _lindaDriver_scoreboard.readN(mensaje);
			}
		#endif
        
        //test_server(mensaje.size());
    
		string buffer;
		Canal& c = *getCanal(mensaje.size());
				
    //Añadimos "3" para que el subservidor sepa que accion realizar, y el tamaño de la tupla para que pueda tratar el mensaje.
    string mens = mensaje.to_string() + to_string(mensaje.size()) + "3";
		c << mens;
		
		c >> buffer;
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

Canal* LindaDriver::getCanal(const Tupla& note){
	switch(note.size()){
		case 1:
		case 2:
		case 3:
			return canal[0];
		case 4:
		case 5:
			return canal[1];
		case 6:
			return canal[2];
		default:
			cout << "Nota inválida: " << note << endl;
			exit(1);
	}
}
