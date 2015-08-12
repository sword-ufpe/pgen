/*
 * Alternative_Test.h
 *
 *  Created on: Jun 05, 2014
 *      Author: Dimas Melo Filho
 */

#ifndef ALTERNATIVE_TEST_H_
#define ALTERNATIVE_TEST_H_

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
#include <sstream>
// pgen
#include "../expr/Alternative.h"
#include "../expr/Sequence.h"
#include "../expr/Character.h"
#include "ICompilable_Test.h"

using namespace std;
namespace pgen
{
	/**
	 * Unit Test Fixture for the "Alternative" class.
	 */
	class AlternativeTest 
	 : public CppUnit::TestFixture 
	{
	public:
		stringstream ss;
		Alternative alt;
			
		/**
		 * Returns the suite of tests for this fixture.
		 */
		static CppUnit::Test * suite() {
			CppUnit::TestSuite * s = new CppUnit::TestSuite("AlternativeTest");
			s->addTest(new CppUnit::TestCaller<AlternativeTest>("AlternativeTest::testCompile", &AlternativeTest::testCompile));
			s->addTest(new CppUnit::TestCaller<AlternativeTest>("AlternativeTest::testName", &AlternativeTest::testName));
			return s;
		}

		/**
		 * Test preparations.
		 */
		void setUp() {
			Sequence *s = new Sequence();
			Character *c = new Character('r');
			s->add(c);
			ss << c->ccompile();
			c = new Character('e');
			s->add(c);
			ss << c->compile();
			ss << s->compile();
			alt.add(s);
			s = new Sequence();
			c = new Character('d');
			s->add(c);
			ss << c->ccompile();
			c = new Character('i');
			s->add(c);
			ss << c->compile();
			ss << s->compile();
			alt.add(s);
		}

		/**
		 * Post-test procedures.
		 */
		void tearDown() {
		}

		void testCompile() {		
			ICompilableTest::compileSource(alt, Code::getHeader(), ss.str());
			CPPUNIT_ICOMPILABLE_ASSERT("re",2);
			CPPUNIT_ICOMPILABLE_ASSERT("r",255);
			CPPUNIT_ICOMPILABLE_ASSERT("e",255);
			CPPUNIT_ICOMPILABLE_ASSERT("di",2);
			CPPUNIT_ICOMPILABLE_ASSERT("d",255);
			CPPUNIT_ICOMPILABLE_ASSERT("i",255);
			ICompilableTest::clearFiles();
		}

		void testName() {
			CPPUNIT_ICOMPILABLE_ASSERTNAME(alt, "al_sq_ch_114_ch_101_sq_ch_100_ch_105");
		}
	}; /* class AlternativeTest */
} /* namespace pgen */

#endif /* CODE_TEST_H_ */
