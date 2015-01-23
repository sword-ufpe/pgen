/**
 * \author Dimas Melo Filho
 * \date 2015-01-07
 * \file
 * This file contains the declaration of the Language class. The Language handles all information regarding the
 * language. It is also responsible for processing everything related to the language such as parsing the YAML source
 * file, calling the tokenizer code generator, the parser code generator and managing information regarding the states,
 * the initial state, the grammar, the initial grammar symbol, etc.
 */
 
#ifndef LANGUAGE_H_
#define LANGUAGE_H_

namespace pgen { class Language; }

// STL
#include <string>
#include <set>
#include <vector>
// Other
#include "Tokenizer.h"
#include "IGrammar.h"

using namespace std;
 
namespace pgen {
	
	class Language {
	public:
		string name;				//< The language name
		string prefix;				//< The language prefix
		string outputFileName;		//< The name of the output file
		string helperFileName;		//< The name of the helper file
		set<string> stateList;		//< The list of states
		vector<string> ruleList;		//< The list of rules
		int startState;				//< The initial state
		int startRule;				//< Starting rule name
		Tokenizer tokenizer;		//< The tokenizer object
		IGrammar* grammar;			//< The grammar object
		//Rule *startRule;			//< The initial rule, all other rules should be accessible from this one.
		
		Language();
		virtual ~Language();
		
		void load(string fileName);
		int getStateId(const string& name);
		int getNonTerminalId(const string& name);
		const string getNonTerminalName(int id);
		int getTerminalId(const string& name);
		string compile();
	};
};
 
#endif /* LANGUAGE_H_ */