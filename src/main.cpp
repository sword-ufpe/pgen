/*
 * main.cpp
 *
 *  Created on: Dec 4, 2014
 *      Author: Dimas Melo Filho
 */

#include "misc/Options.h"
#include "parser/Parser.h"

int main(int argc, char* argv[]) {
	pagen::Options options(argc, argv);
	if (!options.valid) return 1;
	pagen::Parser parser(options);
	return 0;
}

