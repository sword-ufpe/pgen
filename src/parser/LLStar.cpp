/**
 * \author Dimas Melo Filho
 * \date 2015-01-07
 * \file
 * This file contains the implementation of the LLStar class. This class is responsible for generating a LL(*) parser 
 * in a recursive descent way, for parsing a specified language. The class requires access to the Language and its 
 * tokens in order to generate code for parsing it.
 * These parsers don't need to parse text at all. They parse a list of tokens.
 */
 
// STL
#include <sstream>
#include <string>
// pgen
#include "LLStar.h"
#include "LLStarRule.h"
 
using namespace std;
namespace pgen 
{

	const string LLStar::NAME = "LL(*)";

	LLStar::~LLStar() 
	{
	}

	void LLStar::addRule(string& name, YAML::Node &ruleNode) 
	{
		rules.push_back(new LLStarRule(language, name, ruleNode));
	}

	string LLStar::compile() 
	{
		stringstream s;
		for (auto rule: rules) 
		{
			s << rule->compile();
		}
		s <<
			"ast_node* " << language->prefix << "parse(token_list* tokens, int* pos) {"		"\n"
			" return ast_invert_siblings(" << rules[language->startRule-1000000000]->funcname() << "(tokens, pos), NULL);\n"
			"}"																				"\n\n";
		return s.str();
	}
	
}; /* namespace pgen */