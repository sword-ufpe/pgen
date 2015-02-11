/*
 * RegexException.cpp
 *
 *  Created on: Oct 5, 2014
 *      Author: dimas
 */

// STL
#include <string>
#include <sstream>
// pgen
#include "RegexException.h"

using namespace std;
namespace pgen 
{

	RegexException::RegexException(string expression, int column, string reason)
	 : expression(expression)
	 , reason(reason)
	 , column(column)
	{
	}

	RegexException::~RegexException() throw () 
	{
	}

	const char * RegexException::what() const throw () 
	{
		stringstream s;
		s << "Error while parsing a regular expression: " << reason << " ('" << expression << "':" << column << ")";
		return s.str().c_str();
	}

} /* namespace pgen */
