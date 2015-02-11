/**
 * \author Dimas Melo Filho
 * \date 2015-01-26
 * \tabsize 4
 * \file
 * This file contains an unit test fixture for testing the LLStar and the LLStarRule classes. 
 */
 
#ifndef LLSTAR_TEST_H_
#define LLSTAR_TEST_H_

// CppUnit
#include <cppunit/TestFixture.h>
#include <cppunit/TestAssert.h>
#include <cppunit/TestSuite.h>
#include <cppunit/TestCaller.h>
#include <cppunit/Test.h>
// STL
#include <sstream>
#include <string>
#include <fstream>
// pgen
#include "../parser/LLStarRule.h"
#include "../parser/LLStar.h"
#include "../parser/Language.h"
#include "../expr/Code.h"

using namespace std;
namespace pgen
{
	class LLStarTest
	 : public CppUnit::TestFixture
	{
	private:
		Language *lang;
	public:
		
		/**
		 * Returns the suite of tests for this fixture.
		 */
		static CppUnit::Test* suite() 
		{
			using namespace CppUnit;
			TestSuite * s = new TestSuite("LLStarTest");
			s->addTest(new TestCaller<LLStarTest>("LLStarTest::testCompile", &LLStarTest::testCompile));
			return s;
		}
		
		/**
		 * Test preparations.
		 */
		void setUp() 
		{
			lang = new Language();
			lang->name = "Test";
			lang->prefix = "UT_";
			lang->stateList.insert("default");
			lang->stateList.insert("comment");
			lang->startStateName = "default";
			lang->startState = lang->getStateId(lang->startStateName);
			int comment = lang->getStateId("comment");
			int def = lang->getStateId("default");
			int* validStates;
			validStates = new int[2]; validStates[0] = def; validStates[1] = -1;
			lang->tokenizer.add("[ \\t]+","$WHITESPACE",-1,validStates);
			validStates = new int[2]; validStates[0] = def; validStates[1] = -1;
			lang->tokenizer.add("/\\*","$COMMENTSTART",comment,validStates);
			validStates = new int[2]; validStates[0] = comment; validStates[1] = -1;
			lang->tokenizer.add("\\*/","$COMMENTEND",def,validStates);
			validStates = new int[2]; validStates[0] = comment; validStates[1] = -1;
			lang->tokenizer.add(".","$COMMENT",-1,validStates);
			validStates = new int[2]; validStates[0] = def; validStates[1] = -1;			
			lang->tokenizer.add("\\d*\\.\\d+","NUMBER",-1,validStates);
			validStates = new int[2]; validStates[0] = def; validStates[1] = -1;
			lang->tokenizer.add("[\\+\\-\\*\\/]","OPERATOR",-1,validStates);
			lang->ruleList.push_back("expression");
			lang->startRule = lang->getNonTerminalId("expression");
			string exprRuleName("expression");
			LLStarRule* rule = new LLStarRule(lang, exprRuleName);
			vector<int>* symbolList = new vector<int>();
			symbolList->push_back(lang->getSymbolId("NUMBER"));
			symbolList->push_back(lang->getSymbolId("OPERATOR"));
			symbolList->push_back(lang->getSymbolId("expression"));
			rule->symbols.push_back(symbolList);
			symbolList = new vector<int>();
			symbolList->push_back(lang->getSymbolId("NUMBER"));
			rule->symbols.push_back(symbolList);
			lang->grammar = new LLStar(lang);
			lang->grammar->rules.push_back(rule);
		}
		
		/**
		 * Post-test procedures.
		 */
		void tearDown() 
		{
			delete lang;
		}
		
		/**
		 * Test the compilation of the LLStar (and LLStarRule) (GCC is required).
		 */
		void testCompile()
		{
			system("rm -f ___test___.c"); 								// remove source
			ofstream o("___test___.c");
			o << Code::helperCode << endl << lang->compile() << endl << flush <<
				 "int compare_tree(ast_node* ast, char** rep) {"			"\n"
				 " int type = 0;"											"\n"
				 " if (ast == 0) return (**rep == 0 || **rep == ')');"		"\n"
				 " while (**rep != '(') {"									"\n"
				 "  type *= 10;"											"\n"
				 "  type += **rep - '0';"									"\n"
				 "  (*rep)++;"												"\n"
				 " }"														"\n"
				 " (*rep)++;"												"\n"
				 " if (type != ast->tokenId) return 0;"						"\n"
				 " if (compare_tree(ast->firstChild, rep) == 0) return 0;"	"\n"
				 " if (**rep == ')') (*rep)++;"								"\n"
				 " if (**rep == ',') {"										"\n"
				 "  (*rep)++;"												"\n"
				 "  return compare_tree(ast->nextSibling, rep);"			"\n"
				 " }"														"\n"
				 " return 1;"												"\n"
				 "}"														"\n\n"
				 "int main(int argc, char* argv[]) {"						"\n"
				 " int pos = 0;"											"\n"
				 " char* check = argv[2];"									"\n"
				 " token_list* tokens = UT_tokenize_string(argv[1]);"		"\n"
				 " if (tokens == NULL) return 0;"							"\n"
				 " ast_node* ast = UT_parse(tokens, &pos);"					"\n"
				 " if (ast == NULL) return 0;"								"\n"
				 " return compare_tree(ast, &check);"						"\n"
				 "}"														"\n";
			o.close();
			int res = system("gcc -o ___test___ ___test___.c") >> 8;	// compile using gcc
			CPPUNIT_ASSERT( res == 0 ); 								// assert a successfull compilation
			res = system("./___test___ \"1.2 * \t /* test */ 5.11 + 2.56\" "
						 "\"1000000000(4(),5(),1000000000(4(),5(),1000000000(4())))\"") >> 8;
			CPPUNIT_ASSERT( res == 1 );
			res = system("./___test___ \"1.2 * \t /* test */ 5.11 + 2.56\" "
						 "\"1000000000(4(),5(),5(),1000000000(4(),5(),1000000000(4())))\"") >> 8;
			CPPUNIT_ASSERT( res == 0 );
			res = system("./___test___ \"1.2 ** \t /* test */ 5.11 + 2.56\" "
						 "\"1000000000(4(),5(),5(),1000000000(4(),5(),1000000000(4())))\"") >> 8;
			CPPUNIT_ASSERT( res == 0 );
		}
	}; /* class LLStarTest */
} /* namespace pgen */

#endif /* LLSTAR_TEST_H_ */
