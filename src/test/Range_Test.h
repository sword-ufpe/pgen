/*
 * Range_Test.h
 *
 *  Created on: Jun 13, 2014
 *      Author: Dimas Melo Filho
 */

#ifndef RANGE_TEST_H_
#define RANGE_TEST_H_

// CppUnit
#include <cppunit/TestFixture.h>
#include <cppunit/TestAssert.h>
#include <cppunit/TestSuite.h>
#include <cppunit/TestCaller.h>
#include <cppunit/Test.h>
// STL
#include <fstream>
#include <iostream>
#include <sstream>
#include <cstdlib>
// pgen
#include "../expr/Range.h"
#include "../expr/Code.h"
#include "ICompilable_Test.h"

using namespace std;
namespace pgen
{
	/**
	 * Unit Test Fixture for the "Range" class.
	 */
	class RangeTest 
	 : public CppUnit::TestFixture 
	{
	private:
		Range *single, *normal, *inverted;
	public:
		/**
		 * Returns the suite of tests for this fixture.
		 */
		static CppUnit::Test * suite() {
			CppUnit::TestSuite * s = new CppUnit::TestSuite("RangeTest");
			s->addTest(new CppUnit::TestCaller<RangeTest>("RangeTest::testSingle", &RangeTest::testSingle));
			s->addTest(new CppUnit::TestCaller<RangeTest>("RangeTest::testNormal", &RangeTest::testNormal));
			s->addTest(new CppUnit::TestCaller<RangeTest>("RangeTest::testInverted", &RangeTest::testInverted));
			s->addTest(new CppUnit::TestCaller<RangeTest>("RangeTest::testCompile", &RangeTest::testCompile));
			s->addTest(new CppUnit::TestCaller<RangeTest>("RangeTest::testName", &RangeTest::testName));
			return s;
		}

		/**
		 * Test preparations.
		 */
		void setUp() 
		{
			single = new Range(10,10);
			normal = new Range(100,200);
			inverted = new Range(200,100);
		}

		/**
		 * Post-test procedures.
		 */
		void tearDown() 
		{
			delete single;
			delete normal;
			delete inverted;
		}

		/**
		 * Test signle character ranges.
		 */
		void testSingle() 
		{
			CPPUNIT_ASSERT( !single->in(9) );
			CPPUNIT_ASSERT( single->in(10) );
			CPPUNIT_ASSERT( !single->in(11) );
			CPPUNIT_ASSERT( single->before(9) );
			CPPUNIT_ASSERT( !single->before(10) );
			CPPUNIT_ASSERT( !single->before(11) );
			CPPUNIT_ASSERT( !single->after(9) );
			CPPUNIT_ASSERT( !single->after(10) );
			CPPUNIT_ASSERT( single->after(11) );
			// TODO 999 Test Range::join, operator== and operator!= for single characacter ranges.
		}

		/**
		 * Test Normal Ranges.
		 */
		void testNormal() 
		{
			CPPUNIT_ASSERT( !normal->in(99) );
			CPPUNIT_ASSERT( normal->in(100) );
			CPPUNIT_ASSERT( normal->in(150) );
			CPPUNIT_ASSERT( normal->in(200) );
			CPPUNIT_ASSERT( !normal->in(201) );
			CPPUNIT_ASSERT( normal->before(99) );
			CPPUNIT_ASSERT( !normal->before(100) );
			CPPUNIT_ASSERT( !normal->before(150) );
			CPPUNIT_ASSERT( !normal->before(200) );
			CPPUNIT_ASSERT( !normal->before(201) );
			CPPUNIT_ASSERT( !normal->after(99) );
			CPPUNIT_ASSERT( !normal->after(100) );
			CPPUNIT_ASSERT( !normal->after(150) );
			CPPUNIT_ASSERT( !normal->after(200) );
			CPPUNIT_ASSERT( normal->after(201) );
			// TODO 999 test Range::join, operator== and operator!= for normal ranges.
		}

		void testInverted() 
		{
			CPPUNIT_ASSERT( !inverted->in(99) );
			CPPUNIT_ASSERT( inverted->in(100) );
			CPPUNIT_ASSERT( inverted->in(150) );
			CPPUNIT_ASSERT( inverted->in(200) );
			CPPUNIT_ASSERT( !inverted->in(201) );
			CPPUNIT_ASSERT( inverted->before(99) );
			CPPUNIT_ASSERT( !inverted->before(100) );
			CPPUNIT_ASSERT( !inverted->before(150) );
			CPPUNIT_ASSERT( !inverted->before(200) );
			CPPUNIT_ASSERT( !inverted->before(201) );
			CPPUNIT_ASSERT( !inverted->after(99) );
			CPPUNIT_ASSERT( !inverted->after(100) );
			CPPUNIT_ASSERT( !inverted->after(150) );
			CPPUNIT_ASSERT( !inverted->after(200) );
			CPPUNIT_ASSERT( inverted->after(201) );
			// There is no need to test other methods for inverted ranges, if they are inverted
			// correctly.
		}

		void testCompile() 
		{
			Range *r = new Range(33,1000);
			ICompilableTest::compileSource(*r, pgen::Code::getHeader());
			ICompilableTest::assert("\x20", 255,__FILE__,__LINE__); 		//32 (false)
			ICompilableTest::assert("\x21", 1,__FILE__,__LINE__);			//33 (true)
			ICompilableTest::assert("\xCF\xA8",2,__FILE__,__LINE__);		//1000 (true)
			ICompilableTest::assert("\xCF\xA9",255,__FILE__,__LINE__);		//1001 (false)
			ICompilableTest::clearFiles();
		}

		void testName() 
		{
			CPPUNIT_ICOMPILABLE_ASSERTNAME( *single, "ra_10_10" );
			CPPUNIT_ICOMPILABLE_ASSERTNAME( *normal, "ra_100_200" );
			CPPUNIT_ICOMPILABLE_ASSERTNAME( *inverted, "ra_100_200" );
		}
		
	}; /* class RangeTest */
} /* namespace pgen */

#endif /* RANGE_TEST_H_ */
