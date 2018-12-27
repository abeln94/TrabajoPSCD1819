
#include <iostream>
#include <thread>
#include <string>
#include <Socket.hpp>

using namespace std;

// Duerme el proceso un número aleatorio de milisegundos entre minMillis
// (incluido) y maxMillis (incluido)
void esperar(int minMillis, int maxMillis) {
  this_thread::sleep_for(
      chrono::milliseconds(rand() % (maxMillis - minMillis + 1) + minMillis));
}
//-----------------------------------------------------
//Simula un cliente. Envía pings y espera pongs
void t_client(int port) {
	try{
		//iniciamos la comunicación con el servidor
		Canal servidor("localhost", port, 10, 1000);
		string res;
		
		//Primer envío, somos un cliente
		servidor << "cliente";
		
		
		while(true){
			//repetimos indefinidamente hasta que queramos salir
		
			//enviamos un ping
			if(rand()%100==0){
				cout << "-> eres feo servidor" << endl;
				servidor << "feo";
			}else{
				cout << "-> hola server!" << endl;
				servidor << "ping";
			}
			
			//esperamos el pong
			servidor >> res;
			if(res != "pong"){
				//la respuesta no es la esperada, salir
				cout << "que maleducado, pues ahora me voy" << endl;
				break;
			}
		}
	}catch(...){
		//ha habido algún error (el servidor se ha desconectado)
		cout << "el servidor se ha ido" << endl;
	}
}
//-----------------------------------------------------
//un thread del servidor que se encarga de tratar con un cliente concreto
void t_serverthread(Canal cliente) {
	try{
		string res;
		while(true){
			//repetimos indefinidamente hasta que el cliente se vaya
			
			//esperamos el ping
			cliente >> res;
			if(res != "ping"){
				//error en el ping, salir
				cout << "que maleducado, ahí te quedas" << endl;
				break;
			}
			
			//devolvemos el pong
			if(rand()%100==0){
				cout << "<- que feo eres cliente" << endl;
				cliente << "feo";
			}else{
				cout << "<- hola cliente" << endl;
				cliente << "pong";
			}
			
			//nos dormimos un poco
			esperar(0,200);
		}
	}catch(...){
		//algún error (el cliente se ha desconectado)
		cout << "el cliente ha desaparecido" << endl;
	}
}
//-----------------------------------------------------
//simula el servidor. Espera clientes y lanza threads para tratarlos
void t_server(int port) {
	
	//inicializamos el servidor
  Servidor server(port, 10);
	
	string res;
	
	while(true){
		//repetimos hasta que recibamos la orden concreta
		
		//esperamos a un nuevo cliente
		Canal& c = server.getCliente(); // al ponerlo como 'Cliente&' el objeto no se destruye automáticamente al finalizar el loop
		
		//para saber que tipo de cliente es
		c >> res;
		
		if(res == "salir"){
			//hay que cerrar el servidor
			delete(&c); //al ponerlo como 'Cliente&' no se destruye automáticamente, hay que hacerlo manual
			cout << "cerrando servidor" << endl;
			break;
		}else{
			//iniciamos un thread para tratarlo, y nos olvidamos de él
			thread(t_serverthread, ref(c)).detach();
		}
		
	}
}
//-----------------------------------------------------
int main(int argc, char* argv[]) {
	//simulación de un bash
	
	int port = 2000;

	// iniciar servidor
	thread s;
	s = thread(&t_server, port);
	esperar(1000, 1000);
	
	//iniciar clientes
	int C = 10;
	thread c[C];
	for(int i=0;i<C;++i){
		c[i] = thread(&t_client, port);
	}
	
	//esperar a que los clientes terminen
	for(int i=0;i<C;++i){
		c[i].join();
	}	
	
	//cerrar servidor (le enviamos un comando especial)
	Canal server("localhost", port, 10, 1000);
	server << "salir";
	
	s.join();
  return 0;
}
