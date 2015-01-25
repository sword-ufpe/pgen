/**
 * \author Dimas Melo Filho
 * \date 2015-01-07
 * \file
 * This file contains the declaration of the LLStar class. This class is responsible for generating a LL(*) parser in a
 * recursive descent way, for parsing a specified language. The class requires access to the Language and its tokens in
 * order to generate code for parsing it.
 */
 
#ifndef LLSTAR_H_
#define LLSTAR_H_

namespace pgen { class LLStar; }

// STL
#include <string>
#include <vector>
// yaml-cpp
#include <yaml-cpp/yaml.h>
// pgen
#include "IGrammar.h"
#include "Language.h"

using namespace std; 
namespace pgen 
{
	
	class LLStar 
	 : public IGrammar 
	{
	public:
		/**
		 * Name of the grammar type (parser type)
		 */
		static const string NAME;
		
		/**
		 * Using the IGrammar constructor.
		 */
		using IGrammar::IGrammar;
		
		/**
		 * Virtual destructor. Currently does nothing. However the base (IGrammar) constructor is called and frees all
		 * rules.
		 */
		virtual ~LLStar();
		
		/**
		 * Adds a new grammar rule from a YAML node.
		 * \param name the name of the rule
		 * \param ruleNode the actual rule node.
		 */
		virtual void addRule(string &name, YAML::Node &ruleNode);
		
		/**
		 * Generate C99 code that is able to parse the grammar.
		 * \return the C99 code that is able to parse the grammar.
		 */
		virtual string compile();
	}; /* class LLStar */
}; /* namespace pgen */
 
#endif /* LLSTAR_H_ */