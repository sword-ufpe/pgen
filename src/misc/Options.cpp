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
 * This file contains the implementation of the Options Class. The Options class is responsible for parsing the command
 * line options and displaying help information to the user.
 */
 
// STL
#include <unistd.h>
#include <string>
#include <iostream>
// pgen
#include "Options.h"

using namespace std;
namespace pgen 
{
	struct option Options::long_options[] = {
		{"help",	no_argument,		0,	'h'},
		{"input",	required_argument,	0,	'i'},
		{"version",	no_argument,		0,	0},
		{"warranty",no_argument,		0,	0},
		{"output",  required_argument,	0,	'o'},
		{"defs",	required_argument,	0,	'd'},
		{"symbol-name", no_argument,	0,	'n'},
		{0,			0,					0,	0}
	};
	
	Options::Options(int argc, char* argv[])
	 : inputFileName(nullptr)
	 , outputFileName(nullptr)
	 , definitionFileName(nullptr)
	 , writeGetSymbolNameMethod(false)
	 , valid(true)
	{
		int c;
		int opt_index;
		while ((c = getopt_long(argc, argv, "i:ho:d:n", long_options, &opt_index)) != -1) 
		{
			switch (c) 
			{
			case 0:
				switch (opt_index) 
				{
				case 2: // version
					this->printVersion();
					return;
				case 3: // warranty
					this->printWarranty();
					return;
				}
				break;
			case 'i': // input
				this->inputFileName = new string(optarg);
				break;
			case 'h': // help
				this->printHelp();
				return;
			case 'o': // output
				this->outputFileName = new string(optarg);
				break;
			case 'd': // definition file (header)
				this->definitionFileName = new string(optarg);
				break;
			case 'n': // write the function "getSymbolName" that returns a string for each name.
				this->writeGetSymbolNameMethod = true;
				break;				
			case '?': // something is wrong
				if (optopt == 'i') 
				{
					cout << "The --input option requires a parameter (the input file name)." << endl;
				} 
				else 
				{
					cout << "Unknown option '" << optopt << "'." << endl;
				}
				this->printHelp();
				return;
			}
		}
		if (this->inputFileName == nullptr || this->outputFileName == nullptr)
		{
			this->printHelp();
		}
	}

	Options::~Options() 
	{
		if (inputFileName != NULL) 
		{ 
			delete inputFileName;
		}
	}

	void Options::printHelp() 
	{
		cout << 
		"Usage: pgen -i <input_file> -o <output_prefix> [options]\n"
		"Options:\n"
		"  -h, --help              display this help information.\n"
		"  -i, --input=FILENAME    use the provided input file.\n"
		"  -o, --output=PREFIX     write the generated code PREFIX.c and PREFIX.h.\n"
		"  -d, --defs=FILENAME	   write the definitions to this file.\n"
		"  -n, --symbol-name       write the getSymbolName method.\n"
		"      --version           display the program's version information.\n\n";		
		this->valid = false;
	}
	
	void Options::printVersion()
	{
		cout << 
		"pgen 0.1.0\n"
		"Copyright (C) 2015 Dimas Melo Filho.\n"
		"pgen comes with ABSOLUTELY NO WARRANTY; for details\n"
		"run pgen --warranty. This is free software, and you are\n"
		"welcome to redistribute it under certain conditions;\n"
		"check <http://www.gnu.org/licenses/gpl-2.0.html> for details.\n\n";
		this->valid = false;
	}
	
	void Options::printWarranty()
	{
		cout <<
		"BECAUSE THE PROGRAM IS LICENSED FREE OF CHARGE, THERE IS NO\n"
		"WARRANTY FOR THE PROGRAM, TO THE EXTENT PERMITTED BY\n"
		"APPLICABLE LAW. EXCEPT WHEN OTHERWISE STATED IN WRITING THE\n"
		"COPYRIGHT HOLDERS AND/OR OTHER PARTIES PROVIDE THE PROGRAM\n"
		"\"AS IS\" WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESSED OR\n"
		"IMPLIED, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED\n"
		"WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR\n"
		"PURPOSE. THE ENTIRE RISK AS TO THE QUALITY AND PERFORMANCE\n"
		"OF THE PROGRAM IS WITH YOU. SHOULD THE PROGRAM PROVE\n"
		"DEFECTIVE, YOU ASSUME THE COST OF ALL NECESSARY SERVICING,\n"
		"REPAIR OR CORRECTION.\n\n"
		"IN NO EVENT UNLESS REQUIRED BY APPLICABLE LAW OR AGREED TO\n"
		"IN WRITING WILL ANY COPYRIGHT HOLDER, OR ANY OTHER PARTY\n"
		"WHO MAY MODIFY AND/OR REDISTRIBUTE THE PROGRAM AS PERMITTED\n"
		"ABOVE, BE LIABLE TO YOU FOR DAMAGES, INCLUDING ANY GENERAL,\n"
		"SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES ARISING OUT OF\n"
		"THE USE OR INABILITY TO USE THE PROGRAM (INCLUDING BUT NOT\n"
		"LIMITED TO LOSS OF DATA OR DATA BEING RENDERED INACCURATE\n"
		"OR LOSSES SUSTAINED BY YOU OR THIRD PARTIES OR A FAILURE OF\n"
		"THE PROGRAM TO OPERATE WITH ANY OTHER PROGRAMS), EVEN IF\n"
		"SUCH HOLDER OR OTHER PARTY HAS BEEN ADVISED OF THE\n"
		"POSSIBILITY OF SUCH DAMAGES.\n\n"
		"Check <http://www.gnu.org/licenses/gpl-2.0.html> for details.\n\n";
		this->valid = false;
	}

} /* namespace pgen */
