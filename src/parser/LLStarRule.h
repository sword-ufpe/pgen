/**
 * \author Dimas Melo Filho
 * \date 2015-01-07
 * \file
 * This file contains the declaration of the LLStarRule class. This class handles a single LL(*) grammar rule. It is
 * able to generate C99 to match to an array of tokens according to a grammar rule.
 */
 
 #ifndef LLSTARRULE_H_
 #define LLSTARRULE_H_
 
 namespace pgen { class LLStarRule; };
 
 // STL
 #include <string>
 #include <vector>
 // pgen
 #include "IGrammarRule.h"
 #include "Language.h"
 
 using namespace std;
 namespace pgen 
 {
	 
	class LLStarRule 
	 : public IGrammarRule 
	{
	public:
		/**
		 * Using IGrammarRule's constructors.
		 */
		using IGrammarRule::IGrammarRule;
		
		/**
		 * Destructor that does nothing for now.
		 */
		virtual ~LLStarRule();
		
		/**
		 * Generate C99 code that is able to parse the grammar.
		 * @return the C99 code that is able to parse the grammar.
		 */
		virtual string compile();
	}; /* class LLStarRule */
 }; /* namespace pgen */
 
 #endif /* LLSTARRULE_H_ */