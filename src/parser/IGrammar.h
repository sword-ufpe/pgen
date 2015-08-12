/**
 * \author Dimas Melo Filho
 * \date 2015-01-22
 * \file
 * This file contains the declaration of the IGrammar class. This class behaves like an extended interface, which
 * defines the methods that all grammar types must implement.
 */

#ifndef IGRAMMAR_H_
#define IGRAMMAR_H_

namespace pgen { class IGrammar; };

// STL
#include <vector>
// yaml-cpp
#include <yaml-cpp/yaml.h>
// pgen
#include "IGrammarRule.h"
#include "Language.h"

using namespace std;
namespace pgen 
{
	class IGrammar 
	{
	public:
		/**
		 * Pointer to the language object
		 */
		Language* language;
		
		/**
		 * List of rules. Each rule can have multiple options (alternatives) to follow.
		 */
		vector<IGrammarRule*> rules;
		 
		/**
		 * Constructs a new IGrammar object with a given.
		 */
		IGrammar(Language* language);
		
		/**
		 * Destructor
		 */
		virtual ~IGrammar();
		 
		/**
		 * Adds a new grammar rule from a YAML node.
		 * \param name the name of the rule
		 * \param ruleNode the actual rule node.
		 */
		virtual void addRule(string &name, YAML::Node &ruleNode) = 0;
		
		/**
		 * Generate C99 code that is able to parse the grammar.
		 * \return the C99 code that is able to parse the grammar.
		 */
		virtual string compile() = 0;
	}; /* class IGrammar */
}; /* namespace pgen */

#endif /* IGRAMMAR_H_ */