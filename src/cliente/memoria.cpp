//*****************************************************************
// File:  memoria.cpp
// Authors:   GONZÁLEZ VILLA, DANIEL
//            NAYA FORCANO, ABEL
//            GONZÁLEZ GORRADO, JESÚS ÁNGEL
//            GARCÍA DÍAZ, ÁLVARO
// Date:   Diciembre 2018-Enero 2019
//*****************************************************************

/*
** Fichero de tests de la memoria técnica (crean ficheros .csv), para usar con Cliente.cpp **

** Uso:
	Para cada test, añadir una o más funciones con parámetros (char* ip, char* port, int i, char* param) siendo
		ip, port la ip:puerto del servidor (usar como LindaDriver(ip,port);)
		i es el identificador de la instancia actual (contador empezando en 0)
		param es el parámetro lanzado por línea de comandos
	
	Después añadir la macro ADDFUNCTION(nombre,descripción) siendo
		name el nombre de la función anterior (añadir tantas macros como funciones)
		descripción un string que se imprime al definir el test

** Ejemplo:
	Si el fichero tiene el siguiente contenido:

		void myTestFunction(char* ip, int port, int i, char* param){
			LindaDriver ld(ip, port);
			
			cout << "I am the instance " << i << " launched with parameter " << param << endl;
			//do things
		}
		
		ADDFUNCTION(myTestFunction,"Example, run as 'N myTestFunction M'")

	Al lanzar como "$run <ip> <port> 10 test 0" se mostrará lo siguiente:
		I am the instance 0 launched with parameter 0
		I am the instance 1 launched with parameter 0
		I am the instance 2 launched with parameter 0
		I am the instance 3 launched with parameter 0
		I am the instance 4 launched with parameter 0
		I am the instance 5 launched with parameter 0
		I am the instance 6 launched with parameter 0
		I am the instance 7 launched with parameter 0
		I am the instance 8 launched with parameter 0
		I am the instance 9 launched with parameter 0
*/
 

//------------------------------
//---Gráfica dependiendo del número de tuplas
//------------------------------

void t_medio(char* ip, int port, int i, char* param){
  LindaDriver scb(ip, port);
  
  int repetir = atoi(param);
  
  Tupla tamano("numero", "?B");
  Tupla info("tupla", "", "", "", "");	//tupla,tamaño,PN,readN,RN
  
  for(int j = 0; j < repetir; ++j){
	  
	Tupla querer = scb.RN(tamano);
	int max = stoi(querer[1]);
	
    for(int k = 1; k <= 6; ++k){

	  Tupla a("tupla");
	  
	  if(k == 2){
		Tupla a("tupla", "tupla");
	  } else if(k == 3){
		Tupla a("tupla", "tupla", "tupla");
	  } else if(k == 4){
		Tupla a("tupla", "tupla", "tupla", "tupla");
	  } else if(k == 5){
		Tupla a("tupla", "tupla", "tupla", "tupla", "tupla");
	  } else if(k == 6){
		Tupla a("tupla", "tupla", "tupla", "tupla", "tupla", "tupla");
	  }
	  
	  float tPN, treadN, tRN;
	  
	  tPN = time_LD(scb, max, a, 1);
	  treadN = time_LD(scb, max, a, 0);
	  tRN = time_LD(scb, max, a, -1);
	  
	  info[1] = to_string(k);
	  info[2] = to_string(tPN);
	  info[3] = to_string(treadN);
	  info[4] = to_string(tRN);
	  scb.PN(info);
	  
	  if(k == 1) {
		a.from_string("[prove]");
	  } else if(k == 2){
		a.from_string("[prove,prove]");
	  } else if(k == 3){
		a.from_string("[prove,prove,prove]");
	  } else if(k == 4){
		a.from_string("[prove,prove,prove,prove]");
	  } else if(k == 5){
		a.from_string("[prove,prove,prove,prove,prove]");
	  } else{
		a.from_string("[prove,prove,prove,prove,prove,prove]");
	  }
	  
	  tPN = time_LD(scb, max, a, 1);
	  treadN = time_LD(scb, max, a, 0);
	  tRN = time_LD(scb, max, a, -1);
	  
	  info[1] = to_string(k);
	  info[2] = to_string(tPN);
	  info[3] = to_string(treadN);
	  info[4] = to_string(tRN);
	  scb.PN(info);
	  
	}
  }
}
//-----------------------------------------------------
void t_cond(char* ip, int port, int _, char* param){
  LindaDriver scb(ip, port);

  int max = atoi(param);
  
  ofstream graf[6];
  
  graf[0].open("numTuplas-1.csv");
  graf[1].open("numTuplas-2.csv");
  graf[2].open("numTuplas-3.csv");
  graf[3].open("numTuplas-4.csv");
  graf[4].open("numTuplas-5.csv");
  graf[5].open("numTuplas-6.csv");

  Tupla tamano("numero", "1000");
  Tupla info("tupla", "?P", "?X", "?B", "?N");	//tupla,tamaño,PN,readN,RN
  
  for(int i = 0; i < 6; ++i){
	if(graf[i].is_open()){
	  graf[i] << "Número de tuplas,PN,readN,RN,PN (FS),readN (FS),RN (FS)" << endl;
	}
  }
  
  scb.PN(tamano);
  
  for(int i = 0; i < max; ++i){
	for(int j = 0; j < 6; ++j){
	  Tupla querer = scb.RN(info);
	  graf[j] << tamano[1] << "," << querer[2] << "," << querer[3] << "," << querer[4];
	  querer = scb.RN(info);
	  graf[j] << "," << querer[2] << "," << querer[3] << "," << querer[4] << endl;;
	}
	int n = stoi(tamano[1]);
	tamano[1] = to_string(n + 1000);
	scb.PN(tamano);
  }
  
  for(int i = 0; i < 6; ++i){
	cout << "Creado fichero numTuplas-" << i + 1 << ".csv con tuplas de tamaño " << i + 1 << endl;
  }
  
}

//--------------------------------------------------

ADDFUNCTION( t_medio, "Gráfico, ejecutar como '1 t_medio N' siendo N el número de veces");
ADDFUNCTION( t_cond, "Gráfico, ejecutar como '1 t_cond N' siendo N el número de veces");