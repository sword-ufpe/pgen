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
 * \date 2015-02-08
 * \file
 * This file contains the RegexEscape class definition. The class contains a method to parse regular expression escape
 * sequences and return the correct ICompilable object for the escape sequence.
 */

#ifndef PGEN_EXPR_REGEXESCAPE_H_
#define PGEN_EXPR_REGEXESCAPE_H_

// STL
#include <string>
// pgen
#include "ICompilable.h"

using namespace std;
namespace pgen
{
	class RegexEscape
	{
	private:
		static ICompilable* digitsClass();
		static ICompilable* spacesClass();
		static ICompilable* nonSpacesClass();
		static ICompilable* Hex(string& expression, unsigned int& pos, int len);
		static ICompilable* Octal(string& expression, unsigned int& pos, int len);
	public:
		static ICompilable* parse(string& text, unsigned int& pos);
	};
}

#endif /* PGEN_EXPR_REGEXESCAPE_H_ */