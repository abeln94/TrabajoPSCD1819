#ifndef SCOREBOARD_H
#define SCOREBOARD_H

#include <iostream>
#include <string>
#include <mutex>
#include <condition_variable>
#include <list>

#include "tupla.hpp"

using namespace std;

class Scoreboard {
	
 public:
  //----------------- Constructor
  Scoreboard();

  //----------------- Destructor
  ~Scoreboard();

  void PN(Tupla& note);

  void RN(Tupla& note);
	
  void readN(Tupla& note);

 private:
 
  class PendingStruct{
		public:
			Tupla& note;
			bool remove;
			condition_variable condVar;
			
			PendingStruct(Tupla& t, bool remove);
	};
 
  //functions
	void _rn(Tupla& note, bool remove, unique_lock<mutex>& lck);
  bool _assign(Tupla& dst, const Tupla& src);
 
  // ----- scoreboard -----
  list<Tupla> notes;
	list<reference_wrapper<PendingStruct>> pending;

  // ----- monitor -----
  mutex mtxMonitor;
	
};
#endif
