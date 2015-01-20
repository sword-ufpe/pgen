/*
 * InvaidCharacter.h
 *
 *  Created on: Jun 13, 2014
 *      Author: Dimas Melo Filho
 */

#ifndef INVALIDCHARACTER_H_
#define INVALIDCHARACTER_H_

#include <exception>

namespace pagen {

class InvalidCharacter : public std::exception {
public:
	InvalidCharacter();
	virtual ~InvalidCharacter() throw ();
	virtual const char * what() const throw ();
};

} /* namespace pagen */

#endif /* INVALIDCHARACTER_H_ */
