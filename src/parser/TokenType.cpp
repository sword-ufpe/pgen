/**
 * \author Dimas Melo Filho
 * \date 2015-01-06
 * \tabsize 4
 * \file
 * This file contains the implementation of the TokenType Class. The TokenType is a
 * class that is responsible for handling a type of token. The types of tokens are
 * defined by the language description file. All TokenType objects must be managed
 * and owned by the TokenTypeManager.
 */

// STL
#include <string>
#include <vector>
#include <sstream>
// Other
#include "TokenType.h"

using namespace std;
 
namespace pgen {
	
//----------------------------------------------------------------------------------------------------------------------
TokenType::TokenType(Language* language, string expression, string name, int typeId, bool discard, int setState, 
	int* validStates) 
  : regex(expression)
  , name(name)
  , discard(discard)
  , typeId(typeId)
  , setState(setState)
  , validStates(validStates)
  , language(language)
{
}

//----------------------------------------------------------------------------------------------------------------------
TokenType::~TokenType() {
	if (validStates != nullptr) {
		delete validStates;
	}
}

//----------------------------------------------------------------------------------------------------------------------
void TokenType::matchCode(string tabs, stringstream& s) {
	s << tabs << "*pos = " << regex.funcname() << "(text);" << endl;
	s << tabs << "if (*pos != -1) ";
	if (setState != -1) {
		// Match and set a new state
		s << "{" << endl;
		s << tabs << "\t" << language->prefix << "state = " << setState << ";" << endl;
		s << tabs << "\t return " << (discard? -2 : typeId) << ";" << endl;
		s << tabs << "}" << endl;
	} else {
		// Only match, doesn't make any changes to the state
		s << "return " << (discard? -2 : typeId) << ";" << endl;
	}
}

//----------------------------------------------------------------------------------------------------------------------
string TokenType::code() {
	stringstream s;
	if (validStates != nullptr) {
		// If the token is valid only on specific states, check states first
		s << " if (";
		s << language->prefix << "state == " << validStates[0];
		int i = 1;
		while (validStates[i] != -1) {
			s << " || " << endl << "     " << language->prefix << "state == " << validStates[i++];
		}
		s << ")" << endl << " {" << endl;
		// And then match the token
		matchCode("  ", s);
		s << " }" << endl;
	} else {
		// If the token doesn't specify any valid states, match the code no matter what stat it is in
		matchCode(" ", s);
	}
	return s.str();
}

};
