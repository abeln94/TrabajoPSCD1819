//*****************************************************************
// File:  memoria.cpp
// Authors:   GONZÁLEZ VILLA, DANIEL
//            NAYA FORCANO, ABEL
//            GONZÁLEZ GORRADO, JESÚS ÁNGEL
//            GARCÍA DÍAZ, ÁLVARO
// Date:   Diciembre 2018-Enero 2019
//*****************************************************************

/*
** Fichero de tests de la memoria técnica (crean ficheros .csv), para usar con
Cliente.cpp **

** Uso:
        Para cada test, añadir una o más funciones con parámetros (char* ip,
char* port, int i, char* param) siendo
                ip, port la ip:puerto del servidor (usar como
LindaDriver(ip,port);)
                i es el identificador de la instancia actual (contador empezando
en 0)
                param es el parámetro lanzado por línea de comandos

        Después añadir la macro ADDFUNCTION(nombre,descripción) siendo
                name el nombre de la función anterior (añadir tantas macros como
funciones)
                descripción un string que se imprime al definir el test

** Ejemplo:
        Si el fichero tiene el siguiente contenido:

                void myTestFunction(char* ip, int port, int i, char* param){
                        LindaDriver ld(ip, port);

                        cout << "I am the instance " << i << " launched with
parameter " << param << endl;
                        //do things
                }

                ADDFUNCTION(myTestFunction,"Example, run as 'N myTestFunction
M'")

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

void t_medio(char* ip, int port, int i, char* param) {
  LindaDriver scb(ip, port);

  Tupla tamano("numero", "?B");
  Tupla info("tupla", "", "", "");  // tupla,PN,readN,RN

  while (true) {

    Tupla querer = scb.RN(tamano);
    if (querer[1] == "terminar") {
      break;
    }
    int max = stoi(querer[1]);

    Tupla a("tupla");

    float tPN, treadN, tRN;

    tPN = time_LD(scb, max, a, 1);
    treadN = time_LD(scb, max, a, 0);
    tRN = time_LD(scb, max, a, -1);

    info[1] = to_string(tPN);
    info[2] = to_string(treadN);
    info[3] = to_string(tRN);
    scb.PN(info);

    cout << "Finalizado para " << max << " tuplas" << endl;
  }
}
//-----------------------------------------------------
void t_cond(char* ip, int port, int _, char* param) {
  LindaDriver scb(ip, port);

  int max = atoi(param);

  ofstream graf;

  graf.open("numTuplas.csv");

  Tupla tamano("numero", "1000");
  Tupla info("tupla", "?X", "?B", "?N");  // tupla,PN,readN,RN

  if (graf.is_open()) {
    graf << "Número de tuplas,PN,readN,RN" << endl;
  }

  for (int i = 0; i < max; ++i) {
    scb.PN(tamano);
    Tupla querer = scb.RN(info);
    graf << tamano[1] << "," << querer[1] << "," << querer[2] << ","
         << querer[3] << endl;
    int n = stoi(tamano[1]);
    tamano[1] = to_string(n + 1000);
  }

  tamano[1] = "terminar";
  scb.PN(tamano);

  cout << "Creado fichero numTuplas.csv" << endl;
}

//--------------------------------------------------

ADDFUNCTION(
    t_medio,
    "Gráfico, ejecutar como 'M t_medio .' siendo M el número de clientes");
ADDFUNCTION(t_cond,
            "Gráfico, ejecutar como '1 t_cond N' siendo N el número de veces");

//--------------------------------------------------
//---Gráfica dependiendo del número de clientes concurrentes
//--------------------------------------------------
void t_conc(char* ip, int port, int _, char* param) {
  LindaDriver scb(ip, port);

  int max, start;
  sscanf(param, "%d %*c %d", &max, &start);

  Tupla tamano("numero", to_string(start));

  int client = 5;
  int clientMax = client * max + 1;
  Tupla cliente("clientes", to_string(client));

  while (client < clientMax) {
    scb.PN(cliente);
    for (int o = 0; o < client; ++o) {
      scb.PN(tamano);
    }
    vector<thread> threads;
    for (int o = 0; o < client; ++o) {
      threads.push_back(thread(t_medio, ip, port, o, param));
    }
    for (thread& thread_ : threads) {
      thread_.join();
    }
    client += 5;
    cliente[1] = to_string(client);
  }

  cliente[1] = "0";
  scb.PN(cliente);
}
//--------------------------------------------------
void t_tamano(char* ip, int port, int _, char* param) {
  LindaDriver scb(ip, port);

  ofstream graf;

  graf.open("concTuplas.csv");

  if (graf.is_open()) {
    graf << "Número de clientes,PN,readN,RN" << endl;
  }

  Tupla tamano("numero", "terminar");
  Tupla cliente("clientes", "?X");
  Tupla info("tupla", "?X", "?B", "?N");  // tupla,PN,readN,RN

  while (true) {
    cliente = scb.RN(cliente);
    int client = stoi(cliente[1]);
    if (client == 0) {
      break;
    }
    cliente[1] = "?B";
    float tPN, treadN, tRN;
    for (int i = 0; i < client; ++i) {
      Tupla querer = scb.RN(info);
      tPN += stof(querer[1]);
      treadN += stof(querer[2]);
      tRN += stof(querer[3]);
    }
    graf << client << "," << tPN / client << "," << treadN / client << ","
         << tRN / client << endl;
    for (int o = 0; o < client; ++o) {
      scb.PN(tamano);
    }
  }

  cout << "Creado fichero concTuplas.csv" << endl;
}

//--------------------------------------------------

ADDFUNCTION(t_conc,
            "Gráfico, ejecutar como '1 t_conc N' siendo N el número de veces y "
            "el número de tuplas siguiendo este formato: 'veces:tuplas'");
ADDFUNCTION(t_tamano, "Gráfico, ejecutar como '1 t_tamano .'");
