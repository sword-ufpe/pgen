/**
 * \author Dimas Melo Filho
 * \date 2015-01-07
 * \file
 * This file contains the declaration of the Language class. The Language handles all information regarding the
 * language. It is also responsible for processing everything related to the language such as parsing the YAML source
 * file, calling the tokenizer code generator, the parser code generator and managing information regarding the states,
 * the initial state, the grammar, the initial grammar symbol, etc.
 */

// STL
#include <string>
#include <sstream>
#include <iostream>
// yaml-cpp
#include <yaml-cpp/yaml.h>
// pgen
#include "Language.h"
#include "LLStar.h"
#include "../misc/LanguageException.h"

// DEFINITIONS
#define ASSERT_NODE(_v,_n) \
if (!_v) throw new LanguageException("Could not find the '" _n "' node on the YAML language specification");

using namespace std; 
namespace pgen 
{

	/**
	 * Language default constructor. Currently only initializes the tokenizer object.
	 */
	Language::Language()
	 : tokenizer(this)
	{
	}

	/**
	 * Language destructor, currently does nothing.
	 */
	Language::~Language()
	{
	}
	
	/**
	 * Parse the "states" node of a YAML language definition file.
	 * \param statesNode the YAML node loaded from the file.
	 */
	void Language::loadStatesNode(YAML::Node statesNode)
	{
		this->stateList.clear();
		this->stateList.insert("default");
		if (statesNode && statesNode.IsSequence())
		{
			for (int i = 0, sz = statesNode.size(); i < sz; i++) 
			{
				this->stateList.insert(statesNode[i].as<string>());
			}
		}
	}
	
	/**
	 * Parse the "language" node of a YAML language definition file.
	 * \param languageNode the YAML node loaded from the file.
	 */
	void Language::loadLanguageNode(YAML::Node languageNode)
	{
		ASSERT_NODE(languageNode, "language");	
		// language.name
		YAML::Node node = languageNode["name"];		
		ASSERT_NODE(node, "name");
		this->name = node.as<string>();	
		// language.prefix
		node = languageNode["prefix"];
		ASSERT_NODE(node, "prefix");
		this->prefix = node.as<string>();	
		// language.startRule (name)
		node = languageNode["startRule"];			
		ASSERT_NODE(node, "startRule");
		this->startRuleName = node.as<string>();	
		// language.startState (name)
		node = languageNode["startState"];		
		this->startStateName = (node ? node.as<string>() : "default");	
		this->startState = getStateId(startStateName);
		// language.outputFile (name)
		node = languageNode["outputFile"];	
		this->outputFileName = (node ? node.as<string>() : "parser.c");
		// language.helperFile (name)
		node = languageNode["helperFile"];
		this->helperFileName = (node ? node.as<string>() : "parser_helper.c");
		// language.type (name)
		node = languageNode["type"];
		this->languageType = (node? node.as<string>() : "LL(*)");
	}
	
	/**
	 * Parse the "tokens" node of a YAML language definition file.
	 * \param tokensNode the YAML node loaded from the file.
	 */
	void Language::loadTokensNode(YAML::Node tokensNode)
	{
		YAML::Node node;
		ASSERT_NODE(tokensNode, "tokens");
		if (!tokensNode.IsSequence()) {
			throw new LanguageException("The 'tokens' node needs to be a Sequence.");
		}
		for (int i = 0, sz = tokensNode.size(); i < sz; i++) 		// for each token
		{		
			YAML::Node token = tokensNode[i];
			if (token && token.IsMap()) 
			{
				node = token["name"];								// token.name
				ASSERT_NODE(node, "token.name");
				string tokenName = node.as<string>();
				node = token["regex"];								// token.regex
				ASSERT_NODE(node, "token.regex");
				string tokenRegex = node.as<string>();
				set<int> validStates;								// token.states
				validStates.insert(getStateId("default"));
				node = token["states"];
				if (node && node.IsSequence()) 
				{
					for (int j = 0, szj = node.size(); j < szj; j++) 
					{
						int stateId = getStateId(node[j].as<string>());
						if (stateId != -1)
						{
							validStates.insert(stateId);
						}
					}
				}
				int* validStatesArray = nullptr;
				if (validStates.size() > 0) 
				{
					validStatesArray = new int[validStates.size()+1];
					int j = 0;
					for (auto validState: validStates) 
					{
						validStatesArray[j++] = validState;
					}
					validStatesArray[j] = -1;
				}
				node = token["set_state"];							// token.set_state
				int setStateId = -1;
				if (node) 
				{
					setStateId = getStateId(node.as<string>());
				}
				tokenizer.add(tokenRegex, tokenName, setStateId, validStatesArray);
			}
		}
	}
	
	/**
	 * Parse the "grammar" node of a YAML language definition file.
	 * \param grammarNode the YAML node loaded from the file.
	 */
	void Language::loadGrammarNode(YAML::Node grammarNode, vector<YAML::Node>& ruleNodes)
	{
		YAML::Node node;
		ASSERT_NODE(grammarNode, "grammar");
		if (!grammarNode.IsSequence()) 
		{ 
			throw new LanguageException("The grammar node needs to be a Sequence");
		}
		this->ruleList.clear();
		for (int j = 0, sz = grammarNode.size(); j < sz; j++) 
		{
			node = grammarNode[j];
			for (YAML::Node::const_iterator it = node.begin(); it != node.end(); ++it) 
			{
				auto &key = it->first;
				auto &ruleNode = it->second;
				if (key.IsScalar() && ruleNode.IsSequence()) 
				{
					string ruleName = key.as<string>();
					this->ruleList.push_back(ruleName);
					ruleNodes.push_back(ruleNode);
				}
			}
		}
		this->startRule = getNonTerminalId(startRuleName);
		if (startRule == -1)
		{
			throw new LanguageException("The start rule was not found: " + startRuleName);
		}
	}
	
	/**
	 * Loads a language definition from a YAML file.
	 * \param fileName the YAML file name.
	 */
	void Language::load(string fileName) 
	{
		vector<YAML::Node> ruleNodes;
		YAML::Node spec = YAML::LoadFile(fileName);		
		
		loadStatesNode(spec["states"]);
		loadLanguageNode(spec["language"]);
		loadTokensNode(spec["tokens"]);
		loadGrammarNode(spec["grammar"], ruleNodes);
		
		if (languageType == "LL(*)") {
			grammar = new LLStar(this);
		} else {
			throw new LanguageException("Invalid language type '" + languageType + "'.");
		}

		for (int j = 0, sz = ruleNodes.size(); j< sz; j++) {
			grammar->addRule(ruleList[j], ruleNodes[j]);
		}
	}
	
	/**
	 * Get the stateId of a state by its name.
	 * \param the state name
	 * \return the stateId when found, -1 when not found.
	 */
	int Language::getStateId(const string& stateName) 
	{
		int id = 0;
		for (string item: stateList) 
		{
			if (item == stateName) return id;
			++id;
		}
		return -1;
	}
	
	/**
	 * Returns the name of a non-terminal symbol by its symbolId.
	 * \param id the symbolId to get the name of.
	 * \return the non-terminal symbol name (when found).
	 * \throws LanguageException "Invalid rule id" when the symbolId is not found.
	 */
	const string Language::getNonTerminalName(int id)
	{
		id -= 1000000000;
		if (id < 0 || (unsigned int)id >= ruleList.size()) throw new LanguageException("Invalid rule id.");
		for (string item: ruleList) {
			if (id-- == 0) return item;
		}
		throw new LanguageException("Invalid rule id.");
	}
	
	/**
	 * Returns the symbolId of a non-terminal symbol.
	 * \param name the name of a non-terminal symbol (case-sensitive).
	 * \return the symbolId (when found), or -1 when not found.
	 * \remark all non-terminal symbolIds are greater than 1000000000 in order to differentiate from the terminal
	 * symbol Ids in the middle of the code.
	 */
	int Language::getNonTerminalId(const string& name)
	{
		int id = 1000000000;
		for (string item: ruleList) 
		{
			if (item == name) return id;
			++id;
		}
		return -1;
	}

	/**
	 * Returns the symbolId of a terminal symbol.
	 * \param name the name of a terminal symbol (case-sensitive).
	 * \return the symbolId (when found), or -1 when not found.
	 */
	int Language::getTerminalId(const string& name) 
	{
		return tokenizer.getTypeId(name);
	}
	
	/**
	 * Returns a symbolId, either terminal or non-terminal symbol.
	 * \param name the name of a terminal or non-terminal symbol (case-sensitive).
	 * \return the symbolId (when found), or -1 when not found.
	 * \remark this method uses the getNonTerminalId and getTerminalId methods from the Language class.
	 */
	int Language::getSymbolId(const string& name)
	{
		int symbolId = getTerminalId(name);
		if (symbolId != -1) return symbolId;
		return getNonTerminalId(name);
	}
	
	/**
	 * Write C99 comments with information about the parser.
	 * \param s the stringstream to write the comments to.
	 */
	void Language::comments(stringstream &s)
	{
		s << endl <<
			"// Name: " << this->name << endl <<
			"// Prefix: " << this->prefix << endl <<
			"// StartRule: " << this->startRule << endl <<
			"// StartStateName: " << startStateName << endl <<
			"// Output File: " << this->outputFileName << endl <<
			"// Helper File: " << this->helperFileName << endl << endl;
			
		s << "// States:" << endl;
		int i = 0;
		for (auto state: stateList) 
		{
			s << "// " << i++ << " - " << state << endl;
		}
		s << "// Symbol List" << endl;
		i = 0;
		for (auto it = tokenizer.typeList.begin(); it != tokenizer.typeList.end(); ++it) {
			s << "// " << i++ << " - TOKEN: " << it->first << endl;
		}
		i = 1000000000;
		for (auto rule: ruleList) 
		{
			s << "// " << i++ << " - RULE: " << rule << endl;
		}
	}
	
	/**
	 * \return a string containing the compiled C99 code that is able to parse the language.
	 */
	string Language::compile() 
	{
		stringstream s;
		comments(s);
		s << "char* getSymbolName(int id) {"											"\n"
			 " switch (id) {"															"\n";
		for (unsigned int id = 0; id < ruleList.size(); id++) 
		{
			s << "  case " << (id+1000000000) << ": return \"" << ruleList[id] << "\";" "\n";
		}
		int i = 0;
		for (auto it = tokenizer.typeList.begin(); it != tokenizer.typeList.end(); ++it) 
		{
			s << "  case " << i++ << ": return \"" << it->first << "\";"				"\n";
		}
		s << "  default: return \"__INVALID_SYMBOL__\";"								"\n";
		s << " }"																		"\n"
			 "}"																		"\n\n";
		s << endl << "//-------------------------------------" << endl;
		s << tokenizer.code();
		s << endl << "//-------------------------------------" << endl;
		s << grammar->compile();
		return s.str();
	}
	
}; /* namespace pgen */

#undef ASSERT_NODE