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
 * The Character class represents a single character that should be matched on the Regular Expression. For instance, if
 * someone creates a regular expression containing a single character 'x', this character will be encoded using this 
 * class.
 */

// STL
#include <iostream>
#include <string>
#include <sstream>
// Other
#include "Character.h"
using namespace std;

namespace pgen 
{

	Character::Character(unsigned int c)
	 : c(c) 
	{ 
	}

	Character::~Character() 
	{ 
	}

	unsigned int Character::getChar()
	{
		return c;
	}

	/**
	 * compiled code (chained version):
	 * %cprototype()% {
	 * 	int pos = 0;
	 * 	if (next_utf8(text, &pos) == %c%) {
	 * 	 int res = chain_next(text + pos, chain);
	 * 	 if (res != -1) return res + pos;
	 * 	}
	 *	return -1;
	 * }
	 */
	string Character::ccompile() 
	{
		stringstream s;
		s << 
			cprototype() << " {" 							"\n"
			" int pos = 0;"									"\n"
			" if (next_utf8(text, &pos) == " << c << ") {" 	"\n"
			"  int res = chain_next(text + pos, chain);"	"\n"
			"  if (res != -1) return res + pos;"			"\n"
			" }"											"\n"
			" return -1;"									"\n"
			"}"												"\n\n";
		return s.str();
	}

	/**
	 * compiled code: (non-chained version)
	 * %prototype()% {
	 * 	int pos = 0;
	 * 	if (next_utf8(text, &pos) == %c%) return pos;
	 * 	return -1;
	 * }
	 */
	string Character::compile() 
	{
		stringstream s;
		s << 
			this->prototype() << " {"									"\n"
			" int pos = 0;"												"\n"
			" if (next_utf8(text, &pos) == " << c << ") return pos;"	"\n"
			" return -1;"												"\n"
			"}"															"\n\n";
		return s.str();
	}

	/**
	 * \returns the name "chr_" + the integer value of the character.
	 */
	string Character::name() 
	{
		stringstream s;
		s << "ch_" << c;
		return s.str();
	}

	/**
	 * \returns the type identifier of this class.
	 */
	int Character::type() 
	{ 
		return TYPE; 
	}
	
	#ifdef _DEBUG
	void Character::print(int level)
	{
		for (int i = 0; i < level; i++) 
		{
			cout << '.';
		}
		cout << "CHAR(@" << hex << this << ", CHR=" << dec << c << ")" << endl;
	}
	#endif

} /* namespace pgen */
