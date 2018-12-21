
#include "scoreboard.hpp"

#include <cassert>
#include <map> 

//-----------------------------------------------------
Scoreboard::Scoreboard() {}
//-----------------------------------------------------
Scoreboard::~Scoreboard() {}
//-----------------------------------------------------
void Scoreboard::PN(Tupla& note) {
  unique_lock<mutex> lck(mtxMonitor);
	
	//check if there are pending that want it
	auto p = pending.begin();
	while(p != pending.end()){
		if(_assign((*p).get().note, note)){
			//the note was assigned
			bool r = (*p).get().remove;
			
			//notify and remove
			(*p).get().condVar.notify_one();
			pending.erase(p); //remove now because when know where to find it
			
			if(r){
				//note was removed, nothing more to check
				return; 
			}
		}
		
		++p;
	}
	
	//note wasn't removed, add to list
	notes.push_back(note);
}
//-----------------------------------------------------
void Scoreboard::RN(Tupla& note) {
  unique_lock<mutex> lck(mtxMonitor);
	_rn(note, true, lck);
}
//-----------------------------------------------------
void Scoreboard::readN(Tupla& note) {
  unique_lock<mutex> lck(mtxMonitor);
	_rn(note,false, lck);
}
//-----------------------------------------------------

//-----------------------------------------------------
void Scoreboard::_rn(Tupla& note, bool remove, unique_lock<mutex>& lck){
	
	auto p = notes.begin();
	while(p != notes.end()){
		
		if( _assign(note, *p) ){
			//note was assigned
			
			if(remove){
				// note should be removed
				notes.erase(p);
			}
			return;
		}
		
		++p;
	}
	
	//nothing found, add to pending
	Scoreboard::PendingStruct pend(note, remove);
	pending.push_back(pend);
	pend.condVar.wait(lck);
	//no need to remove from pending, already removed
	
}
//-----------------------------------------------------
bool Scoreboard::_assign(Tupla& dst, const Tupla& src) {
	if (dst.length != src.length){
		//different sizes
		return false;
	}
	
	map<string,string> vars;
	
  for(int i=0;i<src.length;++i){
		
		if(dst[i] == "" || dst[i] == "?"){
			//unnamed variable, nothing to check
		}else	if(dst[i][0] == '?'){
			//named variable, check
			string name = dst[i].substr(1, dst[i].size() - 1);
			
			//insert into map
			auto iterInsert = vars.insert(pair<string,string>(name, src[i]));
			if (!iterInsert.second && (iterInsert.first->second != src[i])) {
				//if not inserted and existing value different from new, can't assign
				return false;
			}
			
	  }else{
			//not a variable
			if(dst[i] != src[i]){
				//different, cancel
				return false;
			}
		}
	}
	
	//valid, clone
	for(int i=0;i<src.length;++i){
		dst[i] = src[i];
	}
	return true;
}
//-----------------------------------------------------
Scoreboard::PendingStruct::PendingStruct(Tupla& t, bool remove)
: note(t), remove(remove){};
