/*
 * Sequence.h
 *
 *  Created on: Jun 26, 2014
 *      Author: Dimas Melo Filho
 */

#ifndef SEQUENCE_H_
#define SEQUENCE_H_

#include "../test/Test.h"
#include <memory>
#include <string>
#include <vector>
#include "ICompilable.h"
using namespace std;

namespace pgen {

/**
 * Class that parses a chained sequence of ICompilables.
 */
class Sequence: public ICompilable {
private:										// see "Test.h" for more info on tprivate
	vector<ICompilable*> expr;					//< Sequence of expressions (vector of ICompilable shared pointers).
public:
	Sequence();
	virtual ~Sequence();

	/**
	 * Adds an ICompilable expression to the sequence.
	 * @param expression a shared_ptr ICompilable object.
	 * @remark the first expression to be parsed should be added first (FIFO).
	 */
	void add(ICompilable* expression);

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
	 * Returns a vector<ICompilable> of dependences in case of using the chained version
	 * of the algorithm. It can also return NULL when there are no dependences.
	 * @return A vector of ICompilable dependences of the code.
	 */
	virtual vector<ICompilable*> & dependences();
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

	static const int TYPE = 6;
};

} /* namespace pagen */

#endif /* SEQUENCE_H_ */
