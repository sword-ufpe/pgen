/*
 * Class.h
 *
 *  Created on: Jun 9, 2014
 *      Author: Dimas Melo Filho
 */

#ifndef CHARCLASS_H_
#define CHARCLASS_H_

#include "Range.h"
#include "ICompilable.h"
#include "../test/Test.h"
#include <string>
using namespace std;

namespace pgen {
	class CharClass : public ICompilable {
	private:
		Range * range;
		bool negated;
	public:
		/**
		 * Adds a character to the interval if it does not belong to the interval yet.
		 * @param c unicode character to add to the interval.
		 * @remark if two intervals need to be joined/merged, they will be.
		 */
		void add(unsigned int c);
		/**
		 * Adds an interval of characters to the interval list. Only the characters which are not
		 * part of any intervals are added.
		 * @param cstart first character of the interval to add
		 * @param cend last character of the interval to add
		 * @remark if the interval intesercs with other intervals, they will be merged together accordingly
		 * @remark if two intervals need to be joined/merged, they will be.
		 */
		void add(unsigned int cstart, unsigned int cend);
		/**
		 * Adds an entire Class of Characters to this CharClass.
		 * @param c the pointer to the charclass to add
		 * @remark if the interval intesercs with other intervals, they will be merged together accordingly
		 * @remark if two intervals need to be joined/merged, they will be.
		 */
		void add(CharClass* c);
		/**
		 * Merges(joins) all the intervals that can be merged, starting from the provided interval.
		 * i.e. If the interval intersects with the next interval, they are joined and the current
		 * interval is deleted. This process is repeated until no intervals need to be joined
		 * forward or backwards.
		 * @param v Interval to join with others.
		 */
		void merge(Range * v);
		/**
		 * Inverts the accepted character intervals of a class (i.e. negates the accepted ranges).
		 */
		void invert();
		/**
		 * Normalizes a class, i.e. if it is negated, it gets inverted and non-negated.
		 */
		void normalize();
		/**
		 * Compares two classes. If they aren't both negated or non-negated they are considered
		 * different without comparing the interval. If they are either both negated or both
		 * non-negated then their intervals are compared.
		 * @remark In order to properly compare a negated class with a non-negated one, the negated class needs to be normalized first.
		 */
		bool operator==(const CharClass & c) const;
		/**
		 * Returns the oposite of the == operator. See the == operator for more information.
		 */
		bool operator!=(const CharClass & c) const;

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
		 * @return The name of the expression (usable as function names).
		 */
		virtual string name();
		/**
		 * Points to the implementation of the method of the subclass.
		 * The method does nothing. It serves only as a pointer to identify the class.
		 */
		virtual int type();

		static const int TYPE = 3;

		CharClass();
		virtual ~CharClass();
	};

} /* namespace pagen */

#endif /* CHARCLASS_H_ */
