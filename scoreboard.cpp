
#include "scoreboard.hpp"

#include <cassert>
#include <map> 

//-----------------------------------------------------
//------------- CONSTRUCTOR / DESTRUCTOR --------------
//-----------------------------------------------------
Scoreboard::Scoreboard() {}
//-----------------------------------------------------
Scoreboard::~Scoreboard() {}
//-----------------------------------------------------
//------------------- FUNCTIONS -----------------------
//-----------------------------------------------------
void Scoreboard::PN(const Tupla& note) {
  unique_lock<mutex> lck(mtxMonitor);
	
	//check if there are pending that want it
	auto p = pending.begin();
	while(p != pending.end()){
		if(_compare((*p).get().note, note)){
			//the note can be assigned
			
			(*p).get().note = note; //assign note
			
			//notify and remove
			(*p).get().condVar.notify_one();
			bool r = (*p).get().remove;
			pending.erase(p); //remove now because when know where to find it
			
			if(r){
				//note was removed, stop
				return; 
			}
		}
		
		++p;
	}
	
	//note wasn't removed, add a copy to list
	notes.push_back(note);
}
//-----------------------------------------------------
Tupla Scoreboard::RN(const Tupla& note) {
  unique_lock<mutex> lck(mtxMonitor);
	return _rn(note, true, lck);
}
//-----------------------------------------------------
Tupla Scoreboard::readN(const Tupla& note) {
  unique_lock<mutex> lck(mtxMonitor);
	return _rn(note,false, lck);
}
//-----------------------------------------------------
//------------------INTERNAL---------------------------
//-----------------------------------------------------
Tupla Scoreboard::_rn(const Tupla& note, bool remove, unique_lock<mutex>& lck){
	
	//check each note
	auto p = notes.begin();
	while(p != notes.end()){
		
		if( _compare(note, *p) ){
			//note can be assigned
			Tupla ret(*p);
			
			if(remove){
				// note should be removed from list
				notes.erase(p);
			}
			return ret;
		}
		
		++p;
	}
	
	//nothing found, wait for it
	Tupla ret(note); //this will be modified with the matching note
	Scoreboard::PendingStruct pend(ret, remove);
	pending.push_back(pend);
	pend.condVar.wait(lck);
	//no need to remove from pending, already removed
	return ret;
}
//-----------------------------------------------------
bool Scoreboard::_compare(const Tupla& dst, const Tupla& src) {
	if (dst.size() != src.size()){
		//different sizes, different
		return false;
	}
	
	map<string,string> vars;
	
  for(int i=0;i<src.size();++i){
		//cehck each element
		
		if(dst[i] == "" || dst[i] == "?"){
			//unnamed variable, nothing to check
		}else	if(dst[i][0] == '?'){
			//named variable, check
			string name = dst[i].substr(1, dst[i].size() - 1);
			
			//insert into map
			auto iterInsert = vars.insert(pair<string,string>(name, src[i]));
			if (!iterInsert.second && (iterInsert.first->second != src[i])) {
				//if not inserted and existing value different from new, invalid comparison
				return false;
			}
			
	  }else{
			//not a variable, check exact value
			if(dst[i] != src[i]){
				//different value, invalid comparison
				return false;
			}
		}
	}
	
	//valid comparison
	return true;
}
//-----------------------------------------------------
Scoreboard::PendingStruct::PendingStruct(Tupla& t, bool remove)
: note(t), remove(remove){};
