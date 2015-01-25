/*
 * Options.h
 *
 *  Created on: Dec 4, 2014
 *      Author: Dimas Melo Filho
 */

#ifndef OPTIONS_H_
#define OPTIONS_H_

#include <string>

namespace pgen 
{
	class Options 
	{
	public:
		std::string* inputFileName;
		bool valid;
		Options(int argc, char* argv[]);
		virtual ~Options();

		void printHelp();
	}; /* class Options */
} /* namespace pgen */

#endif /* OPTIONS_H_ */
