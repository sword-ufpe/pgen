/*
 * Text_Test.h
 *
 *  Created on: Jan 04, 2015
 *      Author: Dimas Melo Filho
 */

#ifndef TEXT_TEST_H_
#define TEXT_TEST_H_

// CppUnit
#include <cppunit/TestFixture.h>
#include <cppunit/TestAssert.h>
#include <cppunit/TestSuite.h>
#include <cppunit/TestCaller.h>
#include <cppunit/Test.h>
// STL
#include <iostream>
#include <cstdlib>
#include <memory>
// pgen
#include "../expr/Text.h"
#include "ICompilable_Test.h"

using namespace std;
namespace pgen
{
	/**
	 * Unit Test Fixture for the "Text" class.
	 */
	class TextTest 
	 : public CppUnit::TestFixture 
	{
	public:
		/**
		 * Returns the suite of tests for this fixture.
		 */
		static CppUnit::Test * suite() 
		{
			CppUnit::TestSuite * s = new CppUnit::TestSuite("TextTest");
			s->addTest(new CppUnit::TestCaller<TextTest>("TextTest::testCompile", &TextTest::testCompile));
			s->addTest(new CppUnit::TestCaller<TextTest>("TextTest::testName", &TextTest::testName));
			return s;
		}

		/**
		 * Test preparations.
		 */
		void setUp() 
		{
		}

		/**
		 * Post-test procedures.
		 */
		void tearDown() 
		{
		}

		void testCompile() 
		{
			// test with the text "text"
			Text text("text");
			ICompilableTest::compileSource(text, pgen::Code::getHeader());
			CPPUNIT_ICOMPILABLE_ASSERT("taxt", 255);
			CPPUNIT_ICOMPILABLE_ASSERT("text", 4);
			// test the text "\a\b\t\n\v\f\r\"\'\\"
			Text escape("\a\b\t\n\v\f\r\"\'\\");
			ICompilableTest::compileSource(escape, pgen::Code::getHeader());
			CPPUNIT_ICOMPILABLE_ASSERT("\a\b\t\n\v\f\r\"\'\\", 10);
			CPPUNIT_ICOMPILABLE_ASSERT("\x07\x08\x09\x0A\x0B\x0C\x0D\x22\x27\x5C", 10);
			CPPUNIT_ICOMPILABLE_ASSERT("abtnvfr\"'\\", 255);
			Text special("\x01\x02\x03\x04\x05\x06");
			ICompilableTest::compileSource(special, pgen::Code::getHeader());
			CPPUNIT_ICOMPILABLE_ASSERT("\x01\x02\x03\x04\x05\x06", 6);
			CPPUNIT_ICOMPILABLE_ASSERT("123456", 255);
			ICompilableTest::clearFiles();
		}

		void testName() 
		{
			Text text("text");
			Text escape("\a\b\t\n\v\f\r\"\'\\");
			Text special("\x01\x02\x03\x04\x05\x06");
			CPPUNIT_ICOMPILABLE_ASSERTNAME(text, "tx_text");
			CPPUNIT_ICOMPILABLE_ASSERTNAME(escape, "tx__7__8__9__10__11__12__13__34__39__92_");
			CPPUNIT_ICOMPILABLE_ASSERTNAME(special, "tx__1__2__3__4__5__6_");
		}
		
	}; /* class TextTest */
} /* namespace pgen */

#endif /* TEXT_TEST_H_ */
