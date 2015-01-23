/*
 * Regex.h
 *
 *  Created on: Oct 4, 2014
 *      Author: Dimas Melo Filho
 */

#ifndef REGEX_H_
#define REGEX_H_

#include "ICompilable.h"
#include "Alternative.h"
#include <string>
#include <deque>
#include <exception>

using namespace std;

namespace pgen {

class Regex: public Alternative {
private:
	int group;				//< Group count, for parsing.
	ICompilable* parse(string expression);
	ICompilable* parse(string expression, unsigned int &pos, Alternative* alt);
public:
	Regex(string expression);
	virtual ~Regex();
};

enum RegexState {
	MakeAlternative,		// State used when mounting a sequence of an alternative
	EscapeSequence,			// Inside an escape sequence
	EscapeUnicode,			// Reading hex digits for unicode escape sequence
	EscapeHex,				// Reading hex digits for hex escape sequence
	EscapeOctal,			// Reading oct digits for oct escape sequence
	ExplicitQuantifier1,	// Reading the first number of the explicit quantifier
	ExplicitQuantifier2,	// Reading the second number of the explicit quantifier
	ClassReturnEscape,		// Returning from an escape to inside a class
	ClassFirst,				// Reading the first character of a class
	Class,					// Parsing the characters of a class
	ClassRange,				// Reading one range of a class
	ClassRangeReturnEscape	// Returning from an escape state called from the ClassRange state
};

} /* namespace urecc */

#endif /* REGEX_H_ */
