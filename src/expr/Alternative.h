/*
 * Alternative.h
 *
 *  Created on: Jun 30, 2014
 *      Author: Dimas Melo Filho
 */

#ifndef ALTERNATIVE_H_
#define ALTERNATIVE_H_

#include "../test/Test.h"
#include "ICompilable.h"
#include <string>
#include <vector>
#include <memory>
using namespace std;

namespace pagen {

class Alternative : public ICompilable {
public:
	vector<ICompilable*> expr;		//< Alternative expressions.

	Alternative();
	virtual ~Alternative();

	/**
	 * Adds an ICompilable expression as an alternative.
	 * @param expression a shared_ptr ICompilable object.
	 * @remark the first expression to be tried out should be added first (FIFO).
	 */
	void add(ICompilable* expression);

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
	 * @return The name of the expression (usable as function names).
	 */
	virtual string name();
	/**
	 * Points to the implementation of the method of the subclass.
	 * The method does nothing. It serves only as a pointer to identify the class.
	 */
	virtual int type();

	static const int TYPE = 1;
};

} /* namespace pagen */

#endif /* ALTERNATIVE_H_ */
