/**
 * \author Dimas Melo Filho
 * \date 2015-01-07
 * \tabsize 4
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
// yaml-cpp
#include <yaml-cpp/yaml.h>
// Other
#include "Tokenizer.h"
#include "IGrammar.h"

using namespace std; 
namespace pgen 
{
	
	class Language 
	{
	private:
		/**
		 * Write C99 comments with information about the parser.
		 * @param s the stringstream to write the comments to.
		 */
		void comments(stringstream &s);
		
	public:
		string name;				//< The language name
		string prefix;				//< The language prefix
		string outputFileName;		//< The name of the output file
		string helperFileName;		//< The name of the helper file
		string startStateName;		//< The name of the initial state
		string startRuleName;		//< The name of the starting rule
		string languageType;		//< The name of the language type
		set<string> stateList;		//< The list of states
		vector<string> ruleList;	//< The list of rules
		int startState;				//< The initial state
		int startRule;				//< Starting rule name
		Tokenizer tokenizer;		//< The tokenizer object
		IGrammar* grammar;			//< The grammar object
		
		/**
		 * Language default constructor. Currently only initializes the tokenizer object.
		 */
		Language();
		
		/**
		 * Language destructor, currently does nothing.
		 */
		virtual ~Language();
		
		/**
		 * Parse the "states" node of a YAML language definition file.
		 * \param statesNode the YAML node loaded from the file.
		 */
		void loadStatesNode(YAML::Node statesNode);
		
		/**
		 * Parse the "language" node of a YAML language definition file.
		 * \param languageNode the YAML node loaded from the file.
		 */
		void loadLanguageNode(YAML::Node languageNode);
		
		/**
		 * Parse the "tokens" node of a YAML language definition file.
		 * \param tokensNode the YAML node loaded from the file.
		 */
		void loadTokensNode(YAML::Node tokensNode);
		
		/**
		 * Parse the "grammar" node of a YAML language definition file.
		 * \param grammarNode the YAML node loaded from the file.
		 */
		void loadGrammarNode(YAML::Node grammarNode, vector<YAML::Node>& ruleNodes);
		
		/**
		 * Loads a language definition from a YAML file.
		 * \param fileName the YAML file name.
		 */
		void load(string fileName);
		
		/**
		 * Get the stateId of a state by its name.
		 * \param the state name
		 * \return the stateId when found, -1 when not found.
		 */
		int getStateId(const string& name);
		
		/**
		 * Returns the name of a non-terminal symbol by its symbolId.
		 * \param id the symbolId to get the name of.
		 * \return the non-terminal symbol name (when found).
		 * \throws LanguageException "Invalid rule id" when the symbolId is not found.
		 */
		const string getNonTerminalName(int id);
		
		/**
		 * Returns the symbolId of a non-terminal symbol.
		 * \param name the name of a non-terminal symbol (case-sensitive).
		 * \return the symbolId (when found), or -1 when not found.
		 * \remark all non-terminal symbolIds are greater than 1000000000 in order to differentiate from the terminal
		 * symbol Ids in the middle of the code.
		 */
		int getNonTerminalId(const string& name);
		
		/**
		 * Returns the symbolId of a terminal symbol.
		 * \param name the name of a terminal symbol (case-sensitive).
		 * \return the symbolId (when found), or -1 when not found.
		 */
		int getTerminalId(const string& name);
		
		/**
		 * Returns a symbolId, either terminal or non-terminal symbol.
		 * \param name the name of a terminal or non-terminal symbol (case-sensitive).
		 * \return the symbolId (when found), or -1 when not found.
		 * \remark this method uses the getNonTerminalId and getTerminalId methods from the Language class.
		 */
		int getSymbolId(const string& name);
		
		/**
		 * \return a string containing the compiled C99 code that is able to parse the language.
		 */
		string compile();
	}; /* class Language */
}; /* namespace pgen */
 
#endif /* LANGUAGE_H_ */