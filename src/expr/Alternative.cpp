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
 * \date 2014-06-30
 * \file
 * The Alternative class represents a list of expressions that can be accepted by the parser. On the Regular Expression 
 * it is represented by the '|' character.
 */

// STL
#include <iostream>
#include <sstream>
// pgen
#include "Alternative.h"

using namespace std;
namespace pgen 
{

	Alternative::Alternative() 
	{
	}

	Alternative::~Alternative() 
	{
		// Delete all sub-expressions
		while (!expr.empty()) 
		{
			delete expr.back();
			expr.pop_back();
		}
	}

	void Alternative::add(ICompilable* expression) 
	{
		expr.push_back(expression);
	}

	/**
	 * compiled code: (chained version)
	 * %cprototype()% {
	 *  fc%name()%[%expr.size()+1%].subchain = chain;
	 *  return next_chain(text, fc%name()%);
	 * }
	 */
	string Alternative::ccompile() 
	{
		stringstream s;
		s << cprototype() << " {" << endl;
		s << "\tint pos = 0, cpos = 0;" << endl;
		for (ICompilable* c: expr) 
		{
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
	string Alternative::compile() 
	{
		stringstream s;
		s << prototype() << " {" << endl;
		s << "\tint pos = 0;" << endl;
		for (ICompilable* c: expr) 
		{
			s << "\tpos = " << c->funcname() << "(text);" << endl;
			s << "\tif (pos != -1) return pos;" << endl;
		}
		s << "\treturn -1;" << endl;
		s << "}" << endl << endl;
		return s.str();
	}

	vector<ICompilable*> & Alternative::dependences() 
	{
		return this->expr;
	}

	string Alternative::name() 
	{
		stringstream s;
		s << "al";
		for (ICompilable* c: expr)
		{
			s << '_' << c->name();
		}
		return s.str();
	}

	/**
	 * \returns the type identifier of this class.
	 */
	int Alternative::type() 
	{ 
		return TYPE; 
	}

	#ifdef _DEBUG
	void Alternative::print(int level)
	{
		for (int i = 0; i < level; i++) 
		{
			cout << '.';
		}
		cout << "ALT(@" << hex << this << ", SZ=" << dec << expr.size() << ")" << endl;
		level++;
		for (ICompilable* comp: expr) 
		{
			comp->print(level);
		}
	}
	#endif

} /* namespace pgen */
