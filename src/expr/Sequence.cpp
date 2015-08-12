/**
 * pgen, Parser Generator.
 * Copyright (C) 2015 Dimas Melo Filho
 * 
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA. 
 * 
 * The author can be reached by e-mail: dldmf@cin.ufpe.br.
 * 
 * \author Dimas Melo Filho <dldmf@cin.ufpe.br>
 * \date 2014-06-26
 * \file
 * The Sequence class represents a sequence of sub-expressions that have to be matched for a given regular expression. 
 * For instance, consider the following regular expression: '[a-z]+[0-9]b'. It requires the following sub-expressions to
 * be matched in sequence:
 * 	1. '[a-z]+', a Quantified CharClass
 * 	2. '[0-9]', a single occurence of a CharClass
 * 	3. 'b', a single Character 'b'.
 */

// STL
#include <iostream>
#include <string>
#include <sstream>
#include <memory>
// pgen
#include "ICompilable.h"
#include "Sequence.h"

using namespace std;
namespace pgen 
{

	Sequence::Sequence() 
	{		
	}

	Sequence::~Sequence() 
	{
		while (!expr.empty()) 
		{
			delete expr.back();
			expr.pop_back();
		}
	}

	void Sequence::add(ICompilable* expression) 
	{
		expr.push_back(expression);
	}

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
	string Sequence::ccompile() 
	{
		stringstream s;
		string name = this->name();
		s << "chainptr fc" << name << "[] = {" 								"\n";
		for (unsigned int i=0; i < expr.size(); i++) 
		{
			s << " {(int (*)())(" << expr[i]->cfuncname() << ")},"			"\n";
		}
		s << " {CHAIN_JUMP},"												"\n"
			 " {NULL}" 														"\n"
			 "};" 															"\n\n"
		  << cprototype() << " {" 											"\n"
			 " fc" << name << "[" << expr.size()+1 << "].subchain = chain;" "\n"
			 " return next_chain(text, fc" << name << ");"					"\n"
			 "}" 															"\n\n";
		return s.str();
	}

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
	string Sequence::compile() 
	{
		stringstream s;
		string name = this->name();
		s << "chainptr f" << name << "[] = {"									"\n";
		for (unsigned int i=0; i < expr.size()-1; i++) 
		{
			s << " {(int (*)())(" << expr[i]->cfuncname() << ")},"				"\n";
		}
		if (expr.size() > 0) 
		{ 
			s << " {(int (*)())(" << expr[expr.size()-1]->funcname() << ")},"	"\n";
		}
		s << " {NULL}" 															"\n"
			 "};"																"\n\n"
		  << prototype() << " {"												"\n"
			 " return chain_next(text, f" << name << ");"						"\n"
			 "}"																"\n\n";
		return s.str();
	}

	vector<ICompilable*> & Sequence::dependences()
	{
		return this->expr;
	}

	string Sequence::name() 
	{
		stringstream s;
		s << "sq";
		for (ICompilable* c: this->expr)
			s << "_" << c->name();
		return s.str();
	}

	/**
	 * @returns the type identifier of this class.
	 */
	int Sequence::type() 
	{ 
		return TYPE; 
	}
	
	#ifdef _DEBUG
	void Sequence::print(int level)
	{
		for (int i = 0; i < level; i++) 
		{
			cout << '.';
		}
		cout << "SEQ(@" << hex << this << ", SZ=" << dec << expr.size() << ")" << endl;
		level++;
		for (ICompilable* comp: expr) 
		{
			comp->print(level);
		}
	}
	#endif

} /* namespace pgen */
