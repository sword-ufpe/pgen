/**
 * \author Dimas Melo Filho
 * \date 2015-01-22
 * \file
 * This file contains the declaration of the IGrammar class. This class behaves like an extended interface, which
 * defines the methods that all grammar types must implement.
 */

#include "IGrammar.h"
#include "sstream"

using namespace std;
namespace pgen 
{
	
	IGrammar::IGrammar(Language* language)
	 : language(language)
	{
	}
	
	IGrammar::~IGrammar() 
	{
		for (auto rule: rules)
		{
			if (rule != nullptr) delete rule;
		}
	}

};