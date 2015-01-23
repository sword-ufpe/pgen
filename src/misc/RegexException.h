/*
 * RegexException.h
 *
 *  Created on: Oct 5, 2014
 *      Author: dimas
 */

#ifndef REGEXEXCEPTION_H_
#define REGEXEXCEPTION_H_

#include <exception>
#include <string>

namespace pgen {

class RegexException: public std::exception {
public:
	std::string expression;
	std::string reason;
	int column;
	RegexException(std::string expression, int column, std::string reason);
	virtual ~RegexException() throw ();
	virtual const char * what() const throw ();
};

} /* namespace pagen */

#endif /* REGEXEXCEPTION_H_ */
