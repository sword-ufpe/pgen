/*
 * Options.h
 *
 *  Created on: Dec 4, 2014
 *      Author: Dimas Melo Filho
 */

#ifndef MISC_OPTIONS_H_
#define MISC_OPTIONS_H_

#include <string>

namespace pgen {

class Options {
public:
	std::string* inputFileName;
	bool valid;
	Options(int argc, char* argv[]);
	virtual ~Options();

	void printHelp();
};

} /* namespace pagen */

#endif /* MISC_OPTIONS_H_ */
