/**
 * \author Dimas Melo Filho
 * \date 2015-01-22
 * \file
 * This file contains the declaration of the IGrammarRule class. This class behaves like an extended interface, which
 * defines the methods that all grammar rule types must implement.
 */

#ifndef IGRAMMARRULE_H_
#define IGRAMMARRULE_H_

namespace pgen { class IGrammarRule; };

// STL
#include <string>
#include <vector>
// yaml-cpp
#include <yaml-cpp/yaml.h>
// pgen
#include "Language.h"

using namespace std;
namespace pgen 
{
	
	class IGrammarRule 
	{
	public:
		Language* language;				//< The language of this rule
		string name;					//< The name of this rule
		vector<vector<int>*> symbols;	//< Contains a list of symbols for each alternative of the rule.			
		
		IGrammarRule(Language* language, string &name, YAML::Node &ruleNode);
		virtual ~IGrammarRule();
		
		/**
		 * Add all options of a rule node to the rule options list.
		 * @param ruleNode the YAML node to add.
		 */
		virtual void add(YAML::Node &ruleNode);
		
		/**
		 * Returns the rule's matching function name.
		 * @return the rule's matching function name.
		 */
		virtual string funcname();
		
		/**
		 * Generate C99 code that is able to parse the grammar.
		 * @return the C99 code that is able to parse the grammar.
		 */
		virtual string compile() = 0;
	}; /* class IGrammarRule */
}; /* namespace pgen */

#endif /* IGRAMMARRULE_H_ */