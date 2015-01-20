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

namespace pagen { class Language; }

// STL
#include <string>
#include <set>
// Other
#include "Tokenizer.h"

using namespace std;
 
namespace pagen {
	
	class Language {
	public:
		string name;				//< The language name
		string prefix;				//< The language prefix
		string startRule;			//< Starting rule name
		string outputFileName;		//< The name of the output file
		string helperFileName;		//< The name of the helper file
		set<string> stateList;		//< The list of states
		int startState;				//< The initial state
		Tokenizer tokenizer;		//< The tokenizer object
		//Rule *startRule;			//< The initial rule, all other rules should be accessible from this one.
		
		Language();
		virtual ~Language();
		
		void load(string fileName);
		int getStateId(const string& name);
		int getNonTerminalId(const string& name);
	};
};
 
#endif /* LANGUAGE_H_ */