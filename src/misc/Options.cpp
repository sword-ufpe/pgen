/*
 * Options.cpp
 *
 *  Created on: Dec 4, 2014
 *      Author: Dimas Melo Filho
 */

#include "Options.h"
#include <unistd.h>
#include <string>
#include <iostream>

using namespace std;

namespace pagen {

Options::Options(int argc, char* argv[]):
inputFileName(NULL), valid(true)
{
	int c;
	while ((c = getopt(argc, argv, "i:")) != -1) {
		switch (c) {
		case 'i':
			this->inputFileName = new string(optarg);
			break;
		case '?':
			if (optopt == 'i') {
				cout << "The -i option requires a parameter (the input file name)." << endl;
			} else {
				cout << "Unknown option '" << optopt << "'." << endl;
			}
			this->printHelp();
		}
	}
}

Options::~Options() {
	if (inputFileName != NULL) delete inputFileName;
}

void Options::printHelp() {
	cout << "Use: pagen -i input_file_name" << endl;
	this->valid = false;
}

} /* namespace pagen */
