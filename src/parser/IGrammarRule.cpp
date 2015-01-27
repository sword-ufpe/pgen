/**
 * \author Dimas Melo Filho
 * \date 2015-01-22
 * \file
 * This file contains the declaration of the IGrammarRule class. This class behaves like an extended interface, which
 * defines the methods that all grammar rule types must implement.
 */

// pgen
#include "IGrammarRule.h"
#include "../misc/LanguageException.h"
#include "../misc/Util.h"

using namespace std;
namespace pgen 
{
	IGrammarRule::IGrammarRule(Language* language, string& name)
	 : language(language)
	 , name(name)
	{
	}
	
	IGrammarRule::IGrammarRule(Language* language, string& name, YAML::Node &ruleNode)
	 : language(language)
	 , name(name)
	{
		add(ruleNode);
	}
	
	IGrammarRule::~IGrammarRule() 
	{
		for(auto symSeq: symbols) 
		{
			if (symSeq != nullptr) 
			{ 
				delete symSeq;
			}
		}
	}
	
	void IGrammarRule::add(YAML::Node &ruleNode) 
	{
		if (!ruleNode.IsSequence()) 
		{
			throw new LanguageException("Expected a Sequence of rule options for rule '" + name + "'.");
		}
		for (int i = 0, sz = ruleNode.size(); i < sz; i++) 
		{
			YAML::Node node = ruleNode[i];
			if (node.IsScalar()) 
			{
				string ruleExpr = node.as<string>();
				vector<string> items;
				Util::splitSymbols(ruleExpr, items, ' ');
				if (items.size() > 0)
				{
					vector<int>* symSeq = new vector<int>();
					symbols.push_back(symSeq);
					for (string item: items)
					{
						int symbolId = language->getSymbolId(item);
						if (symbolId == -1) 
						{ 
							throw new LanguageException("Unrecognized symbol: " + item);
						}
						symSeq->push_back(symbolId);
					}
				}
			}
		}
	}
	
	string IGrammarRule::funcname()
	{
		stringstream s;
		s << language->prefix << "parse_" << name;
		return s.str();
	}
}; /* namespace pgen */