/*
 * Text.cpp
 * This file implements the Text class which is an ICompilable that is able to compile code
 * to parse a string.
 *
 *  Created on: Oct 4, 2014
 *      Author: Dimas Melo Filho
 */

#include "Text.h"
#include "../misc/Util.h"
#include <string>
#include <sstream>
#include <iostream>

using namespace std;

namespace pgen {

//-------------------------------------------------------------------------------------------
Text::Text(string text):
text(text) {

}

//-------------------------------------------------------------------------------------------
Text::~Text() {

}

//-------------------------------------------------------------------------------------------
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
string Text::ccompile() {
	stringstream s;
	s << cprototype() << " {" << endl;
	s << "\tint pos = 0, posStr = 0;" << endl;
	s << "\tchar *str = \"" << Util::cstr(text) << "\";" << endl;
	s << "\tdo {" << endl;
	s << "\t\tif (next_utf8(text, &pos) != next_utf8(str, &posStr)) return -1;" << endl;
	s << "\t} while (pos < " << text.length() << ");" << endl;
	s << "\tint res = chain_next(text + pos, chain);" << endl;
	s << "\tif (res == -1) return -1;" << endl;
	s << "\treturn res + pos;" << endl;
	s << "}" << endl;
	return s.str();
}

//-------------------------------------------------------------------------------------------
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
string Text::compile() {
	stringstream s;
	s << this->prototype() << " {" << endl;
	s << "\tint pos = 0, posStr = 0;" << endl;
	s << "\tchar* str = \"" << Util::cstr(text) << "\";" << endl;
	s << "\t do {" << endl;
	s << "\t\t if (next_utf8(text, &pos) != next_utf8(str, &posStr)) return -1;" << endl;
	s << "\t} while (pos < " << text.length() << ");" << endl;
	s << "return pos;" << endl;
	s << "}" << endl;
	return s.str();
}

//-------------------------------------------------------------------------------------------
/**
 * @return the name "chr_" + the integer value of the character.
 */
string Text::name() {
	stringstream s;
	s << "tx_";
	int len = Util::strlen_utf8(text), i = 0;
	while (i < len) {
		unsigned int c = Util::next_utf8(text, i);
		if ((c > '0' && c <= '9') || (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z')) s << ((char)c);
		else s << "_" << c << "_";
	}
	return s.str();
}

//-------------------------------------------------------------------------------------------
int Text::type() { return TYPE; }

// TODO: 999 Create a Test Fixture for the Text class.

} /* namespace pagen */
