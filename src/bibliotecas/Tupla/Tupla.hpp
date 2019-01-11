//*****************************************************************
// File:  Tupla.hpp
// Authors:   GONZÁLEZ VILLA, DANIEL
//            NAYA FORCANO, ABEL
//            GONZÁLEZ GORRADO, JESÚS ÁNGEL
//            GARCÍA DÍAZ, ÁLVARO
// Date:   Diciembre 2018-Enero 2019
//*****************************************************************
#ifndef TUPLA_H
#define TUPLA_H

#include <iostream>
#include <string>
#include <condition_variable>
#include <list>

using namespace std;

#define TUPLA_MAX 6  // hardcoded constructors

/**
Representa una colección de n string, con n>= 1 && n<=6.
*/
class Tupla {
 public:
  //----------- CONSTRUCTORES / DESTRUCTORES ------------
  /**
  Crea una tupla de {i} elementos vacíos ("")

  Requisito: i>=1 && i<=6
  */
  Tupla(int i);

  /**
  Crea una tupla con los elementos dados (1-6)
  */
  Tupla(string e1);
  Tupla(string e1, string e2);
  Tupla(string e1, string e2, string e3);
  Tupla(string e1, string e2, string e3, string e4);
  Tupla(string e1, string e2, string e3, string e4, string e5);
  Tupla(string e1, string e2, string e3, string e4, string e5, string e6);

  /** [Clone]
  Crea una copia independiente de la tupla.
  */
  Tupla(const Tupla& copy);

  /**
  Destructor
  */
  ~Tupla();

  //-------------------- GETTERS ------------------------

  /**
  Devuelve el elemento {index} de la tupla.
  El índice empieza en 1 (.get(1) devuelve el primero, .get(0) da error)
  */
  string get(int index) const;

  /**
  Devuelve el número de elementos de la tupla. (1-6)
  */
  int size() const;

  /**
  Devuelve una string con los elementos de la tupla en formato:
  "[e1]" o "[e1,e2]" o equivalente
  */
  string to_string() const;

  //------------------- SETTERS -------------------------

  /**
  Si {s} es una string con una tupla válida devuelta por #to_string():
          - Sustituye e contenido de la tupla con los valores de la tupla que
  generó el string
          - devuelve true
  Si {s} no es una string coon una tupla válida:
          - devuelve false
  */
  bool from_string(string s);

  /**
  Sustituye el elemento {i} de la tupla por {s}.
  El índice empieza en 1 (.set(1,s) sustituye el primero, .set(0,s) da error)
  */
  void set(int i, string s);

  //-----------------------OVERLOAD----------------------

  /** [[] operator]
  El índice empieza en 0 ([0] devuelve el primero)
  */
  string& operator[](int index);
  const string& operator[](int index) const;

  /** [= operator]
  Crea una copia independiente de la tupla.
  */
  Tupla& operator=(const Tupla& rhs);

 private:
  // número de elementos
  int length;

  // elementos
  string elements[TUPLA_MAX];
};

/** [<< operator]
Imprime una string con los elementos de la tupla en formato:
"[e1]" o "[e1,e2]" o equivalente
*/
ostream& operator<<(ostream& os, const Tupla& tuple);

#endif
