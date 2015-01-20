/*
 * Alternative.cpp
 *
 *  Created on: Jun 30, 2014
 *      Author: Dimas Melo Filho
 */

#include "Alternative.h"
#include <sstream>
using namespace std;

namespace pagen {

//-------------------------------------------------------------------------------------------
Alternative::Alternative() {
}

//-------------------------------------------------------------------------------------------
Alternative::~Alternative() {
	// Delete all sub-expressions
	while (!expr.empty()) {
		delete expr.back();
		expr.pop_back();
	}
}

//-------------------------------------------------------------------------------------------
void Alternative::add(ICompilable* expression) {
	expr.push_back(expression);
}

//-------------------------------------------------------------------------------------------
/**
 * compiled code: (chained version)
 * %cprototype()% {
 *  fc%name()%[%expr.size()+1%].subchain = chain;
 *  return next_chain(text, fc%name()%);
 * }
 */
string Alternative::ccompile() {
	stringstream s;
	s << cprototype() << " {" << endl;
	s << "\tint pos = 0, cpos = 0;" << endl;
	for (ICompilable* c: expr) {
		s << "\tpos = " << c->funcname() << "(text);" << endl;
		s << "\tif (pos != -1) {" << endl;
		s << "\t\tcpos = chain_next(text+pos, chain);" << endl;
		s << "\t\tif (cpos != -1) return cpos + pos;" << endl;
		s << "\t}" << endl;
	}
	s << "\treturn -1;" << endl;
	s << "}" << endl << endl;
	return s.str();
}

//-------------------------------------------------------------------------------------------
/**
 * compiled code:
 * %prototype()% {
 * 	int pos = 0;
 *  %for (shared_ptr<ICompilable> c: expr) {%
 *  	pos = %c->funcname()%(text);
 *  	if (pos != -1) return pos;
 *  %}%
 *  return -1;
 *  }
 */
string Alternative::compile() {
	stringstream s;
	s << prototype() << " {" << endl;
	s << "\tint pos = 0;" << endl;
	for (ICompilable* c: expr) {
		s << "\tpos = " << c->funcname() << "(text);" << endl;
		s << "\tif (pos != -1) return pos;" << endl;
	}
	s << "\treturn -1;" << endl;
	s << "}" << endl << endl;
	return s.str();
}

//-------------------------------------------------------------------------------------------
vector<ICompilable*> & Alternative::dependences() {
	return this->expr;
}

//-------------------------------------------------------------------------------------------
string Alternative::name() {
	stringstream s;
	s << "al";
	for (ICompilable* c: expr)
		s << '_' << c->name();
	return s.str();
}

//-------------------------------------------------------------------------------------------
int Alternative::type() { return TYPE; }

} /* namespace pagen */
