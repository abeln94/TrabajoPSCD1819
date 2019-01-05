
#include "tupla.hpp"

#include <cassert>
#include <sstream>

//-----------------------------------------------------
//------------ CONSTRUCTORS / DESTRUCTORS -------------
//-----------------------------------------------------
Tupla::Tupla(int i) {
	// empty constructor
	
  assert(i >= 1 && i <= TUPLA_MAX);
  length = i;
}
//-----------------------------------------------------
Tupla::Tupla(string e1) {
	// direct constructor
	
  length = 1;
  elements[0] = e1;
}
//-----------------------------------------------------
Tupla::Tupla(string e1, string e2) {
	// direct constructor
	
  length = 2;
  elements[0] = e1;
  elements[1] = e2;
}
//-----------------------------------------------------
Tupla::Tupla(string e1, string e2, string e3) {
	// direct constructor
	
  length = 3;
  elements[0] = e1;
  elements[1] = e2;
  elements[2] = e3;
}
//-----------------------------------------------------
Tupla::Tupla(string e1, string e2, string e3, string e4) {
	// direct constructor
	
  length = 4;
  elements[0] = e1;
  elements[1] = e2;
  elements[2] = e3;
  elements[3] = e4;
}
//-----------------------------------------------------
Tupla::Tupla(string e1, string e2, string e3, string e4, string e5) {
	// direct constructor
	
  length = 5;
  elements[0] = e1;
  elements[1] = e2;
  elements[2] = e3;
  elements[3] = e4;
  elements[4] = e5;
}
//-----------------------------------------------------
Tupla::Tupla(string e1, string e2, string e3, string e4, string e5, string e6) {
	// direct constructor
	
  length = 6;
  elements[0] = e1;
  elements[1] = e2;
  elements[2] = e3;
  elements[3] = e4;
  elements[4] = e5;
  elements[5] = e6;
}
//-----------------------------------------------------
Tupla::Tupla(const Tupla& copy) {
	// clone constructor
	
  length = copy.length;
  for (int i = 0; i < length; ++i) {
		// for each element, copy
    elements[i] = copy.elements[i];
  }
}
//-----------------------------------------------------
Tupla::~Tupla() {}
//-----------------------------------------------------
//-------------------- GETTERS ------------------------
//-----------------------------------------------------
string Tupla::get(int index) const {
	// get element (index 1-based)
	
  assert(index >= 1 && index <= length);
  return elements[index - 1];
}
//-----------------------------------------------------
int Tupla::size() const { 
	// length of tuple
	
	return length; 
}
//-----------------------------------------------------
string Tupla::to_string() const {
	// stringify tuple
	
  stringstream ss;
  ss << "[" << this->get(1);
  for (int i = 2; i <= length; ++i) {
		// for each extra element, add
    ss << "," << this->get(i);
  }
  ss << "]";
  return ss.str();
}
//-----------------------------------------------------
//------------------- SETTERS -------------------------
//-----------------------------------------------------
bool Tupla::from_string(string s) {
	// parse the stringified tuple
	
  if (s[0] != '[' || s[s.length() - 1] != ']') {
    // not "[...]", invalid string
    return false;
  }
	
	string inside = s.substr(1, s.length() - 2);
	
	if(inside.length() == 0){
		//if string is empty it is 1 empty element
		elements[0] = "";
		length = 1;
		return true;
	}
	
  stringstream data(inside);

  // assign each element
  int i = 0;
  while (i < TUPLA_MAX && getline(data, elements[i], ',')) {
		// element assigned
    i++;
  }
  length = i;
	
	
	if(inside[inside.length() - 1] == ','){
		//if string ends with the delimiter, add the missing element
		elements[i] = "";
		length++;
	}

  return true;
}
//-----------------------------------------------------
void Tupla::set(int i, string s) {
	// set element (index 1-based)
	
  assert(i >= 1 && i <= length);
  elements[i - 1] = s;
}
//-----------------------------------------------------
//-----------------------OVERLOAD----------------------
//-----------------------------------------------------
string& Tupla::operator[](int index) {
	// get element  (index 0-based)
	
  assert(index >= 0 && index < length);
  return elements[index];
}
//-----------------------------------------------------
const string& Tupla::operator[](int index) const {
	// get element  (index 0-based)
	
  assert(index >= 0 && index < length);
  return elements[index];
}
//-----------------------------------------------------
Tupla& Tupla::operator=(const Tupla& rhs) {
	// copy the rhs tuple into this
	
  if (this != &rhs) {
    // not same object, continue
    length = rhs.length;
    for (int i = 0; i < length; ++i) {
			// for each element, copy
      elements[i] = rhs.elements[i];
    }
  }
  return *this;
}
//-----------------------------------------------------
ostream& operator<<(ostream& os, const Tupla& tuple) {
	// output as string
	
  os << tuple.to_string();
  return os;
}
