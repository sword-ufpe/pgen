/*
 * CharClass_Test.h
 *
 *  Created on: Jun 14, 2014
 *      Author: Dimas Melo Filho
 */

#ifndef CHARCLASS_TEST_H_
#define CHARCLASS_TEST_H_

#include <cppunit/TestFixture.h>
#include <cppunit/TestAssert.h>
#include <cppunit/TestSuite.h>
#include <cppunit/TestCaller.h>
#include <cppunit/Test.h>
#include <iostream>
#include <climits>
#include "../expr/CharClass.h"
#include "ICompilable_Test.h"
using namespace std;

/**
 * Unit test fixture for the "CharClass" class.
 */
class CharClassTest : public CppUnit::TestFixture {
private:
	pgen::CharClass * il;
public:
	static CppUnit::Test * suite() {
		CppUnit::TestSuite * s = new CppUnit::TestSuite("CharClassTest");
		s->addTest(new CppUnit::TestCaller<CharClassTest>("Test CharClass::add(s,e)", &CharClassTest::testAddInterval));
		s->addTest(new CppUnit::TestCaller<CharClassTest>("Test CharClass::invert()", &CharClassTest::testInvert));
		s->addTest(new CppUnit::TestCaller<CharClassTest>("Test CharClass::normalize()", &CharClassTest::testNormalize));
		s->addTest(new CppUnit::TestCaller<CharClassTest>("Test CharClass::operator==() and CharClass::operator!=()", &CharClassTest::testOperators));
		s->addTest(new CppUnit::TestCaller<CharClassTest>("Test CharClass::compile", &CharClassTest::testCompile));
		s->addTest(new CppUnit::TestCaller<CharClassTest>("Test CharClass::name", &CharClassTest::testName));
		return s;
	}
	void setUp() {
	}

	void tearDown() {
	}

	void testAddInterval() {
		il = new pgen::CharClass();
		CPPUNIT_ASSERT( il->range == nullptr );
		il->add(10);
		CPPUNIT_ASSERT( il->range != nullptr );
		CPPUNIT_ASSERT( il->range->prev == nullptr );
		CPPUNIT_ASSERT( il->range->next == nullptr );
		CPPUNIT_ASSERT( il->range->start == 10 );
		CPPUNIT_ASSERT( il->range->end == 10 );
		il->add(11);
		CPPUNIT_ASSERT( il->range->prev == nullptr );
		CPPUNIT_ASSERT( il->range->next == nullptr );
		CPPUNIT_ASSERT( il->range->start == 10 );
		CPPUNIT_ASSERT( il->range->end == 11 );
		il->add(13);
		CPPUNIT_ASSERT( il->range->prev == nullptr );
		CPPUNIT_ASSERT( il->range->next != nullptr );
		CPPUNIT_ASSERT( il->range->start == 10 );
		CPPUNIT_ASSERT( il->range->end == 11 );
		CPPUNIT_ASSERT( il->range->next->start == 13 );
		CPPUNIT_ASSERT( il->range->next->end == 13 );
		il->add(12,14);
		CPPUNIT_ASSERT( il->range->prev == nullptr );
		CPPUNIT_ASSERT( il->range->next == nullptr );
		CPPUNIT_ASSERT( il->range->start == 10 );
		CPPUNIT_ASSERT( il->range->end == 14 );
		il->add(5,9);
		CPPUNIT_ASSERT( il->range->prev == nullptr );
		CPPUNIT_ASSERT( il->range->next == nullptr );
		CPPUNIT_ASSERT( il->range->start == 5 );
		CPPUNIT_ASSERT( il->range->end == 14 );
		il->add(9,12);
		CPPUNIT_ASSERT( il->range->prev == nullptr );
		CPPUNIT_ASSERT( il->range->next == nullptr );
		CPPUNIT_ASSERT( il->range->start == 5 );
		CPPUNIT_ASSERT( il->range->end == 14 );
		il->add(16,20);
		il->add(15);
		CPPUNIT_ASSERT( il->range->prev == nullptr );
		CPPUNIT_ASSERT( il->range->next == nullptr );
		CPPUNIT_ASSERT( il->range->start == 5 );
		CPPUNIT_ASSERT( il->range->end == 20 );
		delete il;
	}

	void testInvert() {
		il = new pgen::CharClass();
		il->add(10,20);
		il->add(110,120);
		il->invert();
		CPPUNIT_ASSERT(il->range != nullptr);
		CPPUNIT_ASSERT(il->range->start == 1);
		CPPUNIT_ASSERT(il->range->end == 9);
		CPPUNIT_ASSERT(il->range->next != nullptr);
		CPPUNIT_ASSERT(il->range->next->start == 21);
		CPPUNIT_ASSERT(il->range->next->end == 109);
		CPPUNIT_ASSERT(il->range->next->next != nullptr);
		CPPUNIT_ASSERT(il->range->next->next->start == 121);
		CPPUNIT_ASSERT(il->range->next->next->end == UINT_MAX);
		CPPUNIT_ASSERT(il->range->next->next->next == nullptr);
		il->invert();
		CPPUNIT_ASSERT(il->range != nullptr);
		CPPUNIT_ASSERT(il->range->start == 10);
		CPPUNIT_ASSERT(il->range->end == 20);
		CPPUNIT_ASSERT(il->range->next != nullptr);
		CPPUNIT_ASSERT(il->range->next->start == 110);
		CPPUNIT_ASSERT(il->range->next->end == 120);
		CPPUNIT_ASSERT(il->range->next->next == nullptr);
		delete il;
		// test the inversion of a class that contains all characters
		il = new pgen::CharClass();
		il->add(0,UINT_MAX);
		CPPUNIT_ASSERT(il->range != nullptr);
		il->invert();
		// test the inversion of a class that contains no characters.
		CPPUNIT_ASSERT(il->range == nullptr);
		il->invert();
		CPPUNIT_ASSERT(il->range != nullptr);
		CPPUNIT_ASSERT(il->range->start == 1);
		CPPUNIT_ASSERT(il->range->end == UINT_MAX);
		delete il;
		// test the inversion of a class that contains only the 1 (0 gets added as 1)
		il = new pgen::CharClass();
		il->add((unsigned int)0);
		CPPUNIT_ASSERT(il->range != nullptr);
		CPPUNIT_ASSERT(il->range->start == 1);
		CPPUNIT_ASSERT(il->range->end == 1);
		il->invert();
		CPPUNIT_ASSERT(il->range != nullptr);
		CPPUNIT_ASSERT(il->range->start == 2);
		CPPUNIT_ASSERT(il->range->end == UINT_MAX);
		CPPUNIT_ASSERT(il->range->next == nullptr);
		delete il;
		// test the inversion of a class that contains only the UINT_MAX
		il = new pgen::CharClass();
		il->add(UINT_MAX);
		CPPUNIT_ASSERT(il->range != nullptr);
		CPPUNIT_ASSERT(il->range->start == UINT_MAX);
		CPPUNIT_ASSERT(il->range->end == UINT_MAX);
		il->invert();
		CPPUNIT_ASSERT(il->range != nullptr);
		CPPUNIT_ASSERT(il->range->start == 1);
		CPPUNIT_ASSERT(il->range->end == UINT_MAX-1);
		CPPUNIT_ASSERT(il->range->next == nullptr);
		delete il;
	}

	void testNormalize() {
		il = new pgen::CharClass();
		il->negated = true;
		il->add((unsigned int)0);									// since 0 is not accepted, 1 will be added instead
		CPPUNIT_ASSERT(il->range != nullptr);
		CPPUNIT_ASSERT(il->range->start == 1);
		CPPUNIT_ASSERT(il->range->end == 1);
		CPPUNIT_ASSERT(il->negated == true);
		il->normalize();							// invert the 1 that was added, now it should start at 2 and end at UINT_MAX
		CPPUNIT_ASSERT(il->range != nullptr);
		CPPUNIT_ASSERT(il->range->start == 2);
		CPPUNIT_ASSERT(il->range->end == UINT_MAX);
		CPPUNIT_ASSERT(il->negated == false);
		il->normalize();							// nothing is changed since it is already normalized
		CPPUNIT_ASSERT(il->range != nullptr);
		CPPUNIT_ASSERT(il->range->start == 2);
		CPPUNIT_ASSERT(il->range->end == UINT_MAX);
		CPPUNIT_ASSERT(il->negated == false);
		delete il;
	}

	void testOperators() {
		il = new pgen::CharClass();
		pgen::CharClass * il_eq = new pgen::CharClass(),
					 * il_neq1 = new pgen::CharClass(),
					 * il_neq2 = new pgen::CharClass(),
					 * il_neq3 = new pgen::CharClass();
		il->add(10,20);
		il->add(30,40);
		il_eq->add(10,20);
		il_eq->add(30,40);
		il_neq1->add(10,40);
		il_neq2->add(10,20);
		il_neq2->add(31,40);
		il_neq3->add(10,20);
		il_neq3->add(30,40);
		il_neq3->negated = true;
		CPPUNIT_ASSERT(*il == *il_eq);
		CPPUNIT_ASSERT(*il != *il_neq1);
		CPPUNIT_ASSERT(*il != *il_neq2);
		CPPUNIT_ASSERT(*il != *il_neq3);
		il_neq3->normalize();
		CPPUNIT_ASSERT(*il != *il_neq3);
		delete il;
		delete il_eq;
		delete il_neq1;
		delete il_neq2;
		delete il_neq3;

	}

	void testCompile() {
		il = new pgen::CharClass();
		il->add(0x20);
		il->add(0x30,0x40);
		ICompilableTest::compileSource(*il);
		ICompilableTest::assert("\x1F",255,__FILE__,__LINE__);
		ICompilableTest::assert("\x20",1,__FILE__,__LINE__);
		ICompilableTest::assert("\x21",255,__FILE__,__LINE__);
		ICompilableTest::assert("\x2F",255,__FILE__,__LINE__);
		ICompilableTest::assert("\x30",1,__FILE__,__LINE__);
		ICompilableTest::assert("\x40",1,__FILE__,__LINE__);
		ICompilableTest::assert("\x41",255,__FILE__,__LINE__);
		ICompilableTest::clearFiles();
	}

	void testName() {
		il = new pgen::CharClass();
		il->add(10);
		il->add(20,30);
		il->add(30,40);
		CPPUNIT_ICOMPILABLE_ASSERTNAME(*il, "cl_10_10_20_40");
		delete il;
		il = new pgen::CharClass();
		il->add(10,12);
		il->add(13);
		il->add(11);
		CPPUNIT_ICOMPILABLE_ASSERTNAME(*il, "cl_10_13");
	}

};


#endif /* CHARCLASS_TEST_H_ */
