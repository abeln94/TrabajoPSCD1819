
#include <iostream>
#include <thread>
#include <string>
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
int main(int argc, char* argv[]) {

  /*
  Scoreboard scb;
  Tupla it("1");
  Tupla pat("?X");
  Tupla it2(1);
  scb.PN(it);

  for(int i=0;i<10;++i){
          it2 = scb.readN(pat);
          cout << it2 << endl;
  }

  scb.RN(pat);

  scb.RN(pat);


// iniciar threads
thread a[10];
  for(int i=0; i<10; ++i){
          a[i] = thread(&t_filosofo, ref(scb), to_string(i));
  }

// esperar a que terminen
  for(int i=0; i<10; ++i){
          a[i].join();
  }

  */

  // suyo
  Scoreboard LD;

  // un driver con los datos pasados en la invocación
  // LindaDriver LD(argv[1], argv[2], ...);
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

  // mío
  // ejercicio de ordenacion de ficheros
  Scoreboard scb;

  int N = 10;
  thread w[N];
  thread s;

  for (int i = 0; i < N; ++i) {
    w[i] = thread(&t_worker, ref(scb));
  }
  s = thread(&t_supervisor, ref(scb), 100);

  for (int i = 0; i < N; ++i) {
    w[i].join();
  }
  s.join();

  return 0;
}
