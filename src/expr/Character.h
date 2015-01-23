/*
 * Character.h
 *
 *  Created on: Jun 26, 2014
 *      Author: Dimas Melo Filho
 */

#ifndef CHARACTER_H_
#define CHARACTER_H_

#include <string>
#include "ICompilable.h"
using namespace std;

namespace pgen {

class Character: public ICompilable {
private:
	unsigned int c;		//< The character that will be matched by this ICompilable.
public:
	/**
	 * Create a Character object, which is an ICompilable that is responsible for matching a
	 * single character. This is used to compile do simpler expressions for matching single
	 * characters ins tead of using Class or Range objects.
	 * @param c the unicode character to match.
	 */
	Character(unsigned int c);
	virtual ~Character();
	
	/**
	 * @return The character that will be matched by this object
	 */
	unsigned int getChar();

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

	static const int TYPE = 2;
};

} /* namespace pagen */

#endif /* CHARACTER_H_ */
