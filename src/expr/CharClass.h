/**
 * pgen, Parser Generator.
 * Copyright (C) 2015 Dimas Melo Filho
 * 
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA. 
 * 
 * The author can be reached by e-mail: dldmf@cin.ufpe.br.
 * 
 * \author Dimas Melo Filho <dldmf@cin.ufpe.br>
 * \date 2014-07-09
 * \file
 * This file declares an ICompilable that handles all the logic behind a Regular Expression Class of Characters.
 * It provides methods both for parsing regular expressions and for generating C code.
 * The CharClass represents a class of characters that can be matched on a position of the Regular Expression. For 
 * instance, if someone uses the following regular expression '[0-9A-F]', it will be encoded using this class. The 
 * CharClass is also able to parse classes that contain other named classes such as '[:alpha:]'. If the named class is 
 * not a standard POSIX class, it must be declared on the YAML file first.
 */
 
namespace pgen
{
	class CharClass;
}

#ifndef CHARCLASS_H_
#define CHARCLASS_H_

// STL
#include <string>
// pgen
#include "Range.h"
#include "ICompilable.h"
#include "../parser/NamedClassManager.h"

using namespace std;
namespace pgen {
	class CharClass 
	 : public ICompilable {
	private:
		Range * range;
		bool negated;
	public:
		/**
		 * \brief returns a CharClass that matches all characters
		 * \return a CharClass pointer that matches all characters.
		 */
		static CharClass* dot();
		/**
		 * Parses the regular expression definition of the name of a class inside a class definition. i.e.
		 * When something like [:alpha:] is found, it parses the name :alpha: and returns only 'alpha'.
		 * \param expression the regular expression to parse
		 * \param pos the position to start parsing.
		 * \return string the parsed name of a regex class.
		 */
		static string parseClassName(string &expression, unsigned int& pos);
		/**
		 * Parses a regular expression class definiton.
		 * \param expression the regular expression
		 * \param pos the index of the next character to read on the regular expression
		 * \return CharClass the parsed class.
		 */
		static CharClass* parse(string& expression, unsigned int& pos, NamedClassManager& ncm);
		/**
		 * Adds a character to the interval if it does not belong to the interval yet.
		 * \param c unicode character to add to the interval.
		 * \remark if two intervals need to be joined/merged, they will be.
		 */
		void add(unsigned int c);
		/**
		 * Adds an interval of characters to the interval list. Only the characters which are not
		 * part of any intervals are added.
		 * \param cstart first character of the interval to add
		 * \param cend last character of the interval to add
		 * \remark if the interval intesercs with other intervals, they will be merged together accordingly
		 * \remark if two intervals need to be joined/merged, they will be.
		 */
		void add(unsigned int cstart, unsigned int cend);
		/**
		 * Adds an entire Class of Characters to this CharClass.
		 * @param c the reference to the charclass to add
		 * \remark if the interval intesercs with other intervals, they will be merged together accordingly
		 * \remark if two intervals need to be joined/merged, they will be.
		 */
		void add(const CharClass& c);
		/**
		 * Adds an entire Class of Characters to this CharClass.
		 * \param c the pointer to the charclass to add
		 * \remark if the interval intesercs with other intervals, they will be merged together accordingly
		 * \remark if two intervals need to be joined/merged, they will be.
		 */
		void add(CharClass* c);
		/**
		 * Merges(joins) all the intervals that can be merged, starting from the provided interval.
		 * i.e. If the interval intersects with the next interval, they are joined and the current
		 * interval is deleted. This process is repeated until no intervals need to be joined
		 * forward or backwards.
		 * \param v Interval to join with others.
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
		 * \remark In order to properly compare a negated class with a non-negated one, the negated class needs to be normalized first.
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
		 * \return the chainable compiled C code that parses the expression.
		 */
		virtual string ccompile();
		/**
		 * Returns C code that can parse the regular expression.
		 * the returned code shuld be a function with the following prototype:
		 * int expr_name(char *text);
		 * The function must return the numbers of consumed bytes on success or -1 on failure.
		 * This version of the function is not chainable.
		 * \return the compiled C code that parses the expression.
		 */
		virtual string compile();
		/**
		 * Returns the name of the expression. The returned name must be usable as a function
		 * name in the standard C (C99). i.e. it has to match the following format:
		 * [a-zA-Z][A-Za-z0-9_]*
		 * \return The name of the expression (usable as function names).
		 */
		virtual string name();
		/**
		 * \returns the type identifier of this class.
		 */
		virtual int type();

		static const int TYPE = 3;

		CharClass();
		CharClass(const CharClass& c);
		virtual ~CharClass();
		
		#ifdef _DEBUG
		virtual void print(int level = 0);
		#endif
	};

} /* namespace pgen */

#endif /* CHARCLASS_H_ */
