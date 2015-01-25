/*
 * RegexException.h
 *
 *  Created on: Oct 5, 2014
 *      Author: dimas
 */

#ifndef REGEXEXCEPTION_H_
#define REGEXEXCEPTION_H_

// STL
#include <exception>
#include <string>

using namespace std;
namespace pgen 
{
	class RegexException
	 : public std::exception 
	{
	public:
		string expression;
		string reason;
		int column;
		RegexException(string expression, int column, string reason);
		virtual ~RegexException() throw ();
		virtual const char * what() const throw ();
	}; /* class RegexException */
} /* namespace pgen */

#endif /* REGEXEXCEPTION_H_ */
