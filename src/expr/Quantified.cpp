/*
 * Quantified.cpp
 *
 *  Created on: Jun 18, 2014
 *      Author: Dimas Melo Filho
 */

#include "Quantified.h"
#include "ICompilable.h"
#include <memory>
#include <string>
#include <sstream>
using namespace std;

namespace pgen {

//-------------------------------------------------------------------------------------------
Quantified::Quantified(ICompilable* pExpr, unsigned int min, unsigned int max):
min(min), max(max), expr(pExpr), greedy(true) {
	deps.push_back(expr);
}

//-------------------------------------------------------------------------------------------
Quantified::Quantified(ICompilable* pExpr, unsigned int min, unsigned int max, bool greedy):
min(min), max(max), expr(pExpr), greedy(greedy) {
	deps.push_back(expr);
}

//-------------------------------------------------------------------------------------------
Quantified::~Quantified() { 
	delete expr;
}

//-------------------------------------------------------------------------------------------
/**
 * compiled code:
 * %cprototype()% {
 *  int pos = 0, p = 0;
 *  stack_int stack;
 *  stack_int_init(&stack);
 *  while (%if (max != INF) "stack->top < " + max + " && "%(p = %expr->funcname()%(text + pos)) != -1) {
 *  	pos += p;
 *  	stack_int_push(&stack, p);
 *  }
 *  while (stack->top >= %min%) {
 *   p = chain_next(text + pos, chain);
 *   if (p >= 0) {
 *   	stack_int_free(&stack);
 *   	return pos + p;
 *   }
 *   if (stack->top == 0) break;
 *   pos -= stack_int_pop(&stack);
 *  }
 *  stack_int_free(&stack);
 *  return -1;
 * }
 */
string Quantified::ccompile_greedy() {
	stringstream s;
	s << this->cprototype() << " {" << endl;
	s << "\tint pos = 0, p = 0;" << endl;
	s << "\tstack_int stack;" << endl;
	s << "\tstack_int_init(&stack);" << endl;
	s << "\twhile (";
	if (max != INF) s << "stack.top < " << max << " && ";
	s << "(p = " << expr->funcname() << "(text + pos)) != -1) {" << endl;
	s << "\t\tpos += p;" << endl;
	s << "\t\tstack_int_push(&stack, p);" << endl;
	s << "\t}" << endl;
	s << "\twhile (stack.top >= " << min << ") {" << endl;
	s << "\t\tp = chain_next(text + pos, chain);" << endl;
	s << "\t\tif (p >= 0) {" << endl;
	s << "\t\t\tstack_int_free(&stack);" << endl;
	s << "\t\t\treturn pos + p;" << endl;
	s << "\t\t}" << endl;
	s << "\t\tif (stack.top == 0) break;" << endl;
	s << "\t\tpos -= stack_int_pop(&stack);" << endl;
	s << "\t}" << endl;
	s << "\tstack_int_free(&stack);" << endl;
	s << "\treturn -1;" << endl;
	s << "}" << endl << endl;
	return s.str();
}

//-------------------------------------------------------------------------------------------
/**
 * compiled code:
 * *prototype* {
 *  int pos = 0, p = 0, count = 0;
 *  while (%if (max != INF) "count < " + max + " && "%(p = %expr->funcname()%(text + pos)) != -1) {
 *  	pos += p;
 *  	count++;
 *  }
 * 	if (count >= %min%) return pos;
 * 	return -1;
 * }
 */

string Quantified::compile_greedy() {
	stringstream s;
	s << this->prototype() << " {" << endl;
	s << "\tint pos = 0, p = 0, count = 0;" << endl;
	s << "\twhile (";
	if (max != INF) s << "count < " << max << " && ";
	s << "(p = " << expr->funcname() << "(text + pos)) != -1) {" << endl;
	s << "\t\tpos += p;" << endl;
	s << "\t\tcount++;" << endl;
	s << "\t}" << endl;
	s << "\tif (count >= " << min << ") return pos;" << endl;
	s << "\treturn -1;" << endl;
	s << "}" << endl << endl;
	return s.str();
}

//-------------------------------------------------------------------------------------------
/**
 * compiled code:
 * *prototype* {
 *  int pos = 0, p = 0, count = 0;
 *  while (count < %min% && (p = %expr->funcname()%(text + pos)) != -1) {
 *  	pos += p;
 *  	count++;
 *  }
 *  while (%max != INF ? "count <= " + max : "true"%) {
 *   p = chain_next(text + pos, chain);
 *   if (p >= 0) return pos + p;
 *   p = %expr->funcname()%(text + pos);
 *   if (p == -1) break;
 *   pos += p;
 *   count++;
 *  }
 *  return -1;
 * }
 */
string Quantified::ccompile_lazy() {
	stringstream s;
	s << this->cprototype() << " {" << endl;
	s << "\tint pos = 0, p = 0, count = 0;" << endl;
	s << "\twhile(count < " << min << " && (p = " << expr->funcname() << "(text + pos)) != -1) {" << endl;
	s << "\t\tpos += p;" << endl;
	s << "\t\tcount++;" << endl;
	s << "\t}" << endl;
	s << "\twhile (";
	if (max != INF) s << "count <= " << max;
	else s << "true";
	s << ") {" << endl;
	s << "\t\tp = chain_next(text + pos, chain);" << endl;
	s << "\t\tif (p >= 0) return pos + p;" << endl;
	s << "\t\tp = " << expr->funcname() << "(text + pos);" << endl;
	s << "\t\tif (p == -1) break;" << endl;
	s << "\t\tpos += p;" << endl;
	s << "\t\tcount++;" << endl;
	s << "\t}" << endl;
	s << "\treturn -1;" << endl;
	s << "}" << endl << endl;
	return s.str();
}

//-------------------------------------------------------------------------------------------
/**
 * compiled code:
 * *prototype* {
 *  int pos = 0, p = 0, count = 0;
 *  while (count < %min% && (p = %expr->funcname()%(text + pos)) != -1) {
 *  	pos += p;
 *  	count++;
 *  }
 *  %if (max != INF)% return pos;
 *  %else {%
 * 	 if (count <= %max%) return pos;
 * 	 return -1;
 * 	%}%
 * }
 */
string Quantified::compile_lazy() {
	stringstream s;
	s << this->prototype() << " {" << endl;
	s << "\tint pos = 0, p = 0, count = 0;" << endl;
	s << "\twhile (count < " << min << " && (p = " << expr->funcname() << "(text + pos)) != -1) {" << endl;
	s << "\t\tpos += p;" << endl;
	s << "\t\tcount++;" << endl;
	s << "\t}" << endl;
	if (max == INF) s << "return pos;";
	else {
		s << "\tif (count <= " << max << ") return pos;" << endl;
		s << "\treturn -1;" << endl;
	}
	s << "}" << endl << endl;
	return s.str();
}

//-------------------------------------------------------------------------------------------
/**
 * to see the code look at the following methods:
 * compile_greedy(string & chain) - greedy chained code
 * compile_lazy(string & chain) - lazy chained code
 */
string Quantified::ccompile() {
	return (greedy ? ccompile_greedy() : ccompile_lazy());
}

//-------------------------------------------------------------------------------------------
/**
 * to see the code look at the following methods:
 * compile_greedy() - greedy non-chained code
 * compile_lazy() - lazy non-chained code
 */
string Quantified::compile() {
	return (greedy ? compile_greedy() : compile_lazy());
}

//-------------------------------------------------------------------------------------------
vector<ICompilable*> & Quantified::dependences() {
	return this->deps;
}

//-------------------------------------------------------------------------------------------
string Quantified::name() {
	stringstream s;
	s << "qt" << (greedy ? "g" : "l") << "_" << min << "_" << max << "_" << expr->name();
	return s.str();
}

//-------------------------------------------------------------------------------------------
int Quantified::type() { return TYPE; }

} /* namespace pagen */
