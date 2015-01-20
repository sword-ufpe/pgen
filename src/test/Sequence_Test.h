/*
 * Sequence_Test.h
 *
 *  Created on: Jun 28, 2014
 *      Author: Dimas Melo Filho
 */

#ifndef SEQUENCE_TEST_H_
#define SEQUENCE_TEST_H_


#include <cppunit/TestFixture.h>
#include <cppunit/TestAssert.h>
#include <cppunit/TestSuite.h>
#include <cppunit/TestCaller.h>
#include <cppunit/Test.h>
#include <iostream>
#include <cstdlib>
#include <memory>
#include "../expr/Sequence.h"
#include "../expr/Character.h"
#include "ICompilable_Test.h"
using namespace std;

/**
 * Unit Test Fixture for the "Sequence" class.
 */
class SequenceTest : public CppUnit::TestFixture {
private:
	pagen::Sequence *s;
	string extra;
public:
	/**
	 * Returns the suite of tests for this fixture.
	 */
	static CppUnit::Test * suite() {
		CppUnit::TestSuite * suite = new CppUnit::TestSuite("SequenceTest");
		suite->addTest(new CppUnit::TestCaller<SequenceTest>("testAdd", &SequenceTest::testAdd));
		suite->addTest(new CppUnit::TestCaller<SequenceTest>("testCompile", &SequenceTest::testCompile));
		suite->addTest(new CppUnit::TestCaller<SequenceTest>("testName", &SequenceTest::testName));
		return suite;
	}

	/**
	 * Test preparations.
	 */
	void setUp() {
		s = new pagen::Sequence();
		pagen::Character *r = new pagen::Character('r');
		pagen::Character *e = new pagen::Character('e');
		s->add(r);
		s->add(e);
		stringstream ex;
		ex << r->ccompile() << e->compile();
		extra = ex.str();
	}

	/**
	 * Post-test procedures.
	 */
	void tearDown() {
		delete s;
	}

	/**
	 * Test the Sequence::add method.
	 */
	void testAdd() {
		CPPUNIT_ASSERT(s->expr.size() == 2);
		CPPUNIT_ICOMPILABLE_ASSERTNAME(*(s->expr[0]), "ch_114");
		CPPUNIT_ICOMPILABLE_ASSERTNAME(*(s->expr[1]), "ch_101");
	}

	/**
	 * Test the Sequence::compile and Sequence::ccompile implementations.
	 */
	void testCompile() {
		ICompilableTest::compileSource(*s,extra);
		CPPUNIT_ICOMPILABLE_ASSERT("r", 255);
		CPPUNIT_ICOMPILABLE_ASSERT("e", 255);
		CPPUNIT_ICOMPILABLE_ASSERT("re", 2);
		CPPUNIT_ICOMPILABLE_ASSERT("ren", 2);
		CPPUNIT_ICOMPILABLE_ASSERT("renata", 2);
		ICompilableTest::clearFiles();
	}

	/**
	 * Test the Sequence::name() method implementation.
	 */
	void testName() {
		CPPUNIT_ICOMPILABLE_ASSERTNAME(*s, "sq_ch_114_ch_101");
	}
};

#endif /* SEQUENCE_TEST_H_ */
