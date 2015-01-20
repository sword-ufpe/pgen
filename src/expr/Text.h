/*
 * Text.h
 *
 *  Created on: Oct 4, 2014
 *      Author: Dimas Melo Filho
 */

#ifndef TEXT_H_
#define TEXT_H_

#include "ICompilable.h"
#include <string>

using namespace std;

namespace pagen {

class Text: public ICompilable {
private:
	string text;
public:
	Text(string text);
	virtual ~Text();

	// ICompilable Methods Implementation
	/**
	 * Returns C code that can parse the regular expression.
	 * The returned code should be a function with the following
	 * prototype: int expr_name(char *text, int (*chain[])());
	 * the function must return the numbers of consumed bytes on success or -1 on failure.
	 * This version of the function must be chainable with other functions, it must
	 * provide an implementation of the function that accepts an extra parameter called
	 * "chain" that is an array of function pointers (see ICompilable::cprototype() for
	 * more info).
	 * @return the chainable compiled C code that parses the expression.
	 */
	virtual string ccompile();
	/**
	 * Returns C code that can parse the regular expression.
	 * the returned code shuld be a function with the following prototype:
	 * int expr_name(char *text);
	 * The function must return the numbers of consumed bytes on success or -1 on failure.
	 * This version of the function is not chainable.
	 * @return the compiled C code that parses the expression.
	 */
	virtual string compile();
	/**
	 * Returns the name of the expression. The returned name must be usable as a function
	 * name in the standard C (C99). i.e. it has to match the following format:
	 * [a-zA-Z][A-Za-z0-9_]*
	 * @return The text "chr_" + the text corresponding to the integer value of the character.
	 */
	virtual string name();
	/**
	 * Points to the implementation of the method of the subclass.
	 * The method does nothing. It serves only as a pointer to identify the class.
	 */
	virtual int type();

	static const int TYPE = 7;
};

} /* namespace pagen */

#endif /* TEXT_H_ */
