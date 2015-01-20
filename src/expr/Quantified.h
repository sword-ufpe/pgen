/*
 * Quantified.h
 *
 *  Created on: Jun 18, 2014
 *      Author: Dimas Melo Filho
 */

#ifndef QUANTIFIED_H_
#define QUANTIFIED_H_

#include "../test/Test.h"
#include "ICompilable.h"
#include <memory>
#include <string>
#include <vector>
#include <climits>
using namespace std;

namespace pagen {

/**
 * Represents a quantified expression. The quantified expression is an expression which
 * enforces a number of occurrences between a defined minimum and maximum. It requires a
 * compilable expression and it is compilable itself.
 * The quantified expression can be either greedy or lazy. If it is greedy it will try to
 * match the most number of characters at first. If it is lazy it will try to match the least
 * number of characters possible at first. In the common regular expression syntax all
 * quantifiers are greedy by default. They become lazy if the user adds a '?' character after
 * the quantifier.
 */
class Quantified : public ICompilable {
private:
	unsigned int min;					//< Minimum occurrence
	unsigned int max;					//< Maximum occurrence
	vector<ICompilable*> deps; 			//< List of dependences
	ICompilable* expr;					//< Any ICompilable (classes, groups, expressions, etc).
public:
	bool greedy;						//< Greedy or lazy quantifier?
	/**
	 * Used to refer to an infinite amount.
	 */
	static const unsigned int INF = UINT_MAX;
	/**
	 * Constructor. Requires an ICompilable expression, the minimum and maximum occurrence.
	 * @param pExpr an ICompilable expression to quantify.
	 * @param min the minimum accepted occurrence for the expression.
	 * @param max the maximum accepted occurrence for the expression.
	 * @remark when using this constructor, the quantified expression is treated as a greedy
	 * expression by default.
	 */
	Quantified(ICompilable* pExpr, unsigned int min, unsigned int max);
	/**
	 * Constructor. Requires an ICompilable expression, the minimum and maximum occurrence.
	 * @param pExpr an ICompilable expression to quantify.
	 * @param min the minimum accepted occurrence for the expression.
	 * @param max the maximum accepted occurrence for the expression.
	 * @param greedy tells if the quantified expression should be greedy or not.
	 */
	Quantified(ICompilable* pExpr, unsigned int min, unsigned int max, bool greedy);
	virtual ~Quantified();

	/**
	 * Returns C code that can parse the greedy quantified expression.
	 * The returned code should be a function with the following prototype:
	 * int expr_name(char *text, int (*chain[])());
	 * @return the compiled C code that parses the a Greedy version of the Quantified Expression.
	 */
	string ccompile_greedy();
	/**
	 * Returns C code that can parse the greedy quantified expression.
	 * The returned code should be a function with the following prototype:
	 * int expr_name(char *text);
	 * @return the compiled C code that parses the a Greedy version of the Quantified Expression.
	 */
	string compile_greedy();
	/**
	 * Returns C code that can parse the lazy quantified expression.
	 * The returned code should be a function with the following prototype:
	 * int expr_name(char *text, int (*chain[])());
	 * @return the compiled C code that parses the a Lazy version of the Quantified Expression.
	 */
	string ccompile_lazy();
	/**
	 * Returns C code that can parse the lazy quantified expression.
	 * The returned code should be a function with the following prototype:
	 * int expr_name(char *text);
	 * @return the compiled C code that parses the a Lazy version of the Quantified Expression.
	 */
	string compile_lazy();

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
	 * @return The name of the expression (usable as function names).
	 */
	virtual string name();
	/**
	 * Points to the implementation of the method of the subclass.
	 * The method does nothing. It serves only as a pointer to identify the class.
	 */
	virtual int type();
	static const int TYPE = 4;

};

} /* namespace pagen */

#endif /* QUANTIFIED_H_ */
