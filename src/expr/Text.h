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
 * \date 2014-10-04
 * \file
 * The Text class was built as an optimization to replace Sequences of Characters on Regular Expressions. For instance, 
 * consider the following Regular expression: 'abcd'. It could be represented by a Sequence of Characters as follows:
 *  - Sequence(Character('a'), Character('b'), Character('c'), Character('d'))
 * 
 * However, it could be more efficiently treated as a text string as follows:
 *  - Text("abcd")
 * 
 * It is also faster to match for strings than Sequences of characters in a row in the C99 language.
 */

#ifndef TEXT_H_
#define TEXT_H_

// STL
#include <string>
// pgen
#include "ICompilable.h"

using namespace std;
namespace pgen 
{
	class Text
	 : public ICompilable 
	{
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
		 * \return The text "chr_" + the text corresponding to the integer value of the character.
		 */
		virtual string name();
		/**
		 * \returns the type identifier of this class.
		 */
		virtual int type();
		
		#ifdef _DEBUG
		virtual void print(int level = 0);
		#endif

		static const int TYPE = 7;
	}; /* class Text */
} /* namespace pagen */

#endif /* TEXT_H_ */
