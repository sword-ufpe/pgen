/*
 * ICompilable.cpp
 *
 *  Created on: Jun 16, 2014
 *      Author: Dimas Melo Filho
 */

#include "ICompilable.h"
#include <string>
#include <sstream>
using namespace std;

namespace pgen {

vector<ICompilable*> ICompilable::_dependences;

ICompilable::~ICompilable() {}

vector<ICompilable*> & ICompilable::dependences() {
	return ICompilable::_dependences;
}

string ICompilable::funcname() {
	stringstream s;
	s << "fn_" << this->name();
	return s.str();
}

string ICompilable::cfuncname() {
	stringstream s;
	s << "cf_" << this->name();
	return s.str();
}

string ICompilable::prototype() {
	stringstream s;
	s << "int " << this->funcname() << "(char* text)";
	return s.str();
}

/**
 * The current chained prototype is int ch_function(char * text, int(*chain[])()), meaning
 * that it is a function that receives a char pointer and an array of function pointers as
 * parameters. The array of function pointers should contain the chain of functions to be
 * called, ending with a NULL function pointer.
 */
string ICompilable::cprototype() {
	stringstream s;
	s << "int " << this->cfuncname() << "(char* text, chainptr *chain)";
	return s.str();
}

} /* namespace pagen */
