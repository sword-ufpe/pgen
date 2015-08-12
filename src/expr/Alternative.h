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
 * \date 2014-06-30
 * \file
 * The Alternative class represents a list of expressions that can be accepted by the parser. On the Regular Expression 
 * it is represented by the '|' character.
 */

#ifndef ALTERNATIVE_H_
#define ALTERNATIVE_H_

// STL
#include <string>
#include <vector>
#include <memory>
// pgen
#include "ICompilable.h"

using namespace std;
namespace pgen 
{

	class Alternative 
	 : public ICompilable 
	{
	public:
		vector<ICompilable*> expr;		//< Alternative expressions.

		Alternative();
		virtual ~Alternative();

		/**
		 * Adds an ICompilable expression as an alternative.
		 * \param expression a shared_ptr ICompilable object.
		 * \remark the first expression to be tried out should be added first (FIFO).
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
		 * Returns a vector<ICompilable> of dependences in case of using the chained version
		 * of the algorithm. It can also return NULL when there are no dependences.
		 * \return A vector of ICompilable dependences of the code.
		 */
		virtual vector<ICompilable*> & dependences();
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
		
		#ifdef _DEBUG
		virtual void print(int level = 0);
		#endif

		static const int TYPE = 1;
	}; /* class Alternative */
} /* namespace pagen */

#endif /* ALTERNATIVE_H_ */
