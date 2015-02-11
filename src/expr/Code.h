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
 * \date 2014-07-07
 * \file
 * This is the most different class from this directory. Its main function is to provide a way to compile many regular 
 * expressions into a single file, while removing the redundant expressions. That is, the code for a single expression 
 * will not be added twice to the compiled file.
 * It also provides the helper code that is needed by many of the compiled functions.
 */

#ifndef CODE_H_
#define CODE_H_

// STL
#include <string>
#include <sstream>
#include <unordered_set>
// pgen
#include "ICompilable.h"

using namespace std;
namespace pgen 
{

	class Code 
	{
	private:
		unordered_set<string> methodList;
		unordered_set<string> prototypeList;
		static const string helperCode;
		stringstream _code;
	protected:

	public:
		Code();

		/**
		 * Adds the corresponding C++ source code of a regular expression to the code repository.
		 * This function must be used when compiling multiple regular expressions to the same file
		 * to avoid redundancy. i.e. if there is code already in the repository for any part of an
		 * expression, it will be reused, and no duplicate methods will exist.
		 * \author Dimas Melo Filho
		 * \param obj the ICompilable object to add to the Lexicon.
		 * \remark This method adds the NON-CHAINED version of the matching function
		 */
		void add(ICompilable* obj);	
		
		/**
		 * Adds the chained version of the corresponding C++ source code of a regular expression 
		 * to the code repository. This function must be used when compiling multiple regular
		 * expressions to the same file to avoid redundancy. i.e. if there is code already in the
		 * repository for any part of an expression, it will be reused, and no duplicate methods 
		 * will exist.
		 * \author Dimas Melo Filho
		 * \param obj the ICompilable object to add to the Lexicon.
		 * \remark This method adds the CHAINED version of the matching function.
		 */
		void cadd(ICompilable* obj);

		/**
		 * Gets the full code of the lexicon, except for the main method.
		 * \author Dimas Melo Filho
		 * \return All the functions needed to parse all added regular expressions, except for
		 * a main function. The main must be provided by another class.
		 */
		const string code();

		/**
		 * Returns all the C++ helper code needed by the regex code.
		 * \author Dimas Melo Filho
		 * \return C++ helper code that is used by the regex C++ code.
		 */
		const string & helper();
		static const string & getHelper();
	}; /* class Code */
} /* namespace pgen */

#endif /* CODE_H_ */
