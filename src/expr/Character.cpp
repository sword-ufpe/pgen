/*
 * Character.cpp
 *
 *  Created on: Jun 26, 2014
 *      Author: Dimas Melo Filho
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
