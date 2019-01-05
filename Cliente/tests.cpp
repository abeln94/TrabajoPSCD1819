/*
** Fichero de tests, para usar con clientAbel.cpp **

** Uso:
	Para cada test, añadir una o más funciones con parámetros (char* ip, char* port, int i, char* param) siendo
		ip, port la ip:puerto del servidor (usar como LindaDriver(ip,port);)
		i es el identificador de la instancia actual (contador empezando en 0)
		param es el parámetro lanzado por línea de comandos
	
	Después añadir la macro ADDTEST(nombre,descripción) siendo
		name el nombre de la función anterior (añadir tantas macros como funciones)
		descripción un string que se imprime al definir el test

** Ejemplo:
	Si el fichero tiene el siguiente contenido:

		void myTestFunction(char* ip, int port, int i, char* param){
			LindaDriver ld(ip, port);
			
			cout << "I am the instance " << i << " launched with parameter " << param << endl;
			//do things
		}

		void myTestDefinitionFunction(){
			NEXTTEST
			#undef NEXTTEST
			#define NEXTTEST myTestDefinitionFunction();
			
			addTest("test",&myTestFunction);
			//other if necessary
		}
		
		ADDTEST(myTestFunction,"Example, run as 'N myTestFunction M'")

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

  Tupla t_hacer("hacer", "?b", "?s");
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

ADDTEST(t_worker,"test trabajador-supervisor, 'N t_worker .' siendo N el número de trabajadores");
ADDTEST(t_supervisor,"test trabajador-supervisor, '1 t_supervisor M' siendo M el número de ficheros a ordenar");

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

ADDTEST(example,"Ejemplo profesores, lanzar como '1 example .'");

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
	
	mt1.from_string("[?]");
	mt2.from_string("[?,?]");
	mt3.from_string("[?X,?,3]");
	mt4.from_string("[?,?,?,?]");
	mt5.from_string("[?,?,?,?,?]");
	mt6.from_string("[?,?,?,4,?,?]");
	
	mt1 = LD.RN(mt1);
	mt2 = LD.RN(mt2);
	mt3 = LD.RN(mt3);
	mt4 = LD.RN(mt4);
	mt5 = LD.RN(mt5);
	mt6 = LD.RN(mt6);
	
	cout << mt1 << mt2 << mt3 << mt4 << mt5 << mt6 << endl;
}

//---------------------------

ADDTEST( myexample, "test sencillo, hace PN y RN de tuplas con tamaños 1 a 6, lanzar como '1 mytest .'")


	
//-----------------------------------------------------
//---------Filosofos
//-----------------------------------------------------
void t_filosofo(char* ip, int port, int i, char* param){
	LindaDriver scb(ip, port);
	
	int repetir = atoi(param);

  Tupla init("init");
  scb.readN(init);

  Tupla coger("filosofo", "coger", to_string(i));
  Tupla comer("filosofo", "comer", to_string(i));
  Tupla terminar("filosofo", "terminar", to_string(i));

  for (int t=0;t<repetir;++t) {
    scb.PN(coger);
    scb.RN(comer);
    cout << "\tComiendo filosofo " << i << endl;
    esperar(100, 1000);
    scb.PN(terminar);
    cout << "Pensando filosofo " << i << endl;
  }

}
//-----------------------------------------------------
void t_dejar(char* ip, int port, int _, char* param){
	LindaDriver scb(ip, port);
	
	int max = atoi(param);

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
void t_coger(char* ip, int port, int _, char* param){
	LindaDriver scb(ip, port);
	
	int max = atoi(param);

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

//--------------------------------------------------

ADDTEST( t_filosofo, "test filosofos, ejecutar como 'N t_filosofo M' siendo M las veces que come");
ADDTEST( t_coger, "test filosofos, ejecutar como '1 t_coger N'");
ADDTEST( t_dejar, "test filosofos, ejecutar como '1 t_dejar N' siendo N el número de filosofos totales");