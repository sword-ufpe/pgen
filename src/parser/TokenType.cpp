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
 * \author Dimas Melo Filho
 * \date 2015-01-06
 * \tabsize 4
 * \file
 * This file contains the implementation of the TokenType Class. The TokenType is a
 * class that is responsible for handling a type of token. The types of tokens are
 * defined by the language description file. All TokenType objects must be managed
 * and owned by the TokenTypeManager.
 */

// STL
#include <string>
#include <vector>
#include <sstream>
// pgen
#include "TokenType.h"

using namespace std; 
namespace pgen 
{

	TokenType::TokenType(Language* language, string expression, string name, int typeId, bool discard, int setState, 
		int* validStates) 
	  : regex(expression, language->ncm)
	  , name(name)
	  , discard(discard)
	  , typeId(typeId)
	  , setState(setState)
	  , validStates(validStates)
	  , language(language)
	{
	}

	TokenType::~TokenType() 
	{
		if (validStates != nullptr) 
		{
			delete validStates;
		}
	}

	void TokenType::matchCode(string tabs, stringstream& s) 
	{
		s << tabs << "*pos = " << regex.funcname() << "(text);" 							"\n"
		  << tabs << "if (*pos != -1) ";
		if (setState != -1) 
		{
			// Match and set a new state
			s << "{" 																		"\n"
			  << tabs << " " << language->prefix << "state = " << setState << ";"			"\n"
			  << tabs << " return " << (discard? -2 : typeId) << ";" 						"\n"
			  << tabs << "}"																"\n";
		} 
		else 
		{
			// Only match, doesn't make any changes to the state
			s << "return " << (discard? -2 : typeId) << ";"									"\n";
		}
	}

	string TokenType::code() 
	{
		stringstream s;
		if (validStates != nullptr) 
		{
			// If the token is valid only on specific states, check states first
			s << " if ("
			  << language->prefix << "state == " << validStates[0];
			int i = 1;
			while (validStates[i] != -1) 
			{
				s << " || " 											"\n"
					 "     " << language->prefix << "state == " << validStates[i++];
			}
			s << ")" 													"\n"
				 " {"													"\n";
			// And then match the token
			matchCode("  ", s);
			s << " }" << endl;
		} 
		else 
		{
			// If the token doesn't specify any valid states, match the code no matter what stat it is in
			matchCode(" ", s);
		}
		return s.str();
	}

}; /* namespace pgen */
