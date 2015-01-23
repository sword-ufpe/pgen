/*
 * ICompilable.h
 *
 *  Created on: Jun 16, 2014
 *      Author: Dimas Melo Filho
 */

#ifndef ICOMPILABLE_H_
#define ICOMPILABLE_H_

#include <string>
#include <vector>
#include <memory>
using namespace std;

namespace pgen {
/**
 * This class defines an interface of anything that can be compiled, such as
 * Ranges, Classes, Groups and even entire expressions.
 */
class ICompilable {
public:
	/**
	 * An empty list of dependences to be returned by default on the dependences() method.
	 */
	static vector<ICompilable*> _dependences;

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
	virtual string ccompile() = 0;
	/**
	 * Returns C code that can parse the regular expression.
	 * the returned code shuld be a function with the following prototype:
	 * int expr_name(char *text);
	 * The function must return the numbers of consumed bytes on success or -1 on failure.
	 * This version of the function is not chainable.
	 * @return the compiled C code that parses the expression.
	 */
	virtual string compile() = 0;
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
	virtual string name() = 0;
	/**
	 * Returns the function name of the non-chained version of the compiled code.
	 */
	virtual string funcname();
	/**
	 * Returns the function name of the chained version of the compiled code.
	 */
	virtual string cfuncname();
	/**
	 * Returns the prototype of the non-chained version of the function for the
	 * compiled code.
	 * @return The prototype of the non-chained version of the function
	 * (i.e. the signature of the non-chained function).
	 */
	virtual string prototype();
	/**
	 * Returns the prototype for the chained version of the function for the compiled
	 * code.
	 * @return The prototype of the chained version of the function for the compiled
	 * code.
	 */
	virtual string cprototype();
	/**
	 * Points to the implementation of the method of the subclass.
	 * The method does nothing. It serves only as a pointer to identify the class.
	 */
	virtual int type() = 0;

	virtual ~ICompilable();

};
} /* namespace pagen */

#endif /* ICOMPILABLE_H_ */
