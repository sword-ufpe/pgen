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
 * This file contains the RegexEscape class implementation. The class contains a method to parse regular expression 
 * escape sequences and return the correct ICompilable object pointer for the escape sequence.
 */

// STL
#include <string>
// pgen
#include "ICompilable.h"
#include "Character.h"
#include "CharClass.h"
#include "RegexState.h"
#include "RegexEscape.h"
#include "../misc/Util.h"
#include "../misc/RegexException.h"


using namespace std;
namespace pgen
{
	ICompilable* RegexEscape::digitsClass()
	{
		CharClass* digits = new CharClass();
		digits->add('0','9');
		return digits;	
	}
	
	ICompilable* RegexEscape::spacesClass()
	{
		CharClass* spaces = new CharClass();
		spaces->add(0x09,0x0D);
		spaces->add(' ');
		return spaces;
	}
	
	ICompilable* RegexEscape::nonSpacesClass()
	{
		CharClass* nonSpaces = (CharClass*) RegexEscape::spacesClass();
		nonSpaces->invert();
		return nonSpaces;
	}
	
	ICompilable* RegexEscape::Hex(string& expression, unsigned int& pos, int len)
	{
		int count = 0;
		unsigned int c = 0;
		unsigned int charCode = 0;
		do
		{
			c = Util::next_utf8(expression, pos);
			if ( (c < '0' || c > '9') && (c < 'A' || c > 'F') && (c < 'a' || c > 'f'))
			{
				throw RegexException(expression, pos, "Invalid character. Expected hex digit at a regular expression escape sequence.");
			}
			charCode = (charCode * 16) + Util::hex2int(c);
		} while (++count < len);
		return new Character(charCode);
	}
	
	ICompilable* RegexEscape::Octal(string& expression, unsigned int& pos, int len)
	{
		int count = 0;
		unsigned int c = 0;
		unsigned int charCode = 0;
		do
		{
			c = Util::next_utf8(expression, pos);
			if (c < '0' || c > '7')
			{
				throw RegexException(expression, pos, "Invalid character. Expected octal digit at a regular expression escape sequence.");
			}
			charCode = (charCode * 8) + Util::oct2int(c);
		} while (++count < len);
		return new Character(charCode);
	}
	
	ICompilable* RegexEscape::parse(string& expression, unsigned int& pos)
	{
		unsigned int c = 0;
		c = Util::next_utf8(expression, pos);
		switch (c)
		{
		// \a = alert/bell (0x07)
		case 'a': 
			return new Character(0x07);
		// \b = backspace (0x08)
		case 'b': 
			return new Character(0x08);
		// \f = form feed (0x0C)
		case 'f': 
			return new Character(0x0C);
		// \n = new line (0x0A)
		case 'n': 
			return new Character(0x0A);
		// \r = carriage return (0x0D)
		case 'r': 
			return new Character(0x0D);
		// \t = tab (0x09)
		case 't': 
			return new Character(0x09);
		// \v = vertical tab (0x0B)
		case 'v': 
			return new Character(0x0B);
		// \d = the class of all digits. Equivalent to [0-9]
		case 'd': 
			return RegexEscape::digitsClass();
		// \s = the class of all kinds of space. Equivalent to [ \t\r\n\v\f]
		case 's': 
			return RegexEscape::spacesClass();
		// \S = the class of all kinds of non-space characters. Equivalent to [^ \t\r\n\v\f]
		case 'S': 
			return RegexEscape::nonSpacesClass();
		// \u = the beggining of an unicode escape sequence (\uXXXX)
		case 'u': 
			return RegexEscape::Hex(expression, pos, 4);
		// \U = the beggining of a large unicode escape sequence (\UXXXXXXXX)
		case 'U': 
			return RegexEscape::Hex(expression, pos, 8);
		// \x = the beggining of an hexadecimal escape sequence (\xXX)
		case 'x': 
			return RegexEscape::Hex(expression, pos, 2);
		// \[0-7] = the beggining of an octal escape sequence (\ooo)
		case '0': case '1':	case '2': case '3': case '4': case '5': case '6': case '7': 
			return RegexEscape::Octal(expression, pos, 3);					
		// If the end of stream was reached, it is an error.
		case 0x00: case 0x03: case 0x04: 
			throw RegexException(expression, pos, "Unexpected end of expression, expecting escape sequence.");
		// For any other character, just use it (despite its original meaning).
		default:
			return new Character(c);
		} /* end of switch(c) */
	}
}