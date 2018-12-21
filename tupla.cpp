
#include "tupla.hpp"

#include <cassert>
#include <sstream>

//-----------------------------------------------------
//------------ CONSTRUCTORS / DESTRUCTORS -------------
//-----------------------------------------------------
Tupla::Tupla(int i){
	assert(i>= 1 && i<=TUPLA_MAX);
	length = i;
}
//-----------------------------------------------------
Tupla::Tupla(string e1){
	length = 1;
	elements[0] = e1;
}
//-----------------------------------------------------
Tupla::Tupla(string e1,string e2){
	length = 2;
	elements[0] = e1;
	elements[1] = e2;
}
//-----------------------------------------------------
Tupla::Tupla(string e1,string e2,string e3){
	length = 3;
	elements[0] = e1;
	elements[1] = e2;
	elements[2] = e3;
}
//-----------------------------------------------------
Tupla::Tupla(string e1,string e2,string e3,string e4){
	length = 4;
	elements[0] = e1;
	elements[1] = e2;
	elements[2] = e3;
	elements[3] = e4;
}
//-----------------------------------------------------
Tupla::Tupla(string e1,string e2,string e3,string e4,string e5){
	length = 5;
	elements[0] = e1;
	elements[1] = e2;
	elements[2] = e3;
	elements[3] = e4;
	elements[4] = e5;
}
//-----------------------------------------------------
Tupla::Tupla(string e1,string e2,string e3,string e4,string e5,string e6){
	length = 6;
	elements[0] = e1;
	elements[1] = e2;
	elements[2] = e3;
	elements[3] = e4;
	elements[4] = e5;
	elements[5] = e6;
}
//-----------------------------------------------------
Tupla::Tupla(const Tupla& copy){
	length = copy.length;
	for(int i=0; i<length; ++i){
		elements[i] = copy.elements[i];
	}
}
//-----------------------------------------------------
Tupla::~Tupla() {
}
//-----------------------------------------------------
//-------------------- GETTERS ------------------------
//-----------------------------------------------------
string Tupla::get(int index) const{
	assert(index >=1 && index <= length);
	return elements[index-1];
}
//-----------------------------------------------------
int Tupla::size() const{
	return length;
}
//-----------------------------------------------------
string Tupla::to_string() const{
	stringstream ss;
	ss << "[" << this->get(1);
	for(int i=2; i<= length; ++i){
		ss << "," << this->get(i);
	}
	ss << "]";
	return ss.str();
}
//-----------------------------------------------------
//------------------- SETTERS -------------------------
//-----------------------------------------------------
bool Tupla::from_string(string s){
	if(s[0] != '[' || s[s.length()-1] != ']'){
		//not separated
		return false;
	}
	stringstream data(s.substr(1, s.length() - 2));

	//assign each element
	int i = 0;
	while(i<TUPLA_MAX && getline(data,elements[i],',')){
		i++;
	}
	
	length = i;
	return true;
}
//-----------------------------------------------------
void Tupla::set(int i, string s){
	assert(i >=1 && i <= length);
	elements[i-1] = s;
}
//-----------------------------------------------------
//-----------------------OVERLOAD----------------------
//-----------------------------------------------------
string& Tupla::operator[](int index){
	assert(index >=0 && index < length);
	return elements[index];
}
//-----------------------------------------------------
const string& Tupla::operator[](int index) const{
	assert(index >=0 && index < length);
	return elements[index];
}
//-----------------------------------------------------
Tupla & Tupla::operator=(const Tupla &rhs){
	if(this != &rhs){
		//not same object, copy
		length = rhs.length;
		for(int i=0; i<length; ++i){
			elements[i] = rhs.elements[i];
		}
	}
	return *this;
}
//-----------------------------------------------------
ostream& operator<<(ostream& os, const Tupla& tuple){
  os << tuple.to_string();
	return os;
}
