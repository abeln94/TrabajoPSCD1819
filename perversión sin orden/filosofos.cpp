//*****************************************************************
// File:   Cliente.cpp
// Author: Álvaro García Díaz
//	   Abel Naya Forcano
//	   NOMBRE
//	   NOMBRE
// Date:   Enero 2019
// Coms:   invocar mediante
//				./Cliente <IP_Linda> <Port_Linda> 
//*****************************************************************

#include <iostream>
#include <thread>
#include <stdlib.h>
#include <string>
//#include "LindaDriver.hpp"
#include "scoreboard.hpp"
#include "tupla.hpp"

using namespace std;

// Duerme el proceso un número aleatorio de milisegundos entre minMillis
// (incluido) y maxMillis (incluido)
void esperar(int minMillis, int maxMillis) {
  this_thread::sleep_for(
      chrono::milliseconds(rand() % (maxMillis - minMillis + 1) + minMillis));
}
//-----------------------------------------------------
void t_worker(Scoreboard& scb) {

  Tupla init("init");
  scb.readN(init);

  Tupla t_hacer("hacer", "?b", "?s");
  Tupla t_hecho("hecho", "");
  while (true) {
    Tupla t = scb.RN(t_hacer);
    if (t[1] == "salir") {
      break;
    }
    cout << "Ordenando fichero >" << t[2] << endl;
    esperar(100, 1000);
    cout << "Fichero ordenado >" << t[2] << endl;
    t_hecho[1] = t[2];
    scb.PN(t_hecho);
  }
}
//-----------------------------------------------------
void t_supervisor(Scoreboard& scb, int M) {
  Tupla t_hacer("hacer", "ordenar", "");
  for (int i = 0; i < M; ++i) {
    t_hacer[2] = "fichero_" + to_string(i);
    scb.PN(t_hacer);
  }

  Tupla init("init");
  scb.PN(init);

  Tupla t_hecho("hecho", "");
  for (int i = 0; i < M; ++i) {
    t_hecho[1] = "fichero_" + to_string(i);
    scb.RN(t_hecho);
  }

  t_hacer[1] = "salir";
  t_hacer[2] = "";
  for (int i = 0; i < M; ++i) {
    scb.PN(t_hacer);
  }
}
//-----------------------------------------------------
void t_filosofo(Scoreboard& scb, int i, int max) {

  Tupla init("init");
  scb.readN(init);

  Tupla coger("filosofo", "coger", to_string(i));
  Tupla comer("filosofo", "comer", to_string(i));
  Tupla terminar("filosofo", "terminar", to_string(i));

  while (true) {
    scb.PN(coger);
    scb.RN(comer);
    cout << "\tComiendo filosofo " << i << endl;
    esperar(100, 1000);
    scb.PN(terminar);
    cout << "Pensando filosofo " << i << endl;
  }

}
//-----------------------------------------------------
void t_dejar(Scoreboard& scb, int max) {

  Tupla tenedor("filosofo", "tenedor", "");
  Tupla terminar("filosofo", "terminar", "?b");

  Tupla init("init");
  scb.readN(init);

  while(true){
	Tupla querer = scb.RN(terminar);
	int n = stoi(querer[2]);
	tenedor[2] = querer[2];
	scb.PN(tenedor);
	tenedor[2] = to_string((n + 1)%max);
	scb.PN(tenedor);
  }
}

//-----------------------------------------------------
void t_coger(Scoreboard& scb, int max) {

  Tupla coger("filosofo", "coger", "?b");
  Tupla tenedor("filosofo", "tenedor", "");
  Tupla comer("filosofo", "comer", "");

  for (int i = 0; i < max; ++i) {
    tenedor[2] = to_string(i);
    scb.PN(tenedor);
  }

  Tupla init("init");
  scb.PN(init);

  while(true){
	Tupla querer = scb.RN(coger);
	int n = stoi(querer[2]);
	tenedor[2] = querer[2];
	scb.RN(tenedor);
	tenedor[2] = to_string((n + 1)%max);
	scb.RN(tenedor);
	comer[2] = querer[2];
	scb.PN(comer);
  }
}
//-----------------------------------------------------
int main(int argc, char* argv[]) {
	/*
	//Ejecutar como ./Cliente <IP_Linda> <Port_Linda>
	if(argc != 3){
		cerr << "[X] Número incorrecto de parámetros" << endl;
		cerr << "[X] ./Cliente <IP_Linda> <Port_Linda>" << endl;
		return 0;
	}
	*/

  Scoreboard scb;

  int N;
  cout << "Número de procesos: " << flush;
  cin >> N;
  
  thread w[N];
  thread d, c;

  for (int i = 0; i < N; ++i) {
    w[i] = thread(&t_filosofo, ref(scb), i, N);
  }
  c = thread(&t_coger, ref(scb), N);
  d = thread(&t_dejar, ref(scb), N);

  for (int i = 0; i < N; ++i) {
    w[i].join();
  }
  d.join();
  c.join();
/*
  // Abel
  // ejercicio de ordenacion de ficheros
  N = 10;
  thread h[N];

  for (int i = 0; i < N; ++i) {
    h[i] = thread(&t_worker, ref(scb));
  }
  s = thread(&t_supervisor, ref(scb), 100);

  for (int i = 0; i < N; ++i) {
    h[i].join();
  }
  s.join();
  */
  return 0;
}
