/**
 * \author Dimas Melo Filho
 * \date 2015-01-06
 * \file
 * This file contains the implementation of the Tokenizer class. This class was built to own and manage all token
 * types and to generate the Tokenizer C99 code.
 */
 
#ifndef TOKENIZER_H_
#define TOKENIZER_H_

namespace pgen { class Tokenizer; };

// STL
#include <string>
#include <map>
// Other
#include "TokenType.h"
#include "Language.h"

using namespace std;
namespace pgen 
{
	
	class Tokenizer 
	{
	private:
		Language* language;							//< The language object
		
		void codeDefines(stringstream& s);
		void codeStructs(stringstream& s);
		void codeGlobals(stringstream &s);
		void codeTokenListHelper(stringstream &s);
		void codeNextToken(stringstream &s);
		void codeNumTokens(stringstream &s);
		void codeTokenizeStringLen(stringstream &s);
		void codeTokenizeString(stringstream &s);
		void codeTokenizeFile(stringstream &s);
	public:
		map<string, TokenType*> typeList;			//< map of TokenTypes, indexed by their names.
		
		Tokenizer(Language* language);
		~Tokenizer();
		
		/**
		 * Returns the typeId for the specified token type name.
		 * \param name the token type name.
		 * \return the current typeId being used by the tokentype with the name, or a new id if the tokentype is not
		 * present on the typeList yet.
		 */
		int getTypeId(const string& name);
		
		/**
		 * Add a TokenType to the manager.
		 * \param tokenType a pointer to the tokenType to be added.
		 * \remark once added, the tokenType is managed and owned by this Tokenizer.
		 */ 
		void add(TokenType* tokenType);
		
		/**
		 * Creates a TokenType from an expression and a name and add it to the Tokenizer.
		 * \param expression the regex expression of the token
		 * \param name the name of the token, when the name starts with '%', the token is read but ignored by the parser
		 * \param setState the state to be set when this token is read by the parser or -1 if state shouldn't change
		 * \param validStates list of states on which this token is valid, nullptr if all states are valid.
		 * \remark The created TokenType will be fully owned and managed by this Tokenizer.
		 */
		void add(string expression, string name, int setState = -1, int* validStates = nullptr);
		
		/**
		 * Returns the C99 code for tokenizing a string and a file according to the token type rules.
		 * \return the C99 code for tokenizing a string and also a code to tokenize a file according to the token rules.
		 */
		string code();
		
		const string fnNameTokenizeStringLen();
		
		/**
		 * Name of the function to tokenize strings.
		 * \return the name of the C99 method on the generated code that can be used to tokenize a string.
		 */
		const string fnNameTokenizeString();
		 
		/**
		 * Name of the function to tokenize files.
		 * \return the name of hte C99 method on the generated code that can be used to tokenize a file.
		 */
		const string fnNameTokenizeFile();
		
		/**
		 * Name of the function to get the number of tokens.
		 */
		const string fnNameNumTokens();
	}; /* class Tokenizer */
}; /* namespace pgen */

#endif /* TOKENIZER_H_ */