/*
 * InvalidCharacter.cpp
 *
 *  Created on: Jun 13, 2014
 *      Author: Dimas Melo Filho
 */

// pgen
#include "InvalidCharacter.h"

namespace pgen {
	InvalidCharacter::InvalidCharacter() 
	{
	}

	InvalidCharacter::~InvalidCharacter() throw () 
	{
	}

	const char * InvalidCharacter::what() const throw () 
	{
		return "Invalid character";
	}

} /* namespace pgen */
