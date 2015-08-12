/*
 * Quantified_Test.h
 *
 *  Created on: Jun 22, 2014
 *      Author: Dimas Melo Filho
 */

#ifndef QUANTIFIED_TEST_H_
#define QUANTIFIED_TEST_H_

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
#include "../expr/Quantified.h"
#include "../expr/Range.h"
#include "../expr/CharClass.h"
#include "../expr/Code.h"
#include "ICompilable_Test.h"

using namespace std;
namespace pgen
{
	/**
	 * Unit Test Fixture for the "Quantified" class.
	 */
	class QuantifiedTest 
	 : public CppUnit::TestFixture 
	{
	private:
		pgen::Range *single, *normal, *inverted;
	public:
		/**
		 * Returns the suite of tests for this fixture.
		 */
		static CppUnit::Test * suite() 
		{
			CppUnit::TestSuite * s = new CppUnit::TestSuite("QuantifiedTest");
			s->addTest(new CppUnit::TestCaller<QuantifiedTest>("QuantifiedTest::testCompile", &QuantifiedTest::testCompile));
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
			// test with a digit quantified between 2 an 5 times [0-9]{2,5} (greedy)
			Range *r = new Range('0','9');
			Quantified *q = new Quantified(r, 2, 5, true);
			ICompilableTest::compileSource(*q, Code::getHeader(), q->expr->compile());
			ICompilableTest::assert("0", 255,__FILE__,__LINE__); 		// false
			ICompilableTest::assert("00", 2,__FILE__,__LINE__);		// true
			ICompilableTest::assert("09", 2,__FILE__,__LINE__);		// true
			ICompilableTest::assert("90", 2,__FILE__,__LINE__);		// true
			ICompilableTest::assert("99", 2,__FILE__,__LINE__);		// true
			ICompilableTest::assert("9", 255,__FILE__,__LINE__);		// false
			ICompilableTest::assert("A", 255,__FILE__,__LINE__);		// false
			ICompilableTest::assert("A0", 255,__FILE__,__LINE__);		// false
			ICompilableTest::assert("0A", 255,__FILE__,__LINE__);		// false
			ICompilableTest::assert("000",3,__FILE__,__LINE__);		// true
			ICompilableTest::assert("00A",2,__FILE__,__LINE__);		// true (reads only the first two 0s).
			ICompilableTest::assert("0000",4,__FILE__,__LINE__);		// true
			ICompilableTest::assert("00000",5,__FILE__,__LINE__);		// true
			ICompilableTest::assert("0000000000", 5,__FILE__,__LINE__); // true
			CPPUNIT_ICOMPILABLE_ASSERTNAME(*q, "qtg_2_5_ra_48_57");
			delete q;
			// Test with an hexadecimal digit quantified between 0-3 times [0-9A-Fa-f]*? (lazy)
			CharClass *c = new CharClass();
			c->add('0','9');
			c->add('A','F');
			c->add('a','f');
			q = new Quantified(c, 0, 3, false);
			// since the expression can match 0 to 3 characters, all expressions are valid (matching 0 characters).
			// since it is a lazy match, they all should match 0 characters.
			// the lazy match is useful when chained.
			ICompilableTest::compileSource(*q, Code::getHeader(), q->expr->compile());
			ICompilableTest::assert("a",0, __FILE__,__LINE__);
			ICompilableTest::assert("ab",0, __FILE__,__LINE__);
			ICompilableTest::assert("abc",0, __FILE__,__LINE__);;
			ICompilableTest::assert("0Aa", 0, __FILE__,__LINE__);
			ICompilableTest::assert("fF9",0, __FILE__,__LINE__);
			ICompilableTest::assert("abX",0, __FILE__,__LINE__);
			ICompilableTest::assert("aX",0, __FILE__,__LINE__);
			ICompilableTest::assert("X",0, __FILE__,__LINE__);
			ICompilableTest::assert("0123456",0, __FILE__,__LINE__);
			CPPUNIT_ICOMPILABLE_ASSERTNAME(*q, "qtl_0_3_cl_48_57_65_70_97_102");
			delete q;
			ICompilableTest::clearFiles();
		}

	}; /* class QuantifiedTest */
} /* namespace pgen */

#endif /* QUANTIFIED_TEST_H_ */
