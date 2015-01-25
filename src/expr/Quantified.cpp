/*
 * Quantified.cpp
 *
 *  Created on: Jun 18, 2014
 *      Author: Dimas Melo Filho
 */

// STL
#include <memory>
#include <string>
#include <sstream>
#include <iostream>
// pgen
#include "Quantified.h"
#include "ICompilable.h"

using namespace std;
namespace pgen 
{

	Quantified::Quantified(ICompilable* pExpr, unsigned int min, unsigned int max)
	 : min(min)
	 , max(max)
	 , expr(pExpr)
	 , greedy(true) 
	{
		deps.push_back(expr);
	}

	Quantified::Quantified(ICompilable* pExpr, unsigned int min, unsigned int max, bool greedy)
	 : min(min)
	 , max(max)
	 , expr(pExpr)
	 , greedy(greedy) 
	{
		deps.push_back(expr);
	}

	Quantified::~Quantified() 
	{ 
		delete expr;
	}

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
	string Quantified::ccompile_greedy() 
	{
		stringstream s;
		s << this->cprototype() << " {"									"\n"
			 " int pos = 0, p = 0;"										"\n"
			 " stack_int stack;"										"\n"
			 " stack_int_init(&stack);"									"\n"
			 " while (";
		if (max != INF) 
		{
			s << "stack.top < " << max << " && ";
		}
		s << "(p = " << expr->funcname() << "(text + pos)) != -1) {"	"\n"
			 "  pos += p;"												"\n"
			 "  stack_int_push(&stack, p);"								"\n"
			 " }"														"\n"
			 " while (stack.top >= " << min << ") {"					"\n"
			 "  p = chain_next(text + pos, chain);"						"\n"
			 "  if (p >= 0) {"											"\n"
			 "   stack_int_free(&stack);"								"\n"
			 "   return pos + p;"										"\n"
			 "  }"														"\n"
			 "  if (stack.top == 0) break;"								"\n"
			 "  pos -= stack_int_pop(&stack);"							"\n"
			 " }"														"\n"
			 " stack_int_free(&stack);"									"\n"
			 " return -1;"												"\n"
			 "}"														"\n\n";
		return s.str();
	}

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
	string Quantified::compile_greedy() 
	{
		stringstream s;
		s << this->prototype() << " {" 									"\n"
			 " int pos = 0, p = 0, count = 0;"							"\n"
			 " while (";
		if (max != INF)
		{ 
			s << "count < " << max << " && ";
		}
		s << "(p = " << expr->funcname() << "(text + pos)) != -1) {"	"\n"
			 "  pos += p;"												"\n"
			 "  count++;" 												"\n"
			 " }"														"\n"
			 " if (count >= " << min << ") return pos;"					"\n"
			 " return -1;"												"\n"
			 "}"														"\n\n";
		return s.str();
	}

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
	string Quantified::ccompile_lazy() 
	{
		stringstream s;
		s << this->cprototype() << " {" 																"\n"
			 " int pos = 0, p = 0, count = 0;"															"\n"
			 " while(count < " << min << " && (p = " << expr->funcname() << "(text + pos)) != -1) {"	"\n"
			 "  pos += p;" 																				"\n"
			 "  count++;"																				"\n"
			 " }"																						"\n"
			 " while (";
		if (max != INF) 
		{ 
			s << "count <= " << max;
		}
		else 
		{ 
			s << "true";
		}
		s << ") {"																						"\n"
			 "  p = chain_next(text + pos, chain);"														"\n"
			 "  if (p >= 0) return pos + p;"															"\n"
			 "  p = " << expr->funcname() << "(text + pos);"											"\n"
			 "  if (p == -1) break;"																	"\n"
			 "  pos += p;"																				"\n"
			 "  count++;"																				"\n"
			 " }"																						"\n"
			 " return -1;"																				"\n"
			 "}"																						"\n\n";
		return s.str();
	}

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
	string Quantified::compile_lazy() 
	{
		stringstream s;
		s << this->prototype() << " {"																		"\n"
			 " int pos = 0, p = 0, count = 0;"																"\n"
			 " while (count < " << min << " && (p = " << expr->funcname() << "(text + pos)) != -1) {"		"\n"
			 "  pos += p;"																					"\n"
			 "  count++;"																					"\n"
			 " }"																							"\n";
		if (max == INF) 
		{ 
			s << " return pos;"																				"\n";
		}
		else 
		{
			s << " if (count <= " << max << ") return pos;"													"\n"
				 " return -1;"																				"\n";
		}
		s << "}" 																							"\n\n";
		return s.str();
	}

	/**
	 * to see the code look at the following methods:
	 * compile_greedy(string & chain) - greedy chained code
	 * compile_lazy(string & chain) - lazy chained code
	 */
	string Quantified::ccompile() 
	{
		return (greedy ? ccompile_greedy() : ccompile_lazy());
	}

	/**
	 * to see the code look at the following methods:
	 * compile_greedy() - greedy non-chained code
	 * compile_lazy() - lazy non-chained code
	 */
	string Quantified::compile() 
	{
		return (greedy ? compile_greedy() : compile_lazy());
	}

	vector<ICompilable*> & Quantified::dependences() 
	{
		return this->deps;
	}

	string Quantified::name() 
	{
		stringstream s;
		s << "qt" << (greedy ? "g" : "l") << "_" << min << "_" << max << "_" << expr->name();
		return s.str();
	}

	/**
	 * @returns the type identifier of this class.
	 */
	int Quantified::type() 
	{ 
		return TYPE; 
	}
	
	#ifdef _DEBUG
	void Quantified::print(int level)
	{
		for (int i = 0; i < level; i++) 
		{
			cout << '.';
		}
		cout << "QUANT(@" << hex << this << ", LEN={" << dec << min << ',' << max << "})" << endl;
		this->expr->print(level+1);
	}
	#endif

} /* namespace pgen */
