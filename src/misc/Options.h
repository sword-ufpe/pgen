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
 * This file contains the definition of the Options class, which is responsible for parsing command line options and
 * displaying help information to the user.
 */

#ifndef OPTIONS_H_
#define OPTIONS_H_

#include <getopt.h>
#include <string>

namespace pgen 
{
	class Options 
	{
	private:
	static struct option long_options[];
	public:
		std::string* inputFileName;
		std::string* outputFileName;
		std::string* definitionFileName;
		bool writeGetSymbolNameMethod;
		bool valid;
		Options(int argc, char* argv[]);
		virtual ~Options();

		void printHelp();
		void printVersion();
		void printWarranty();
	}; /* class Options */
} /* namespace pgen */

#endif /* OPTIONS_H_ */
