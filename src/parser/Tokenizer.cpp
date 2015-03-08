/**
 * \author Dimas Melo Filho
 * \date 2015-01-06
 * \tabsize 4
 * \file
 * This file contains the implementation of the Tokenizer class. This class was built to own and manage all token
 * types and to generate the Tokenizer C99 code.
 */
 
// STL
#include <vector>
#include <string>
#include <sstream>
// pgen
#include "../expr/Code.h"
#include "TokenType.h"
#include "Tokenizer.h"

using namespace std;
namespace pgen 
{

	/**
	 * Construct a new Tokenizer for the given language.
	 * The only field of the language that is used by the Tokenizer is the "prefix" field, which should be something
	 * like "ML_".
	 */
	Tokenizer::Tokenizer(Language* language)
	 : language(language)
	{
	}

	/**
	 * Destroy the Tokenizer. Delete all TokenTypes on the list.
	 */
	Tokenizer::~Tokenizer() 
	{
		for (auto& kv: typeList) 
		{
			delete kv.second;
		}
	}

	/**
	 * Gets the id of a type by its name.
	 * \return the TokenType ID (int).
	 */
	int Tokenizer::getTypeId(const string& name) 
	{
		auto iter = typeList.find(name);
		if (iter != typeList.end()) 
		{
			return iter->second->typeId;
		}
		return -1;
	}

	/**
	 * Adds a TokenType to the Tokenizer (the TokenType is provided by the user).
	 * \param tokenType the tokenType.
	 * \remark the ownership of the tokentype is transfered to the tokenizer. i.e. the user doesn't need to delete it. 
	 * In fact, the user must not delete it, and it must not be a stackframe object which will get released upon the end
	 * of some method.
	 */
	void Tokenizer::add(TokenType* tokenType) 
	{
		if (tokenType->name.length() <= 0) return;
		auto iter = typeList.find(tokenType->name);
		if (iter != typeList.end()) 
		{
			delete iter->second;
		}
		typeList[tokenType->name] = tokenType;
	}

	/**
	 * Adds a TokenType to the Tokenizer (the TokenType is constructed inside the method).
	 * \param expression the TokenType expression (regex)
	 * \param name the name of the type
	 * \param setState the state to set the tokenizer when the token is matched (or -1 if state doesn't change)
	 * \param validStates the states in which the token is valid (or nullptr if the token is valid on all states)
	 */
	void Tokenizer::add(string expression, string name, int setState, int* validStates) 
	{
		bool discard = (name.length() > 0 && name[0] == '$');
		int typeId = this->getTypeId(name);
		if (typeId == -1) 
		{ 
			typeId = typeList.size();
		}
		TokenType *tokenType = new TokenType(language, expression, name, typeId, discard, setState, validStates);
		this->add(tokenType);
	}

	/**
	 * Write the defines needed by the tokenizer.
	 * Currently only one define is needed. the EINVTOK which is an error identifier.
	 * \param stringstream the stringstream to write to
	 */
	void Tokenizer::codeDefines(stringstream& s) 
	{
		s << "#define EINVTOK 1\n\n";
	}

	/**
	 * Write the globals needed by the tokenizer.
	 * Currently only one global is used, a "state" global, which keeps the current state of the tokenizer.
	 * \param stringstream the stringstream to write to
	 */
	void Tokenizer::codeGlobals(stringstream &s) 
	{
		s << "int " << language->prefix << "state = " << language->startState << ";"	"\n"
			 "int " << language->prefix << "inv_token_pos = -1;"						"\n"
			 "char " << language->prefix << "inv_token_txt[200];"						"\n\n";
	}

	/**
	 * Writes the code to the next_token method, which gets the next token of the text.
	 * \param stringstream the stringstream to write to
	 */
	void Tokenizer::codeNextToken(stringstream &s) 
	{
		s << "int next_token(char* text, int* pos) {"			"\n";
		for (int i = 0, sz = typeList.size(); i < sz; i++) {
			for (auto kv: typeList) {
				if (kv.second->typeId == i) {
					s << kv.second->code();
					break;
				}
			}
		}
		s << " strncpy(" << language->prefix << "inv_token_txt, text, 200);"		"\n"
			 " return -1;"															"\n"
			 "}"																	"\n\n";
	}

	/**
	 * Contains the get_num_tokens method, which returns the number of tokens on the text.
	 * It does a complete tokenization of the text, without allocating any memory or copying any data, in order to check
	 * how many tokens there are.
	 * I believe this is faster than allocating dynamic structures to keep the tokens as they are parsed, because it 
	 * reduces the memory fragmentation by allocating a single huge block of memory to keep all the data.
	 * \param stringstream the stringstream to write to
	 */
	void Tokenizer::codeNumTokens(stringstream &s) 
	{
		s << "int " << fnNameNumTokens() << "(char* text, int len) {"	"\n"
			 " int pos = 0;"											"\n"
			 " int p;"													"\n"
			 " int tok = 0;"											"\n"
			 " int tokid = 0;"											"\n"
			 " " << language->prefix << "state = " << language->startState << ";\n"
			 " while (pos < len) {"										"\n"
			 "  tokid = next_token(text, &p);"							"\n"
			 "  if (tokid == -1) {"										"\n"
			 "   " << language->prefix << "inv_token_pos = pos;"		"\n"
			 "   return 0;"												"\n"
			 "  }"														"\n"
			 "  text += p;"												"\n"
			 "  pos += p;"												"\n"
			 "  if (tokid == -2) continue;"								"\n"
			 "  ++tok;"													"\n"
			 " }"														"\n"
			 " if (pos != len) return 0;"								"\n"
			 " return tok;"												"\n"
			 "}"														"\n\n";
	}

	/**
	 * Writes the tokenize_string_len method, which tokenizes a text until a max number of utf-8 characters are parsed.
	 * \param stringstream the stringstream to write to
	 */
	void Tokenizer::codeTokenizeStringLen(stringstream &s) 
	{
		s << "token_list* " << fnNameTokenizeStringLen() << "(char* text, int len) {\n"
			 " int pos = 0;"											"\n"
			 " int tokenId;"											"\n"
			 " token_list* tokens;"										"\n"
			 " int numTokens = " << fnNameNumTokens() << "(text, len);"	"\n"
			 " if (numTokens <= 0) return NULL;"						"\n"
			 " " << language->prefix << "inv_token_pos = -1;"			"\n"
			 " tokens = token_list_init(numTokens, len);"				"\n"
			 " " << language->prefix << "state = " << language->startState << ";\n"
			 " while (tokens->count < numTokens) {"						"\n"
			 "  tokenId = next_token(text, &pos);"						"\n"
			 "  if (tokenId != -1) {"									"\n"
			 "   if (tokenId != -2) {"									"\n"
			 "    token_list_add(tokens, tokenId, text, pos);"			"\n"
			 "   }"														"\n"
			 "   text += pos;"											"\n"
			 "   pos = 0;"												"\n"
			 "  }"														"\n"
			 " }"														"\n"
			 " return tokens;"											"\n"
			 "}"														"\n\n";
	}

	/**
	 * Writes the tokenize_string method, which Tokenizes a string. Before tokenizing the string using the 
	 * tokenize_string_len method, it first count the number of utf-8 characters on the string.
	 * \param stringstream the stringstream to write to
	 */
	void Tokenizer::codeTokenizeString(stringstream &s) 
	{
		s << "token_list* " << fnNameTokenizeString() << "(char* text) {"					"\n"
			 " return " << fnNameTokenizeStringLen() << "(text, (int)strlen_utf8(text));"	"\n"
			 "}"																			"\n\n";
	}

	/**
	 * Writes the tokenize_file method, which tokenizes all the text on a given file. The method currently reads the 
	 * entire file to a memory buffer and then uses the tokenize_string_len method to tokenize it. The user need to have 
	 * enough memory to keep the entire file buffer.
	 * \param stringstream the stringstream to write to
	 */
	void Tokenizer::codeTokenizeFile(stringstream &s) 
	{
		s << "token_list* " << fnNameTokenizeFile() << "(char* fileName) {"	"\n"
			 " int len;"												"\n"
			 " char* text;"												"\n"
			 " token_list* tokens;"										"\n"
			 " FILE* f = fopen(fileName, \"r\");"						"\n"
			 " if (f == NULL) {"										"\n"
			 "  return NULL;"											"\n"
			 " }"														"\n"
			 " fseek(f, 0, SEEK_END);"									"\n"
			 " len = ftell(f);"											"\n"
			 " rewind(f);"												"\n"
			 " text = (char*)malloc(len);"								"\n"
			 " fread(text, 1, len, f);"									"\n"
			 " fclose(f);"												"\n";
		s << " tokens = " << fnNameTokenizeStringLen() << "(text, strlen_utf8(text)); \n"
			 " free(text);"												"\n"
			 " return tokens;"											"\n"
			 "}"														"\n\n";
	}

	/**
	 * \returns a string with all the code needed by the tokenizer.
	 */
	string Tokenizer::code() 
	{
		stringstream s;
		Code mCode;
		
		// Add the code to parse all tokens
		for (const auto& kv: typeList) 
		{
			mCode.add(&(kv.second->regex));
		}
		s << mCode.code() << endl;
		
		this->codeDefines(s);
		this->codeGlobals(s);
		this->codeNextToken(s);
		this->codeNumTokens(s);
		this->codeTokenizeStringLen(s);
		this->codeTokenizeString(s);
		this->codeTokenizeFile(s);

		return s.str();
	}

	/**
	 * \returns the name of the tokenize_string_len method for the language.
	 */
	const string Tokenizer::fnNameTokenizeStringLen() 
	{
		stringstream s;
		s << language->prefix << "tokenize_string_len";
		return s.str();
	}

	/**
	 * \returns the name of the tokenize_string method for the language.
	 */ 
	const string Tokenizer::fnNameTokenizeString() 
	{
		stringstream s;
		s << language->prefix << "tokenize_string";
		return s.str();
	}

	/**
	 * \returns the name of the tokenize_file method for the language.
	 */ 
	const string Tokenizer::fnNameTokenizeFile() 
	{
		stringstream s;
		s << language->prefix << "tokenize_file";
		return s.str();
	}

	/**
	 * \returns the name of the _get_num_tokens method for the language.
	 */ 
	const string Tokenizer::fnNameNumTokens() 
	{
		stringstream s;
		s << language->prefix << "get_num_tokens";
		return s.str();
	}

}; /* namespace pgen */