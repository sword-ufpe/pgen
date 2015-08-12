/*
 * InvaidCharacter.h
 *
 *  Created on: Jun 13, 2014
 *      Author: Dimas Melo Filho
 */

#ifndef INVALIDCHARACTER_H_
#define INVALIDCHARACTER_H_

// STL
#include <exception>

namespace pgen 
{
	class InvalidCharacter 
	 : public std::exception 
	{
	public:
		InvalidCharacter();
		virtual ~InvalidCharacter() throw ();
		virtual const char * what() const throw ();
	}; /* class InvalidCharacter */
} /* namespace pgen */

#endif /* INVALIDCHARACTER_H_ */
