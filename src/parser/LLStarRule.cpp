/**
 * \author Dimas Melo Filho
 * \date 2015-01-07
 * \file
 * This file contains the implementation of the LLStarRule class. This class handles a single LL(*) grammar rule. It is
 * able to generate C99 to match to an array of tokens according to a grammar rule.
 */

// STL
#include <sstream>
// Other
#include "../misc/LanguageException.h"
#include "LLStarRule.h"
 
using namespace std;
namespace pagen {

//----------------------------------------------------------------------------------------------------------------------
LLStarRule::LLStarRule(Language* language, string& ruleName)
 : language(language)
 , name(ruleName)
{
}

//----------------------------------------------------------------------------------------------------------------------
LLStarRule::~LLStarRule() {
	for (auto option: options) {
		if (option != nullptr) delete option;
	}
}

//----------------------------------------------------------------------------------------------------------------------
void LLStarRule::addOption(string& body) {
	vector<string> items;
	splitSymbols(body, items);
	if (items.size() == 0) {
		options.push_back(nullptr);		// nullptr is equivalent to an empty rule
	} else if (items.size() > 0) {
		vector<int>* symbols = new vector<int>();
		for (string symbol: items) {
			int symbolId = language->getStateId(symbol);
			if (symbolId != -1) {
				symbols->push_back(symbolId);
				continue;
			}
			symbolId = language->getNonTerminalId(symbol);
			if (symbolId != -1) {
				symbols->push_back(symbolId);
				continue;
			}
			throw new LanguageException("Undefined symbol or token '" + symbol + "' on the rule '" + name + "'.");
		}
	} else {
		throw new LanguageException("Invalid body for option of the '" + name + "' grammar rule.");
	}	
}

//----------------------------------------------------------------------------------------------------------------------
void LLStarRule::splitSymbols(string& body, vector<string>& items) {
	stringstream ss(body);
    string item;
    while (getline(ss, item, ' ')) {
        if (!item.empty()) {
			items.push_back(item);
		} 
    }
}

//----------------------------------------------------------------------------------------------------------------------
string LLStarRule::compile() {
	stringstream s;
	// TODO
	return s.str();
}

};