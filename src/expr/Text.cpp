/*
 * Text.cpp
 * This file implements the Text class which is an ICompilable that is able to compile code
 * to parse a string.
 *
 *  Created on: Oct 4, 2014
 *      Author: Dimas Melo Filho
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
		int len = Util::strlen_utf8(text), i = 0;
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
