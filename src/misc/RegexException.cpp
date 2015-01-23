/*
 * RegexException.cpp
 *
 *  Created on: Oct 5, 2014
 *      Author: dimas
 */

#include "RegexException.h"
#include <string>

using namespace std;

namespace pgen {

RegexException::RegexException(string expression, int column, string reason):
expression(expression), reason(reason), column(column)
{

}

RegexException::~RegexException() throw () {

}

const char * RegexException::what() const throw () {
	return "Error while parsing a regular expression.";
}

} /* namespace pagen */
