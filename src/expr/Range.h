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
 * \date 2014-06-13
 * \file
 * The Range class represents a single range of characters, e.g. A-Z. This class is not used directly. It is currently 
 * used by the CharClass to deal with character range definitions inside the character class definition.
 */

#ifndef RANGE_H_
#define RANGE_H_

#include "ICompilable.h"

namespace pgen 
{
	/**
	 * This class represents a range of characters, i.e. from A to Z.
	 * It has methods that check if a given char is in, before or after the interval.
	 * It also provide mechanisms to join two ranges when possible, and to compare two ranges.
	 * Both the start and end fields are public, so that they can be changed without the
	 * overhead of calling a setter method. This class should be treated as a storage class
	 * with helper methods.
	 */
	class Range
	 : public ICompilable 
	{
	public:
		unsigned int start;			//< First character of the interval
		unsigned int end;			//< Last character of the interval
		Range * next;				//< Pointer to the next interval on the list
		Range * prev;				//< Pointer to the previous interval on the list

		/**
		 * Checks if a given character is in the interval limits.
		 * \param c character to check
		 * \return True if the character c is >= start AND <= end.
		 */
		bool in(unsigned int c);
		/**
		 * Checks if a given character is before (smaller than) the lowest bound of the range.
		 * \param c character to check
		 * \return True if the character c is < start
		 */
		bool before(unsigned int c);
		/**
		 * Checks if a given character is after (greater than) the highest bound of the range.
		 * \param c character to check
		 * \return True if the character c is > start
		 */
		bool after(unsigned int c);
		/**
		 * Join a single character with the current range.
		 * \param c the character to join the range.
		 * \return true if it was possible to join the provided character with the current range.
		 * in some cases it is not possible to join them. For instance, when the character is
		 * disjoint with the range they can't be represented by a single interval.
		 */
		bool join(unsigned int c);
		/**
		 * Join a range of characters with the current range.
		 * \param s first character of the range to join (inclusive).
		 * \param e ending character of the range to join (inclusive)
		 * \return true if it was possible to join the provided range with the current range.
		 * in some cases it is not possible to join them. For instance, when the ranges are
		 * disjoint they can't be represented by a single interval.
		 */
		bool join(unsigned int s, unsigned int e);
		/**
		 * Join a range of characters with the current range.
		 * \param i a reference to a Range object containing the interval to join with this range.
		 * \return true if it was possible to join the provided range with the current range.
		 * in some cases it is not possible to join them. For instance, when the ranges are
		 * disjoint they can't be represented by a single interval.
		 */
		bool join(const Range & r);
		/**
		 * Join a range of characters with the current range.
		 * \param i a pointer to a Range object containing the interval to join with this range.
		 * \return true if it was possible to join the provided range with the current range.
		 * in some cases it is not possible to join them. For instance, when the ranges are
		 * disjoint they can't be represented by a single interval.
		 */
		bool join(Range * r);
		/**
		 * Check if two Ranges are equal. The ranges are equal if and only if their starting
		 * and ending characters are the same.
		 * \param r a reference to a const Range object to compare with.
		 * \return true when they are equal, or false if they are different.
		 */
		bool operator==(const Range & r) const;
		/**
		 * Check if two Ranges are different. The ranges are different if and only their starting
		 * OR their ending characters are different.
		 * \param r a reference to a const Range object to compare with.
		 * \return true when they are different, or false if they are equal.
		 */
		bool operator!=(const Range & r) const;
		/**
		 * Creates a range with the given starting and ending bounds.
		 * If the ending bound is smaller than the starting bound, they are reversed during the
		 * creation.
		 * If the ending and starting bounds are equal, the interval is equivalent to a single
		 * character.
		 * \param start Starting bound (usually the lower bound, ie. the char of smallest value).
		 * \param end Ending bound (usually the higher bound, ie. the char of greatest value).
		 */
		Range(unsigned int start, unsigned int end);
		virtual ~Range();

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
		 * Returns the name of the class. The returned name must be usable as a function
		 * name in the standard C (C99). i.e. it has to match the following format:
		 * [a-zA-Z][A-Za-z0-9_]*
		 */
		virtual string name();
		/**
		 * \returns the type identifier of this class.
		 */
		virtual int type();
		
		#ifdef _DEBUG
		virtual void print(int level = 0);
		#endif

		static const int TYPE = 5;
	}; /* class Range */
} /* namespace pagen */

#endif /* RANGE_H_ */
