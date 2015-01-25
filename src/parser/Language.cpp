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

	Language::Language()
	 : tokenizer(this)
	{	
	}

	Language::~Language()
	{		
	}
	
	void Language::load(string fileName) 
	{
		YAML::Node spec = YAML::LoadFile(fileName);
		// Load the "language" node
		YAML::Node languageNode = spec["language"];
		ASSERT_NODE(languageNode, "language");	
		YAML::Node node = languageNode["name"];		// language.name
		ASSERT_NODE(node, "name");
		this->name = node.as<string>();	
		node = languageNode["prefix"];				// language.prefix
		ASSERT_NODE(node, "prefix");
		this->prefix = node.as<string>();	
		node = languageNode["startRule"];			// language.startRule (name)
		ASSERT_NODE(node, "startRule");
		string startRuleName = node.as<string>();	
		node = languageNode["startState"];			// language.startState (name)
		this->startStateName = (node ? node.as<string>() : "default");	
		node = languageNode["outputFile"];			// language.outputFile (name)
		this->outputFileName = (node ? node.as<string>() : "parser.c");
		node = languageNode["helperFile"];			// language.helperFile (name)
		this->helperFileName = (node ? node.as<string>() : "parser_helper.c");
		node = languageNode["type"];				// language.type (name)
		string languageType = (node? node.as<string>() : "LL(*)");
		// Load the "states" node
		this->stateList.clear();
		this->stateList.insert("default");
		YAML::Node statesNode = spec["states"];
		if (statesNode && statesNode.IsSequence())
		{
			for (int i = 0, sz = statesNode.size(); i < sz; i++) 
			{
				this->stateList.insert(statesNode[i].as<string>());
			}
		}
		this->startState = getStateId(startStateName);				// get the initial state id by its name
		// Load the "tokens" node
		YAML::Node tokensNode = spec["tokens"];
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
		// Load the "grammar" node
		vector<YAML::Node> ruleNodes;
		this->ruleList.clear();
		YAML::Node grammarNode = spec["grammar"];
		ASSERT_NODE(grammarNode, "grammar");
		if (!grammarNode.IsSequence()) throw new LanguageException("The grammar node needs to be a Sequence");
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
		
		if (languageType == "LL(*)") {
			grammar = new LLStar(this);
		} else {
			throw new LanguageException("Invalid language type '" + languageType + "'.");
		}


		for (int j = 0, sz = ruleNodes.size(); j< sz; j++) {
			grammar->addRule(ruleList[j], ruleNodes[j]);
		}
	}

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
	
	const string Language::getNonTerminalName(int id)
	{
		id -= 1000000000;
		if (id < 0 || (unsigned int)id >= ruleList.size()) throw new LanguageException("Invalid rule id.");
		for (string item: ruleList) {
			if (id-- == 0) return item;
		}
		throw new LanguageException("Invalid rule id.");
	}

	int Language::getTerminalId(const string& name) 
	{
		return tokenizer.getTypeId(name);
	}
	
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
			s << "  case " << i++ << ": return \"" << it->first << "\";"					"\n";
		}
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