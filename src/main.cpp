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
 * \date 2015-02-04
 * \file
 * This is the file that contains the main method of the program.
 */

// STL
#include <iostream>
// yaml-cpp
#include <yaml-cpp/yaml.h>
// pgen
#include "misc/Options.h"
#include "parser/Language.h"
#include "misc/LanguageException.h"

int main(int argc, char* argv[]) 
{
	pgen::Language language;
	pgen::Options options(argc, argv);
	if (!options.valid)
	{
		return 1;
	}
	try
	{
		language.load(*options.inputFileName);
	}
	catch (const pgen::LanguageException& ex)
	{
		cout << "LanguageException: " << ex.what() << endl;
		return 1;
	}
	cout << language.compile();
	return 0;
}

