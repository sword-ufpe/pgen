/**
 * \author Dimas Melo Filho
 * \date 2015-01-07
 * \file
 * This file contains the implementation of the LLStarRule class. This class handles a single LL(*) grammar rule. It is
 * able to generate C99 to match to an array of tokens according to a grammar rule.
 */

// STL
#include <sstream>
#include <iostream>
// pgen
#include "../misc/Util.h"
#include "../misc/LanguageException.h"
#include "LLStarRule.h"
 
using namespace std;
namespace pgen 
{

	LLStarRule::~LLStarRule() 
	{
	}

	string LLStarRule::compile() 
	{
		stringstream s;
		string _funcName = this->funcname();
		int alternativeNumber = 0;
		s << this->prototype() << " {"																"\n"
			 " int opos = *pos;"																	"\n"
			 " ast_node* subtree = ast_new_node();"													"\n"
			 " ast_node* child = NULL;"																"\n"
			 " subtree->tokenId = " << language->getNonTerminalId(name) << ";"						"\n";
		for (vector<int>* option: symbols)
		{
			for (unsigned int i = 0; i < option->size(); i++)
			{
				int symbol = option->at(i);
				if (symbol < 1000000000) {
					s << " if (tokens->items[*pos].type != " << symbol << ") "						"\n"
						 "  goto " << _funcName << "_" << alternativeNumber << ";\n"
						 " child = ast_new_node();"													"\n"
						 " child->tokenId = " << symbol << ";"										"\n"
						 " child->data = tokens->items[*pos].value;"								"\n"
						 " ast_add_child(subtree, child);"											"\n"
						 " ++(*pos);"																"\n";
				}
				else
				{
					s << " if ((child = " << language->grammar->rules[symbol-1000000000]->funcname() << "(tokens, pos)) == NULL) \n"
						 "  goto " << _funcName << "_" << alternativeNumber << ";"					"\n"
						 " ast_add_child(subtree, child);"											"\n";
				}
			}
			// TODO: remove the printf and put the code that must run when matched
			s << " return subtree;"																	"\n" <<
				 _funcName << "_" << alternativeNumber++ << 										":\n"
				 " *pos = opos;"																	"\n"
				 " ast_clear(subtree);"																"\n";
		}
		s << " free(subtree);"																		"\n"
			 " return NULL;"																		"\n"
			 "}"																					"\n\n";
		return s.str();
	}

}; /* namespace pgen */