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
 * The Text class was built as an optimization to replace Sequences of Characters on Regular Expressions. For instance, 
 * consider the following Regular expression: 'abcd'. It could be represented by a Sequence of Characters as follows:
 *  - Sequence(Character('a'), Character('b'), Character('c'), Character('d'))
 * 
 * However, it could be more efficiently treated as a text string as follows:
 *  - Text("abcd")
 * 
 * It is also faster to match for strings than Sequences of characters in a row in the C99 language.
 */

// STL
#include <string>
#include <sstream>
#include <iostream>
// pgen
#include "Text.h"
#include "../misc/Util.h"

using namespace std;
namespace pgen 
{

	Text::Text(string text)
	 : text(text) 
	{
	}

	Text::~Text() 
	{
	}

	/**
	 * We use the text.length property instead of Util::strlen_utf8 because the "pos" variable
	 * holds the position in number of bytes, not in utf8 characters.
	 * compiled code (chained version):
	 * %cprototype()% {
	 * 	int pos = 0, posStr = 0;
	 * 	char *str = "%Util::cstr(text)%";
	 * 	do {
	 * 		if (next_utf8(text, &pos) != next_utf8(str, &posStr)) return -1;
	 * 	} while (pos < %text.length%);
	 * 	int res = chain_next(text + pos, chain);
	 * 	if (res == -1) return -1;
	 * 	return res + pos;
	 * }
	 */
	string Text::ccompile() 
	{
		stringstream s;
		s << cprototype() << " {"													"\n"
			 " int pos = 0, posStr = 0;"											"\n"
			 " char *str = \"" << Util::cstr(text) << "\";"							"\n"
			 " do {"																"\n"
			 "  if (next_utf8(text, &pos) != next_utf8(str, &posStr)) return -1;"	"\n"
			 " } while (pos < " << text.length() << ");"							"\n"
			 " int res = chain_next(text + pos, chain);"							"\n"
			 " if (res == -1) return -1;"											"\n"
			 " return res + pos;"													"\n"
			 "}"																	"\n\n";
		return s.str();
	}

	/**
	 * We use the text.length property instead of Util::strlen_utf8 because the "pos" variable
	 * holds the position in number of bytes, not in utf8 characters.
	 * compiled code: (non-chained version)
	 * %prototype()% {
	 * 	int pos = 0, posStr = 0;
	 * 	char* str = "%Util::cstr(text)%";
	 * 	do {
	 * 		if (next_utf8(text, &pos) != next_utf8(str, &posStr)) return -1;
	 * 	} while (pos < %text.length%);
	 * 	return pos;
	 * }
	 */
	string Text::compile() 
	{
		stringstream s;
		s << this->prototype() << " {"												"\n"
			 " int pos = 0, posStr = 0;"											"\n"
			 " char* str = \"" << Util::cstr(text) << "\";"							"\n"
			 " do {"																"\n"
			 "  if (next_utf8(text, &pos) != next_utf8(str, &posStr)) return -1;"	"\n"
			 " } while (pos < " << text.length() << ");"							"\n"
			 " return pos;"															"\n"
			 "}"																	"\n";
		return s.str();
	}

	/**
	 * @return the name "chr_" + the integer value of the character.
	 */
	string Text::name()
	{
		stringstream s;
		s << "tx_";
		unsigned int len = Util::strlen_utf8(text);
		unsigned int i = 0;
		while (i < len) 
		{
			unsigned int c = Util::next_utf8(text, i);
			if (
				(c > '0' && c <= '9') || 
				(c >= 'A' && c <= 'Z') || 
				(c >= 'a' && c <= 'z')
			) 
			{ 
				s << ((char)c);
			}
			else 
			{ 
				s << "_" << c << "_";
			}
		}
		return s.str();
	}

	/**
	 * @returns the type identifier of this class.
	 */
	int Text::type() 
	{ 
		return TYPE; 
	}
	
	#ifdef _DEBUG
	void Text::print(int level)
	{
		for (int i = 0; i < level; i++) 
		{
			cout << '.';
		}
		cout << "TEXT(@" << hex << this << ", TXT=\"" << Util::cstr(text) << "\")" << endl;
	}
	#endif

} /* namespace pgen */
