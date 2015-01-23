/*
 * Character.cpp
 *
 *  Created on: Jun 26, 2014
 *      Author: Dimas Melo Filho
 */

#include <string>
#include <sstream>
#include "Character.h"
using namespace std;

namespace pgen {

//-------------------------------------------------------------------------------------------
Character::Character(unsigned int c):
c(c) { }

//-------------------------------------------------------------------------------------------
Character::~Character() { }

//-------------------------------------------------------------------------------------------
unsigned int Character::getChar()
{
	return c;
}

//-------------------------------------------------------------------------------------------
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
string Character::ccompile() {
	stringstream s;
	s << cprototype() << " {" << endl;
	s << "\tint pos = 0;" << endl;
	s << "\tif (next_utf8(text, &pos) == " << c << ") {" << endl;
	s << "\t\tint res = chain_next(text + pos, chain);" << endl;
	s << "\t\tif (res != -1) return res + pos;" << endl;
	s << "\t}" << endl;
	s << "\treturn -1;" << endl;
	s << "}" << endl;
	return s.str();
}

//-------------------------------------------------------------------------------------------
/**
 * compiled code: (non-chained version)
 * %prototype()% {
 * 	int pos = 0;
 * 	if (next_utf8(text, &pos) == %c%) return pos;
 * 	return -1;
 * }
 */
string Character::compile() {
	stringstream s;
	s << this->prototype() << " {" << endl;
	s << "\tint pos = 0;" << endl;
	s << "\tif (next_utf8(text, &pos) == " << c << ") return pos;" << endl;
	s << "\treturn -1;" << endl;
	s << "}" << endl;
	return s.str();
}

//-------------------------------------------------------------------------------------------
/**
 * @return the name "chr_" + the integer value of the character.
 */
string Character::name() {
	stringstream s;
	s << "ch_" << c;
	return s.str();
}

//-------------------------------------------------------------------------------------------
int Character::type() { return TYPE; }

} /* namespace pagen */
