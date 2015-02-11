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
 * \date 2014-06-16
 * \file
 * This class is the base class for all classes that can be compiled to C99 code. All classes in this directory inherit 
 * from ICompilable, except for the "Code" class, which is a special class.
 */

// STL
#include <string>
#include <sstream>
// pgen
#include "ICompilable.h"

using namespace std;
namespace pgen 
{

	vector<ICompilable*> ICompilable::_dependences;

	ICompilable::~ICompilable()
	{
	}

	vector<ICompilable*> & ICompilable::dependences() 
	{
		return ICompilable::_dependences;
	}

	string ICompilable::funcname() 
	{
		stringstream s;
		s << "fn_" << this->name();
		return s.str();
	}

	string ICompilable::cfuncname() 
	{
		stringstream s;
		s << "cf_" << this->name();
		return s.str();
	}

	string ICompilable::prototype() 
	{
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
	string ICompilable::cprototype() 
	{
		stringstream s;
		s << "int " << this->cfuncname() << "(char* text, chainptr *chain)";
		return s.str();
	}
	
} /* namespace pgen */
