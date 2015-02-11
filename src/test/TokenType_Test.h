/**
 * \author Dimas Melo Filho
 * \date 2015-01-06
 * \tabsize 4
 * \file
 * This file contains an unit test fixture for testing the TokenType class. 
 */
 
#ifndef TOKENTYPE_TEST_H_
#define TOKENTYPE_TEST_H_

// CppUnit
#include <cppunit/TestFixture.h>
#include <cppunit/TestAssert.h>
#include <cppunit/TestSuite.h>
#include <cppunit/TestCaller.h>
#include <cppunit/Test.h>
// STL
#include <string>
#include <fstream>
// pgen
#include "../parser/TokenType.h"
#include "../expr/Alternative.h"
#include "../expr/Sequence.h"

using namespace std;
namespace pgen 
{
	class TokenTypeTest 
	 : public CppUnit::TestFixture 
	{
	public:
		Language* lang;
		
		/**
		 * Returns the suite of tests for this fixture.
		 */
		static CppUnit::Test* suite() 
		{
			using namespace CppUnit;
			TestSuite * s = new TestSuite("TokenTypeTest");
			s->addTest(new TestCaller<TokenTypeTest>("TokenTypeTest::testConstructor", &TokenTypeTest::testConstructor));
			s->addTest(new TestCaller<TokenTypeTest>("TokenTypeTest::testCompile", &TokenTypeTest::testCompile));
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
		}
		
		/**
		 * Post-test procedures.
		 */
		void tearDown() 
		{
			delete lang;
		}
		
		/**
		 * Test the TokenType constructor.
		 */
		void testConstructor() 
		{
			int* validStates = (int*)malloc(sizeof(int)*2);
			validStates[0] = 0;
			validStates[1] = -1;
			TokenType t(lang, "\\d+.\\d+\\n|ab", "TEST_TOKEN", 1, true, 0, validStates);
			string testToken("TEST_TOKEN");
			CPPUNIT_ASSERT_EQUAL(lang, t.language);
			CPPUNIT_ASSERT_EQUAL(testToken, t.name);
			CPPUNIT_ASSERT_EQUAL(1, t.typeId);
			CPPUNIT_ASSERT_EQUAL(true, t.discard);
			CPPUNIT_ASSERT_EQUAL(0, t.setState);
			CPPUNIT_ASSERT_EQUAL(0, t.validStates[0]);
			CPPUNIT_ASSERT_EQUAL(-1, t.validStates[1]);
			CPPUNIT_ASSERT(Alternative::TYPE == t.regex.type());
			CPPUNIT_ASSERT_EQUAL((unsigned int)2, (unsigned int)t.regex.expr.size());
			CPPUNIT_ASSERT(Sequence::TYPE == t.regex.expr[0]->type());
			CPPUNIT_ASSERT_EQUAL((unsigned int)4, (unsigned int)reinterpret_cast<Sequence*>(t.regex.expr[0])->expr.size());
			CPPUNIT_ASSERT(Sequence::TYPE == t.regex.expr[1]->type());
			CPPUNIT_ASSERT_EQUAL((unsigned int)2, (unsigned int)reinterpret_cast<Sequence*>(t.regex.expr[1])->expr.size());
		}
		
		/**
		 * Test the compilation of the TokenType (GCC is required).
		 */
		void testCompile()
		{
			int* validStates = (int*)malloc(sizeof(int)*2);
			validStates[0] = 0;
			validStates[1] = -1;
			TokenType t(lang, "\\d+\\.\\d+|ab", "TEST_TOKEN", 1, true, 0, validStates);
			string testToken("TEST_TOKEN");
			Code c;
			c.add(&t.regex);
			system("rm -f ___test___.c"); 								// remove source
			ofstream o("___test___.c");
			o << c.helper() << endl << endl << c.code() << endl << endl;
			o << "int main(int argc, char* argv[]) {"	"\n"
				 " int UT_state = 0;"					"\n"
				 " int _pos = 0;"						"\n"
				 " int* pos = &_pos;"					"\n"
				 " char* text = argv[1];"				"\n"
			  << t.code() <<							"\n"
				 " return -1;"							"\n"
				 "}"									"\n"
			  << flush;
			o.close();
			int res = system("gcc -o ___test___ ___test___.c") >> 8;	// compile using gcc
			CPPUNIT_ASSERT( res == 0 ); 								// assert a successfull compilation
			res = system("./___test___ 123.456") >> 8;
			CPPUNIT_ASSERT_EQUAL(254, res);
			res = system("./___test___ 1.2") >> 8;
			CPPUNIT_ASSERT_EQUAL(254, res);
			res = system("./___test___ 123x456") >> 8;
			CPPUNIT_ASSERT_EQUAL(255, res);
			res = system("./___test___ .456") >> 8;
			CPPUNIT_ASSERT_EQUAL(255, res);
			res = system("./___test___ a123.456") >> 8;
			CPPUNIT_ASSERT_EQUAL(255, res);
			res = system("./___test___ 12a3.456") >> 8;
			CPPUNIT_ASSERT_EQUAL(255, res);
			res = system("./___test___ 123.456a") >> 8;
			CPPUNIT_ASSERT_EQUAL(254, res);
			res = system("./___test___ 123.4a32") >> 8;
			CPPUNIT_ASSERT_EQUAL(254, res);
			res = system("./___test___ 123.a523") >> 8;
			CPPUNIT_ASSERT_EQUAL(255, res);
		}
		
	};
};

 
 
#endif /* TOKENTYPE_TEST_H_ */