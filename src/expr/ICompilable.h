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
 * \date 2014-06-16
 * \file
 * This class is the base class for all classes that can be compiled to C99 code. All classes in this directory inherit 
 * from ICompilable, except for the "Code" class, which is a special class.
 */

#ifndef ICOMPILABLE_H_
#define ICOMPILABLE_H_

// STL
#include <string>
#include <vector>
#include <memory>

using namespace std;
namespace pgen 
{
	/**
	 * This class defines an interface of anything that can be compiled, such as
	 * Ranges, Classes, Groups and even entire expressions.
	 */
	class ICompilable 
	{
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
		 * \returns the chainable compiled C code that parses the expression.
		 */
		virtual string ccompile() = 0;
		/**
		 * Returns C code that can parse the regular expression.
		 * the returned code shuld be a function with the following prototype:
		 * int expr_name(char *text);
		 * The function must return the numbers of consumed bytes on success or -1 on failure.
		 * This version of the function is not chainable.
		 * \returns the compiled C code that parses the expression.
		 */
		virtual string compile() = 0;
		/**
		 * Returns a vector<ICompilable> of dependences in case of using the chained version
		 * of the algorithm. It can also return NULL when there are no dependences.
		 * \returns A vector of ICompilable dependences of the code.
		 */
		virtual vector<ICompilable*> & dependences();
		/**
		 * Returns the name of the expression. The returned name must be usable as a function
		 * name in the standard C (C99). i.e. it has to match the following format:
		 * [a-zA-Z][A-Za-z0-9_]*
		 * \returns The name of the expression (usable as function names).
		 */
		virtual string name() = 0;
		/**
		 * \returns the function name of the non-chained version of the compiled code.
		 */
		virtual string funcname();
		/**
		 * \returns the function name of the chained version of the compiled code.
		 */
		virtual string cfuncname();
		/**
		 * \returns The prototype of the non-chained version of the function
		 * (i.e. the signature of the non-chained function).
		 */
		virtual string prototype();
		/**
		 * \returns The prototype of the chained version of the function for the compiled
		 * code.
		 */
		virtual string cprototype();
		/**
		 * \returns the type identifier of this class.
		 */
		virtual int type() = 0;

		virtual ~ICompilable();
		
		#ifdef _DEBUG
		virtual void print(int level = 0) = 0;
		#endif

	}; /* class ICompilable */
} /* namespace pgen */

#endif /* ICOMPILABLE_H_ */
