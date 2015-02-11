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
 * \date 2014-10-04
 * \file
 * This is a file that declares the class Regex, a class that is an ICompilable object that can parse a regular 
 * expression (text).
 * This is a special class, it behaves as an Alternative class that is able to parse a textual Regular Expression into a
 * memory structure that uses all the other classes presented above. This class is used to parse the textual regular 
 * expression into a representation that can be used directly by the rest of the application.
 */

namespace pgen 
{
	class Regex;
}

#ifndef REGEX_H_
#define REGEX_H_

// STL
#include <string>
#include <deque>
#include <exception>
// pgen
#include "ICompilable.h"
#include "Alternative.h"
#include "../parser/NamedClassManager.h"
#include "RegexState.h"

using namespace std;
namespace pgen 
{
	class Regex
	 : public Alternative 
	{
	private:
		int group;				//< Group count, for parsing.
		ICompilable* parse(string& expression, NamedClassManager& ncm);
		ICompilable* parse(string& expression, unsigned int &pos, Alternative* alt, NamedClassManager& ncm);	
	public:
		Regex(string expression, NamedClassManager &ncm);
		virtual ~Regex();
		
		#ifdef _DEBUG
		virtual void print(int level = 0);
		#endif
	}; /* class Regex */
} /* namespace pgen */

#endif /* REGEX_H_ */
