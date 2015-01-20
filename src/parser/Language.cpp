/**
 * \author Dimas Melo Filho
 * \date 2015-01-07
 * \file
 * This file contains the declaration of the Language class. The Language handles all information regarding the
 * language. It is also responsible for processing everything related to the language such as parsing the YAML source
 * file, calling the tokenizer code generator, the parser code generator and managing information regarding the states,
 * the initial state, the grammar, the initial grammar symbol, etc.
 */

#include <string>
#include <iostream>
#include <yaml-cpp/yaml.h>
#include "Language.h"
#include "../misc/LanguageException.h"

using namespace std; 
namespace pagen {

//----------------------------------------------------------------------------------------------------------------------
Language::Language()
 : tokenizer(this)
{
	
}

//----------------------------------------------------------------------------------------------------------------------
Language::~Language() {
	
}

//----------------------------------------------------------------------------------------------------------------------
void Language::load(string fileName) {
#define ASSERT_NODE(_v,_n) if (!_v) throw new LanguageException("Could not find the '" _n "' node on the YAML language specification")
	YAML::Node spec = YAML::LoadFile(fileName);
	YAML::Node languageNode = spec["language"];
	ASSERT_NODE(languageNode, "language");
	
	YAML::Node node = languageNode["name"];
	ASSERT_NODE(node, "name");
	this->name = node.as<string>();
	
	node = languageNode["prefix"];
	ASSERT_NODE(node, "prefix");
	this->prefix = node.as<string>();
	
	node = languageNode["startRule"];
	ASSERT_NODE(node, "startRule");
	this->startRule = node.as<string>();
	
	node = languageNode["startState"];
	string startStateName = (node ? node.as<string>() : "default");
	
	node = languageNode["outputFile"];
	this->outputFileName = (node ? node.as<string>() : "parser.c");
	
	node = languageNode["helperFile"];
	this->helperFileName = (node ? node.as<string>() : "parser_helper.c");
	
	this->stateList.clear();
	this->stateList.insert("default");
	YAML::Node statesNode = spec["states"];
	if (statesNode && statesNode.IsSequence()) {
		for (int i = 0, sz = statesNode.size(); i < sz; i++) {
			this->stateList.insert(statesNode[i].as<string>());
		}
	}
	
	this->startState = getStateId(startStateName);
	
	YAML::Node tokensNode = spec["tokens"];
	ASSERT_NODE(tokensNode, "tokens");
	if (!tokensNode.IsSequence()) throw new LanguageException("The 'tokens' node needs to be a Sequence.");
	for (int i = 0, sz = tokensNode.size(); i < sz; i++) {
		YAML::Node token = tokensNode[i];
		if (token && token.IsMap()) {
			node = token["name"];
			ASSERT_NODE(node, "token.name");
			string tokenName = node.as<string>();
			node = token["regex"];
			ASSERT_NODE(node, "token.regex");
			string tokenRegex = node.as<string>();
			set<int> validStates;
			validStates.insert(getStateId("default"));
			node = token["states"];
			if (node && node.IsSequence()) {
				for (int j = 0, szj = node.size(); j < szj; j++) {
					int stateId = getStateId(node[j].as<string>());
					if (stateId != -1) validStates.insert(stateId);
				}
			}
			node = token["set_state"];
			int setStateId = -1;
			if (node) {
				setStateId = getStateId(node.as<string>());
			}
			int* validStatesArray = nullptr;
			if (validStates.size() > 0) {
				validStatesArray = new int[validStates.size()+1];
				int j = 0;
				for (auto validState: validStates) {
					validStatesArray[j++] = validState;
				}
				validStatesArray[j] = -1;
			}
			tokenizer.add(tokenRegex, tokenName, setStateId, validStatesArray);
		}
	}
	
	cout << endl <<
		"//Name: " << this->name << endl <<
		"//Prefix: " << this->prefix << endl <<
		"//StartRule: " << this->startRule << endl <<
		"//StartStateName: " << startStateName << endl <<
		"//Output File: " << this->outputFileName << endl <<
		"//Helper File: " << this->helperFileName << endl << endl;
		
	cout << "//States:" << endl;
	int i = 0;
	for (auto state: stateList) {
		cout << "//" << i++ << " - " << state << endl;
	}
	
	cout << endl << endl << tokenizer.code();
	
#undef ASSERT_NODE
}

//----------------------------------------------------------------------------------------------------------------------
int Language::getStateId(const string& stateName) {
	int id = 0;
	for (string item: stateList) {
		if (item == stateName) return id;
		id++;
	}
	return -1;
}

//----------------------------------------------------------------------------------------------------------------------
int Language::getNonTerminalId(const string& name) {
	// TODO
}

};
