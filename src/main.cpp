/*
 * main.cpp
 *
 *  Created on: Dec 4, 2014
 *      Author: Dimas Melo Filho
 */

#include "misc/Options.h"

int main(int argc, char* argv[]) {
	pgen::Options options(argc, argv);
	if (!options.valid) return 1;
	return 0;
}

