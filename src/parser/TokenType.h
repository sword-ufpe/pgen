/**
 * \author Dimas Melo Filho
 * \date 2015-01-06
 * \tabsize 4
 * \file
 * This file contains the declaration of the TokenType Class. The TokenType is a
 * class that is responsible for handling a type of token. The types of tokens are
 * defined by the language description file. All TokenType objects must be managed
 * and owned by the TokenTypeManager.
 */
 
#ifndef TOKENTYPE_H_
#define TOKENTYPE_H_

namespace pgen { class TokenType; };

// STL
#include <string>
#include <vector>
// Other
#include "../expr/Regex.h"
#include "Language.h"

using namespace std;
 
namespace pgen {
	
	class TokenType {
	private:
		/**
		 * Returns the C99 code for matching this TokenType, ignoring the validStates.
		 * This method is supposed to be used internally by the "code" method.
		 * \param tabs the string of the tabs to be prepended to each command line.
		 * \param s the stringstream reference to use.
		 */
		void matchCode(string tabs, stringstream& s);
		
	public:
		Regex regex;				//< The regex object associated to this token type
		string name;				//< The token type name
		bool discard;				//< Set to true when the token type should be discarded by the parser
		int typeId;					//< The typeId (the integer that identifies this token type on the C99).
		int setState;				//< The state to be set when the token is found or an empty string if no state 
									//< should be set.
		int* validStates;			//< the states in which this token is valid, or leave without items if it is valid
									//< in all states.
		Language* language;			//< Pointer to the language object. (the language of this TokenType). The language
									//< is not owned or managed by the TokenType.
		
		/**
		 * Constructs a token with a specified type and content.
		 * \param expression the regular expression that can match all tokens of this type
		 * \param name the name of this token type
		 * \param typeId the integer that identifies this token type on the C99 (usually the index of the type on some
		 * sort of vector or list).
		 * \param discard true if the token should be discarded or ignored by the parser. (i.e. all tokens that have
		 * names starting with a '%'.
		 * \param setState the stateId of the state that this token type must set when read.
		 * \param validStates the list of stateIds of the states in which this token can occur. This list, when passed
		 * to the TokenType constructor is supposed to be owned and managed by the TokenType object. It will be 
		 * automatically freed when the TokenType is destroyed.
		 * \remark When the validStates array is provided, it must contain at least ONE VALID ITEM.
		 */
		TokenType(Language* language, string expression, string name, int typeId, bool discard = false, 
			int setState = -1, int* validStates = nullptr);
		/**
		 * TokenType virtual destructor. When the validStates array is set, it is freed.
		 */
		virtual ~TokenType();
		
		/**
		 * Returns the code that parses this TokenType (to be used on the next_token function).
		 */
		string code();
	};
};
 
#endif /* TOKENTYPE_H_ */