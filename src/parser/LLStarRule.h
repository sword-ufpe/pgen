/**
 * \author Dimas Melo Filho
 * \date 2015-01-07
 * \file
 * This file contains the declaration of the LLStarRule class. This class handles a single LL(*) grammar rule. It is
 * able to generate C99 to match to an array of tokens according to a grammar rule.
 */
 
 #ifndef LLSTARRULE_H_
 #define LLSTARRULE_H_
 
 namespace pagen { class LLStarRule; };
 
 // STL
 #include <string>
 #include <vector>
 // Other
 #include "Language.h"
 
 using namespace std;
 namespace pagen {
	 
	class LLStarRule {
	public:
		Language* language;
		string name;
		vector<vector<int>*> options;
		
		LLStarRule(Language *language, string& ruleName);
		virtual ~LLStarRule();
		
		void addOption(string& body);
		void splitSymbols(string& body, vector<string>& items);
		string compile();
	};
 };
 
 #endif /* LLSTARRULE_H_ */