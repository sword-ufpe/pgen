/*
 * Sequence.cpp
 *
 *  Created on: Jun 26, 2014
 *      Author: Dimas Melo Filho
 */

#include <string>
#include <sstream>
#include <memory>
#include "ICompilable.h"
#include "Sequence.h"
using namespace std;

namespace pgen {

//-------------------------------------------------------------------------------------------
Sequence::Sequence() {
	
}

//-------------------------------------------------------------------------------------------
Sequence::~Sequence() {
	while (!expr.empty()) {
		delete expr.back();
		expr.pop_back();
	}
}

//-------------------------------------------------------------------------------------------
void Sequence::add(ICompilable* expression) {
	expr.push_back(expression);
}

//-------------------------------------------------------------------------------------------
/**
 * compiled code: (chained version)
 * chainptr fc%name()%[] = {
 * %for (int i=0; i<expr->size(); i++)%
 *   {(int (*)())(%expr[i]->cfuncname()%)},
 * %end for%
 *   {CHAIN_JUMP},
 *   {NULL}
 * };
 *
 * %cprototype()% {
 *  fc%name()%[%expr.size()+1%].subchain = chain;
 *  return next_chain(text, fc%name()%);
 * }
 */
string Sequence::ccompile() {
	stringstream s;
	string name = this->name();
	s << "chainptr fc" << name << "[] = {" << endl;
	for (unsigned int i=0; i < expr.size(); i++) {
		s << "\t{(int (*)())(" << expr[i]->cfuncname() << ")}," << endl;
	}
	s << "\t{CHAIN_JUMP}," << endl << "\t{NULL}" << endl << "};" << endl << endl;
	s << cprototype() << " {" << endl;
	s << "\tfc" << name << "[" << expr.size()+1 << "].subchain = chain;" << endl;
	s << "\treturn next_chain(text, fc" << name << ");" << endl;
	s << "}" << endl << endl;
	return s.str();
}

//-------------------------------------------------------------------------------------------
/**
 * compiled code: (non-chained version)
 * chainptr f%name()%[] = {
 * %for (int i=0; i<expr.size()-1; i++)%
 *   {(int (*)())(%expr[i]->cfuncname()%)},
 * %end for%
 * %if (expr.size() > 0)%
 *   {(int (*)())(%expr[expr.size()-1]->funcname()%)},
 * %end if%
 *   {NULL}
 * };
 *
 * %prototype()% {
 *  return next_chain(text, f%name()%);
 * }
 */
string Sequence::compile() {
	stringstream s;
	string name = this->name();
	s << "chainptr f" << name << "[] = {" << endl;
	for (unsigned int i=0; i < expr.size()-1; i++) {
		s << "\t{(int (*)())(" << expr[i]->cfuncname() << ")}," << endl;
	}
	if (expr.size() > 0) s << "\t{(int (*)())(" << expr[expr.size()-1]->funcname() << ")}," << endl;
	s << "\t{NULL}" << endl << "};" << endl << endl;
	s << prototype() << " {" << endl;
	s << "\treturn chain_next(text, f" << name << ");" << endl;
	s << "}" << endl << endl;
	return s.str();
}

//-------------------------------------------------------------------------------------------
vector<ICompilable*> & Sequence::dependences() {
	return this->expr;
}

//-------------------------------------------------------------------------------------------
string Sequence::name() {
	stringstream s;
	s << "sq";
	for (ICompilable* c: this->expr)
		s << "_" << c->name();
	return s.str();
}

//-------------------------------------------------------------------------------------------
int Sequence::type() { return TYPE; }

} /* namespace pagen */
