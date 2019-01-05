
#include <iostream>
#include <thread>
#include <string>
#include <map>
#include <vector>
#include "LindaDriver.hpp"

using namespace std;

//-----------------------------------------------------

//to save defined functions
typedef void (* TestFunction)(char*,int,int,char*);
map<string,TestFunction> functions;

//this...this is a magic macro. I'm proud of it. (check usage in tests.cpp)
#define ADDTEST(name,description) 															\
class _Class##name{         																		\
	public:                   																		\
	_Class##name(){           																		\
		cout << "<" << #name << "> " << description << endl;				\
		functions.insert( pair<string,TestFunction>(#name,&name) );	\
	}                         																		\
};                          																		\
_Class##name _object##name;  


//-----------------------------------------------------

#include "tests.cpp"

//-----------------------------------------------------
int main(int argc, char* argv[]) {
	
	//check parameters
	if(argc < 6 || argc%3 != 0){
		cout << "Usage: $run ip port n1 name1 param1 n2 name2 param2 ..." << endl;
		cout << "       nX nameX paramX means: run xN threads as function nameX with parameter paramX (all of them)" << endl;
		return 0;
	}
	
	//find number of functions
	int Nfunctions = (argc-3)/3;
	
	//check tests
	for(int i=0; i<Nfunctions;++i){
		string name = argv[3+i*3+1];
		
		if (functions.find(name) == functions.end()){
			cout << "Test " << name << " not defined" << endl;
			return 0;
		}
	}
	
	cout << "---BEGIN TESTS---" << endl;
	
	//run threads
	vector<thread> threads;
	for(int i=0; i<Nfunctions;++i){
		int instances = atoi(argv[3+i*3]);
		string name = argv[3+i*3+1];
		char* param = argv[3+i*3+2];
		
		TestFunction func = functions[name];
		
		cout << "Running " << instances << " threads as function '" << name << "' with parameter '" << param << "'" <<endl;
		for(int j=0;j<instances;++j){
			threads.push_back( thread(func,argv[1],atoi(argv[2]),j,param) );
		}
	}
	
	//wait threads to finish
	for (thread& thread_ : threads){  
		thread_.join();
	}
	
	cout << "---END TESTS---" << endl;
	
  return 0;
}
