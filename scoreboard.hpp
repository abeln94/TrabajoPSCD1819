#ifndef SCOREBOARD_H
#define SCOREBOARD_H

#include <iostream>
#include <string>
#include <mutex>
#include <condition_variable>
#include <list>

#include "tupla.hpp"

using namespace std;

/**
Representa un tablón de notas con interfaz LINDA.
*/
class Scoreboard {
	
 public:
  //------------- CONSTRUCTOR / DESTRUCTOR --------------
  Scoreboard();
  ~Scoreboard();

	//------------------- FUNCIONES -----------------------
	
	/**
	Añade una copia de la nota al tablón.
	*/
  void PN(const Tupla& note);

	/**
	Elimina una nota del tablón que coincide con {nota}.
	Devuelve la nota eliminada.
	Si no existe ninguna nota que coincida, la función se bloquea hasta que haya una.
	
	Para que dos notas coincidan se debe cumplir:
	  - Deben tener el mismo número de elementos
		- Para cada elemento de {note} que no comience con "?" o sea vacío, la nota obtenida contendrá exactamente el mismo elemento.
		- Para todos los elementos de {note} que comiencen con "?" y sean iguales, la nota obtenida contendrá el mismo elemento en todos ellos.
		- Los elementos de {note} que sean vacíos o la string "?" coinciden con cualquier elemento.
		
	Ejemplo: La nota [1,2,3,4,5]:
	Coincide con [1,2,3,4]
	Coincide con [,?,?X,4,?Y]
	No coincide con [?X,2,?,?Y,?X] (el primer y último elemento no son iguales)
	*/
  Tupla RN(const Tupla& note);
	
	/**
	Idéntico a RN pero no elimina la nota del tablón, devuelve una copia.
	*/
  Tupla readN(const Tupla& note);

	//------------------- PRIVADO -----------------------
 private:
 
	//Information when a thread is waiting for a matching note
  class PendingStruct{
		public:
			//note to match
			Tupla& note;
			//remove or not
			bool remove;
			//to awake
			condition_variable condVar;
			
			//constructor
			PendingStruct(Tupla& t, bool remove);
	};
 
  //----functions----
	//Implements RN and readN with remove=true or false respectively
	Tupla _rn(const Tupla& note, bool remove, unique_lock<mutex>& lck);
	
	//returns true if {src} matches {dst}
  bool _compare(const Tupla& dst, const Tupla& src);
 
  // ----- scoreboard -----
	//Notes saved
  list<Tupla> notes;
	
	//waiting threads
	list<reference_wrapper<PendingStruct>> pending;

  // ----- monitor -----
  mutex mtxMonitor;
	
};
#endif
