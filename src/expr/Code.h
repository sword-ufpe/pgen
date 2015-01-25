/*
 * Code.h
 *
 *  Created on: Jun 7, 2014
 *      Author: Dimas Melo Filho
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
