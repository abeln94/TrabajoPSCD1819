//*****************************************************************
// File:  tests.cpp
// Authors:   GONZÁLEZ VILLA, DANIEL
//            NAYA FORCANO, ABEL
//            GONZÁLEZ GORRADO, JESÚS ÁNGEL
//            GARCÍA DÍAZ, ÁLVARO
// Date:   Diciembre 2018-Enero 2019
//*****************************************************************

/*
** Fichero de tests, para usar con Cliente.cpp **

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
//---Test de trabajadores y supervisor
//------------------------------

// Duerme el proceso un número aleatorio de milisegundos entre minMillis
// (incluido) y maxMillis (incluido)
void esperar(int minMillis, int maxMillis) {
  this_thread::sleep_for(
      chrono::milliseconds(rand() % (maxMillis - minMillis + 1) + minMillis));
}
//-----------------------------------------------------
void t_worker(char* ip, int port, int i, char* param){
  LindaDriver ld(ip, port);
	
  Tupla init("init");
  ld.readN(init);

  Tupla t_hacer("hacer", "?B", "?S");
  Tupla t_hecho("hecho", "");
  while (true) {
    Tupla t = ld.RN(t_hacer);
    if (t[1] == "salir") {
      break;
    }
    cout << "Ordenando fichero >" << t[2] << endl;
    esperar(100, 1000);
    cout << "Fichero ordenado >" << t[2] << endl;
    t_hecho[1] = t[2];
    ld.PN(t_hecho);
  }
}
//-----------------------------------------------------
void t_supervisor(char* ip, int port, int i, char* param){
  LindaDriver ld(ip, port);
	
	int M = atoi(param);
	
  Tupla t_hacer("hacer", "ordenar", "");
  for (int i = 0; i < M; ++i) {
    t_hacer[2] = "fichero_" + to_string(i);
    ld.PN(t_hacer);
  }

  Tupla init("init");
  ld.PN(init);

  Tupla t_hecho("hecho", "");
  for (int i = 0; i < M; ++i) {
    t_hecho[1] = "fichero_" + to_string(i);
    ld.RN(t_hecho);
  }

  t_hacer[1] = "salir";
  t_hacer[2] = "";
  for (int i = 0; i < M; ++i) {
    ld.PN(t_hacer);
  }
}

//---------------------------

ADDFUNCTION(t_worker,"test trabajador-supervisor, 'N t_worker .' siendo N el número de trabajadores");
ADDFUNCTION(t_supervisor,"test trabajador-supervisor, '1 t_supervisor M' siendo M el número de ficheros a ordenar");

//---------------------------
//--------Ejemplo profesores
//---------------------------

void example(char* ip, int port, int i, char* param){
	// un driver con los datos pasados en la invocación
  LindaDriver LD(ip, port);
  // La conexión con el servidor Linda ya está establecido

  // Varias formas de construir tuplas
  Tupla t1("1", "mi casa", "árbol");  // 3 elementos
  Tupla t2("1000");
  Tupla t3("aprieta", "el", "pan", "45", "34", "88");
  Tupla t4("aprieta", "fuerte", "pan", "tt", "34", "pan");
  // insertar las tuplas en linda: varios PostNote
  LD.PN(t1);
  LD.PN(t2);
  LD.PN(t3);
  LD.PN(t3);
  LD.PN(t3);
  LD.PN(t4);

  // muestra "mi casa" por stdout
  cout << t1.get(2) << endl;
  // t3.to_string() devuelve el string "[aprieta,el,pan,45,34,88]"
  string serial = t3.to_string();
  cout << serial << endl;
  // las componentes de t3 tomarán, respectivamente,
  // los valores "a","b","c","45","34","pan"
  t3.from_string("[a,b,c,45,34,pan]");
  // mostrará [a,b,c,45,34,pan] por stdout
  cout << t3.to_string() << endl;
  LD.PN(t3);  // fix

  // Crea una tupla de 3 elementos "".
  // Equivalente a Tupla t5("","","")
  Tupla t5(3);
  t5.set(2, "hola");
  t5.set(3, "Mundo");
  LD.PN(t5);
  // mostrará [,hola,Mundo] por stdout
  cout << t5.to_string() << endl;
  // informará de que tiene 3 elementos
  cout << "t5 tiene " << t5.size() << " elementos" << endl;
  //...

  // Un patrón no es otra cosa que una tupla con
  // la posibilidad de contener el comodín "?" en una o más posiciones
  // "?" no puede ser un valor de componente de tupla. Para ello se
  // tendría que representar mediante "??". Creamos dos patrones
  Tupla p1("?X");
  Tupla p2("a", "?X", "c", "?Y", "34", "?Z");
  // Dos nuevas tuplas, de tamaño 1 y 6, respectivamente
  Tupla res1(1), res2(p2.size());
  // ejemplos de RemoveNote
  res1 = LD.RN(p1);  // res1 tomará él valor que tenía t2
  res2 = LD.RN(p2);  // res2 tomará el valor que tenía t3
  cout << res1.to_string() << endl;  // mostrará [1000]
  cout << res2.to_string() << endl;  // mostrará [a,b,c,45,34,pan]
  //...

  // ¿Si necesitamos un array de tuplas?
  // Tupla v[2]; // NO permitido: no hay constructor por defecto
  Tupla* v[2];
  v[0] = new Tupla("Juan", "1000");
  v[1] = new Tupla("Luisa", "1000", "enero");
  //...
  delete v[0];
  delete v[1];
	
}
//---------------------------

ADDFUNCTION(example,"Ejemplo profesores, lanzar como '1 example .'");

//---------------------------
//----Ejemplo sencillo
//---------------------------

void myexample(char* ip, int port, int i, char* param){
  LindaDriver LD(ip, port);
	
	//Mi propio test sencillo
	
	Tupla mt1("1");
	Tupla mt2("1","2");
	Tupla mt3("1","2","3");
	Tupla mt4("1","2","3","4");
	Tupla mt5("1","2","3","4","5");
	Tupla mt6("1","2","3","4","5","6");
	
	LD.PN(mt1);
	LD.PN(mt2);
	LD.PN(mt3);
	LD.PN(mt4);
	LD.PN(mt5);
	LD.PN(mt6);
	
	cout << "PN correctos" << endl;
	
	mt1.from_string("[?]");
	mt2.from_string("[?,?]");
	mt3.from_string("[?X,?,3]");
	mt4.from_string("[?,?,?,?]");
	mt5.from_string("[?,?,?,?,?]");
	mt6.from_string("[?,?,?,4,?,?]");
	
	
	mt1 = LD.readN(mt1);
	mt2 = LD.readN(mt2);
	mt3 = LD.readN(mt3);
	mt4 = LD.readN(mt4);
	mt5 = LD.readN(mt5);
	mt6 = LD.readN(mt6);
	
	cout << "readN correctos" << endl;
	
	mt1 = LD.RN(mt1);
	mt2 = LD.RN(mt2);
	mt3 = LD.RN(mt3);
	mt4 = LD.RN(mt4);
	mt5 = LD.RN(mt5);
	mt6 = LD.RN(mt6);
	
	cout << "RN correctos" << endl;
	
	cout << mt1 << mt2 << mt3 << mt4 << mt5 << mt6 << endl;
}

//---------------------------

ADDFUNCTION( myexample, "test sencillo, hace PN y RN de tuplas con tamaños 1 a 6, lanzar como '1 myexample .'")

	
//-----------------------------------------------------
//---------Filósofos
//-----------------------------------------------------
void t_filosofo(char* ip, int port, int i, char* param){
  LindaDriver scb(ip, port);
	
  int repetir = atoi(param);
  
  if(i == 0){
	Tupla info("repetir", to_string(repetir));
	scb.PN(info);
  }
  
  Tupla init("init");
  scb.readN(init);

  Tupla coger("filosofo", "coger", to_string(i));
  Tupla comer("filosofo", "comer", to_string(i));
  Tupla terminar("filosofo", "terminar", to_string(i));

  for (int t=0;t<repetir;++t) {
    scb.PN(coger);
    scb.RN(comer);
    cout << "\tComiendo filósofo " << i << endl;
    esperar(100, 1000);
    scb.PN(terminar);
    cout << "Pensando filósofo " << i << endl;
  }
  
}
//-----------------------------------------------------
void t_dejar(char* ip, int port, int _, char* param){
  LindaDriver scb(ip, port);
	
  int max = atoi(param);

  Tupla info("repetir", "?B");
  Tupla tenedor("filosofo", "tenedor", "");
  Tupla terminar("filosofo", "terminar", "?B");

  for (int i = 0; i < max; ++i) {
    tenedor[2] = to_string(i);
    scb.PN(tenedor);
  }
  
  info = scb.readN(info);
  int repetir = stoi(info[1]);
  
  Tupla init("init");
  scb.PN(init);

  for(int i = 0; i < repetir * max; ++i){
	Tupla querer = scb.RN(terminar);
	int n = stoi(querer[2]);
	tenedor[2] = querer[2];
	scb.PN(tenedor);
	(n % 2 == 0 ? tenedor[2] = to_string((n + 1)% max) : tenedor[2] = to_string((n - 1)% max));
	scb.PN(tenedor);
  }
  //finalizar
  scb.RN(info);
  
  for (int i = 0; i < max; ++i) {
    tenedor[2] = to_string(i);
    scb.RN(tenedor);
  }
}

//-----------------------------------------------------
void t_coger(char* ip, int port, int _, char* param){
  LindaDriver scb(ip, port);
	
  int max = atoi(param);

  Tupla info("repetir", "?B");
  Tupla coger("filosofo", "coger", "?B");
  Tupla tenedor("filosofo", "tenedor", "");
  Tupla comer("filosofo", "comer", "");

  info = scb.readN(info);
  int repetir = stoi(info[1]);
  
  Tupla init("init");
  scb.readN(init);

  for(int i = 0; i < repetir * max; ++i){
	Tupla querer = scb.RN(coger);
	int n = stoi(querer[2]);
	tenedor[2] = querer[2];
	scb.RN(tenedor);
	(n % 2 == 0 ? tenedor[2] = to_string((n + 1)% max) : tenedor[2] = to_string((n - 1)% max));
	scb.RN(tenedor);
	comer[2] = querer[2];
	scb.PN(comer);
  }

}

//--------------------------------------------------

ADDFUNCTION( t_filosofo, "test filósofos, ejecutar como 'N t_filosofo M' siendo N el número de filósofos y M las veces que come");
ADDFUNCTION( t_coger, "test filósofos, ejecutar como '1 t_coger N' siendo N el número de filósofos");
ADDFUNCTION( t_dejar, "test filósofos, ejecutar como '1 t_dejar N' siendo N el número de filósofos");

	
//-----------------------------------------------------
//---------Control Gasolinera
//-----------------------------------------------------
void t_coche(char* ip, int port, int i, char* param){
  LindaDriver scb(ip, port);
	
  int repetir = atoi(param);

  Tupla coche("coche", to_string(i));
  Tupla coger("surtidor", "coger" , "?B");
  Tupla gasolinera("gasolinera", "?S");
  Tupla mantenimiento("mantenimiento");
  Tupla surtidorf("surtidor", "dejo", "");
  
  if(i == 0){
	Tupla info("repetir", to_string(repetir));
	scb.PN(info);
  }
  
  scb.readN(gasolinera);
  
  for (int t = 0; t < repetir; ++t){
	scb.readN(mantenimiento);
	scb.PN(coche);
	Tupla querer = scb.RN(coger);
	cout << "Coche " << i << " en el surtidor " << querer[2] << endl;
	esperar(100, 1000);	//repostando
	surtidorf[2] = querer[2];
	scb.PN(surtidorf);
	cout << "\tCoche " << i << " deja surtidor " << querer[2] << endl;
	esperar(100, 1000);	//conduciendo
  }
  
}

//-----------------------------------------------------
void t_mantenimiento(char* ip, int port, int _, char* param){
  LindaDriver scb(ip, port);
	
  int repetir = atoi(param);

  Tupla gasolinera("gasolinera", "?S");
  Tupla mantenimiento("mantenimiento");
  Tupla fin("fin");

  scb.PN(mantenimiento);
  Tupla querer = scb.readN(gasolinera);
  int max = stoi(querer[1]);
  
  for(int i = 0; i < repetir; ++i){
	esperar(4000, 6000);
	scb.RN(mantenimiento);
	cout << "------>>>> LLAMADA MANTENIMIENTO <<<<------" << endl;
	querer = scb.readN(gasolinera);
	int n = stoi(querer[1]);
	while(n != max){
	  querer = scb.readN(gasolinera);
	  n = stoi(querer[1]);
	}
	cout << "------>>>> EMPIEZA MANTENIMIENTO <<<<------" << endl;
	esperar(1000, 2000);
	cout << "------>>>>   FIN MANTENIMIENTO   <<<<------" << endl;
	scb.PN(mantenimiento);
  }

  scb.RN(fin);
  fin.from_string("[OK]");
  scb.PN(fin);
  scb.RN(mantenimiento);
}

//-----------------------------------------------------
void t_gasolinera(char* ip, int port, int _, char* param){
  LindaDriver scb(ip, port);
	
  int max, numCoche;
  sscanf (param,"%d %*c %d",&max,&numCoche);

  Tupla coche("coche", "?B");
  Tupla gasolinera("gasolinera", to_string(max));
  Tupla surtidor("surtidor", "entro", "");
  Tupla coger("surtidor", "coger", "");
  Tupla mantenimiento("mantenimiento");
  Tupla info("repetir", "?S");
  
  for(int i = 0; i < max; ++i){
	surtidor[2] = to_string(i);
	scb.PN(surtidor);
  }
  
  info = scb.readN(info);
  int repetir = stoi(info[1]);
  
  surtidor[2] = "?B";
  scb.PN(gasolinera);
  gasolinera[1] = "?B";
  
  for(int i = 0; i < repetir * numCoche; ++i){
	scb.readN(mantenimiento);
	Tupla querer = scb.RN(coche);
	querer = scb.readN(gasolinera);
	int n = stoi(querer[1]);
	while(n == 0){
	  querer = scb.readN(gasolinera);
	  n = stoi(querer[1]);
	  scb.readN(mantenimiento);
	}
	scb.RN(gasolinera);
	querer[1] = to_string(n - 1);
	scb.PN(querer);
	querer = scb.readN(surtidor);
	coger[2] = querer[2];
	scb.RN(querer);
	scb.PN(coger);
  }
  
}

//-----------------------------------------------------
void t_surtidor(char* ip, int port, int _, char* param){
  LindaDriver scb(ip, port);

  int numCoche = atoi(param);

  Tupla gasolinera("gasolinera", "?B");
  Tupla surtidor("surtidor", "entro", "");
  Tupla surtidorf("surtidor", "dejo", "?B");
  Tupla info("repetir", "?S");
  Tupla fin("fin");
 
  info = scb.readN(info);
  int repetir = stoi(info[1]);

  Tupla querer = scb.readN(gasolinera);
  int max = stoi(querer[1]);
  
  for(int i = 0; i < repetir * numCoche; ++i){
	querer = scb.RN(surtidorf);
	surtidor[2] = querer[2]; 
	querer = scb.RN(gasolinera);
	int n = stoi(querer[1]);
	querer[1] = to_string(n + 1);
	scb.PN(querer);
	scb.PN(surtidor);
  }

  //esperar fin mantenimiento
  scb.PN(fin);
  fin.from_string("[OK]");
  scb.RN(fin);

  //finalizar
  scb.RN(info);
  
  for(int i = 0; i < max; ++i){
	surtidor[2] = to_string(i);
	scb.RN(surtidor);
  }
  
  scb.RN(gasolinera);
}

//--------------------------------------------------

ADDFUNCTION( t_coche, "Control Gasolinera, ejecutar como 'N t_coche M' siendo N el número de coches y M las veces que pasa por la gasolinera");
ADDFUNCTION( t_mantenimiento, "Control Gasolinera, ejecutar como '1 t_mantenimiento N' siendo N el número de mantenimientos");
ADDFUNCTION( t_gasolinera, "Control Gasolinera, ejecutar como '1 t_gasolinera N' siendo N el número de surtidores totales y el de coches con este formato 'surtidores:coches'");
ADDFUNCTION( t_surtidor, "Control Gasolinera, ejecutar como '1 t_surtidor N' siendo N el número de coches");


	
//-----------------------------------------------------
//---------Tiempo tuplas
//-----------------------------------------------------
//Devuelve el tiempo que se tarda en una de las operaciones de LindaDriver 
//con <max> tuplas <a>
float time_LD(LindaDriver& scb, const int max, const Tupla& a, const int select){
	
  clock_t t = clock();
  for(int j = 0; j < max; ++j){
  	switch(select){
  		case 1:
			scb.PN(a);
		break;
		case -1:
			scb.RN(a); 	
		break;
		case 0:
			scb.readN(a);
		break;
  	}
  }
  
  t = clock() - t;
  return (float(t))/CLOCKS_PER_SEC;

}

//--------------------------------------------------
void t_tuplas(char* ip, int port, int _, char* param){
  LindaDriver scb(ip, port);
  
  int max = atoi(param);
  
  for(int i=1;i<=6;i++){
	  cout << endl;
	  cout << "Tamaño de la tupla " << i << endl;
	  
	  Tupla a("tupla");
	  
	  if(i == 2){
		Tupla a("tupla", "tupla");
	  } else if(i == 3){
		Tupla a("tupla", "tupla", "tupla");
	  } else if(i == 4){
		Tupla a("tupla", "tupla", "tupla", "tupla");
	  } else if(i == 5){
		Tupla a("tupla", "tupla", "tupla", "tupla", "tupla");
	  } else if(i == 6){
		Tupla a("tupla", "tupla", "tupla", "tupla", "tupla", "tupla");
	  }
	  
	  float aux, aux2, aux3;
	  
	  cout << "Metiendo " << max << " tuplas..." << endl;
	  
	  aux = time_LD(scb, max, a, 1);
	  cout << "Incluidas " << max << " tuplas en " << fixed << aux << " segundos" << endl;
	  
	  cout << "Leyendo " << max << " tuplas..." << endl;

	  aux2 = time_LD(scb, max, a, 0);
	  cout << "Leídas " << max << " tuplas en " << fixed << aux2 << " segundos" << endl;
	  aux += aux2;
	  
	  cout << "Eliminando " << max << " tuplas..." << endl;

	  aux2 = time_LD(scb, max, a, -1);
	  cout << "Eliminadas " << max << " tuplas en " << fixed << aux2 << " segundos" << endl;
	  aux += aux2;
	  
	  cout << "Tiempo total parte 1: " << fixed << aux << " segundos" << endl << endl;
	  
	  cout << "Ahora las tuplas serán creadas de la forma from_string" << endl << endl;
	  
	  if(i == 1) {
		a.from_string("[prove]");
	  } else if(i == 2){
		a.from_string("[prove,prove]");
	  } else if(i == 3){
		a.from_string("[prove,prove,prove]");
	  } else if(i == 4){
		a.from_string("[prove,prove,prove,prove]");
	  } else if(i == 5){
		a.from_string("[prove,prove,prove,prove,prove]");
	  } else{
		a.from_string("[prove,prove,prove,prove,prove,prove]");
	  }
	  
	  cout << "Metiendo " << max << " tuplas..." << endl;
	  
	  aux2 = time_LD(scb, max, a, 1);
	  cout << "Incluidas " << max << " tuplas en " << fixed << aux2 << " segundos" << endl;
	  
	  cout << "Leyendo " << max << " tuplas..." << endl;

	  aux3 = time_LD(scb, max, a, 0);
	  cout << "Leídas " << max << " tuplas en " << fixed << aux3 << " segundos" << endl;
	  aux2 += aux3;
	  
	  cout << "Eliminando " << max << " tuplas..." << endl;

	  aux3 = time_LD(scb, max, a, -1);
	  cout << "Eliminadas " << max << " tuplas en " << fixed << aux3 << " segundos" << endl;
	  
	  cout << "Tiempo total parte 2: " << fixed << aux2 + aux3 << " segundos" << endl << endl;
	  cout << "Tiempo total entre las dos partes: " << fixed << aux + aux2 + aux3 << " segundos" << endl;
  }
}

//--------------------------------------------------

ADDFUNCTION( t_tuplas, "Tiempo tuplas, ejecutar como '1 t_tuplas N' siendo N el número de tuplas");

void detenedor(char* ip, int port, int _, char* param){
	
	
	Socket soc_serv(ip,port);

  //Connect
  int soc_serv_fd = soc_serv.Connect();
  if (soc_serv_fd == -1) {
    string mensError(strerror(errno));
      cerr << "[x]Error en el connect: " + mensError + "\n";
    exit(1);
  }

  soc_serv.Send(soc_serv_fd, "END");
	soc_serv.Close(soc_serv_fd);
}

ADDFUNCTION( detenedor, "Detiene el servidor en activo");
