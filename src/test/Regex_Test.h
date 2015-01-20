/*
 * Text_Test.h
 *
 *  Created on: Jan 04, 2015
 *      Author: Dimas Melo Filho
 */

#ifndef REGEX_TEST_H_
#define REGEX_TEST_H_

#include <cppunit/TestFixture.h>
#include <cppunit/TestAssert.h>
#include <cppunit/TestSuite.h>
#include <cppunit/TestCaller.h>
#include <cppunit/Test.h>
#include <iostream>
#include <cstdlib>
#include <memory>
#include "../expr/Alternative.h"
#include "../expr/Regex.h"
#include "../expr/Sequence.h"
#include "../expr/Quantified.h"
#include "../expr/CharClass.h"
#include "../expr/Code.h"
#include "ICompilable_Test.h"
using namespace std;
using namespace pagen;

/**
 * Unit Test Fixture for the "Regex" class.
 */
class RegexTest : public CppUnit::TestFixture {
public:
	/**
	 * Returns the suite of tests for this fixture.
	 */
	static CppUnit::Test * suite() {
		CppUnit::TestSuite * s = new CppUnit::TestSuite("RegexTest");
		s->addTest(new CppUnit::TestCaller<RegexTest>("testParse", &RegexTest::testParse));
		return s;
	}

	/**
	 * Test preparations.
	 */
	void setUp() {
	}

	/**
	 * Post-test procedures.
	 */
	void tearDown() {
	}
	
	void testParse() {
		Regex re1("renata");
		CPPUNIT_ASSERT_EQUAL_MESSAGE("Unexpected number of objects on the regex.", 1, (int)re1.expr.size());
		int expected = Sequence::TYPE;
		CPPUNIT_ASSERT_EQUAL_MESSAGE("The regex object must contain a Sequence.", expected, re1.expr[0]->type());
		Sequence* seq = (Sequence*)re1.expr[0];
		CPPUNIT_ASSERT_EQUAL_MESSAGE("The regex object must contain a Sequence with 6 characters.", 6, (int)seq->expr.size());
		expected = Character::TYPE;
		Character* c;
		for (unsigned int i = 0; i < seq->expr.size(); i++) {
			CPPUNIT_ASSERT_EQUAL_MESSAGE("All objects of the regex sequence are expected to be characters.", expected, seq->expr[i]->type());
			c = (Character*)seq->expr[i];
			CPPUNIT_ASSERT_EQUAL_MESSAGE("Wrong character value on the regex sequence", (unsigned int)("renata"[i]), c->getChar());
		}
		
		Regex re2("a?b+c*d??e+?f*?(ghi|jkl){2,4}|[mno][^p-s]+\n");
		CPPUNIT_ASSERT_EQUAL_MESSAGE("Unexpected number of objects on the regex.", 2, (int)re2.expr.size());
		expected = Sequence::TYPE;
		CPPUNIT_ASSERT_EQUAL_MESSAGE("The regex object must contain two Sequences.", expected, re2.expr[0]->type());
		CPPUNIT_ASSERT_EQUAL_MESSAGE("The regex object must contain two Sequences.", expected, re2.expr[1]->type());
		
		seq = (Sequence*)re2.expr[0];
		CPPUNIT_ASSERT_EQUAL_MESSAGE("The first regex sequence must contain 7 elements.", 7, (int)seq->expr.size());
		expected = Quantified::TYPE;
		CPPUNIT_ASSERT_EQUAL_MESSAGE("Wrong element type", expected, seq->expr[0]->type());
		CPPUNIT_ASSERT_EQUAL_MESSAGE("Wrong element type", expected, seq->expr[1]->type());
		CPPUNIT_ASSERT_EQUAL_MESSAGE("Wrong element type", expected, seq->expr[2]->type());
		CPPUNIT_ASSERT_EQUAL_MESSAGE("Wrong element type", expected, seq->expr[3]->type());
		CPPUNIT_ASSERT_EQUAL_MESSAGE("Wrong element type", expected, seq->expr[4]->type());
		CPPUNIT_ASSERT_EQUAL_MESSAGE("Wrong element type", expected, seq->expr[5]->type());
		CPPUNIT_ASSERT_EQUAL_MESSAGE("Wrong element type", expected, seq->expr[6]->type());
		
		Quantified* q = (Quantified*)seq->expr[0];
		c = (Character*)q->expr;
		expected = Character::TYPE;
		CPPUNIT_ASSERT_EQUAL_MESSAGE("Wrong element type", expected, q->expr->type());
		CPPUNIT_ASSERT_MESSAGE("Wrong quantifier minimum", q->min == 0);
		CPPUNIT_ASSERT_MESSAGE("Wrong quantifier maximum", q->max == 1);
		CPPUNIT_ASSERT_EQUAL_MESSAGE("Wrong quantifier type", true, q->greedy);
		CPPUNIT_ASSERT_EQUAL_MESSAGE("Wrong Character Value", (unsigned int)'a', c->getChar());
		
		q = (Quantified*)seq->expr[1];
		c = (Character*)q->expr;
		CPPUNIT_ASSERT_EQUAL_MESSAGE("Wrong element type", expected, q->expr->type());
		CPPUNIT_ASSERT_MESSAGE("Wrong quantifier minimum", q->min == 1);
		CPPUNIT_ASSERT_MESSAGE("Wrong quantifier maximum", q->max == UINT_MAX);
		CPPUNIT_ASSERT_EQUAL_MESSAGE("Wrong quantifier type", true, q->greedy);
		CPPUNIT_ASSERT_EQUAL_MESSAGE("Wrong Character Value", (unsigned int)'b', c->getChar());
		
		q = (Quantified*)seq->expr[2];
		c = (Character*)q->expr;
		CPPUNIT_ASSERT_EQUAL_MESSAGE("Wrong element type", expected, q->expr->type());
		CPPUNIT_ASSERT_MESSAGE("Wrong quantifier minimum", q->min == 0);
		CPPUNIT_ASSERT_MESSAGE("Wrong quantifier maximum", q->max == UINT_MAX);
		CPPUNIT_ASSERT_EQUAL_MESSAGE("Wrong quantifier type", true, q->greedy);
		CPPUNIT_ASSERT_EQUAL_MESSAGE("Wrong Character Value", (unsigned int)'c', c->getChar());
		
		q = (Quantified*)seq->expr[3];
		c = (Character*)q->expr;
		CPPUNIT_ASSERT_EQUAL_MESSAGE("Wrong element type", expected, q->expr->type());
		CPPUNIT_ASSERT_MESSAGE("Wrong quantifier minimum", q->min == 0);
		CPPUNIT_ASSERT_MESSAGE("Wrong quantifier maximum", q->max == 1);
		CPPUNIT_ASSERT_EQUAL_MESSAGE("Wrong quantifier type", false, q->greedy);
		CPPUNIT_ASSERT_EQUAL_MESSAGE("Wrong Character Value", (unsigned int)'d', c->getChar());
		
		q = (Quantified*)seq->expr[4];
		c = (Character*)q->expr;
		CPPUNIT_ASSERT_EQUAL_MESSAGE("Wrong element type", expected, q->expr->type());
		CPPUNIT_ASSERT_MESSAGE("Wrong quantifier minimum", q->min == 1);
		CPPUNIT_ASSERT_MESSAGE("Wrong quantifier maximum", q->max == UINT_MAX);
		CPPUNIT_ASSERT_EQUAL_MESSAGE("Wrong quantifier type", false, q->greedy);
		CPPUNIT_ASSERT_EQUAL_MESSAGE("Wrong Character Value", (unsigned int)'e', c->getChar());
		
		q = (Quantified*)seq->expr[5];
		c = (Character*)q->expr;
		CPPUNIT_ASSERT_EQUAL_MESSAGE("Wrong element type", expected, q->expr->type());
		CPPUNIT_ASSERT_MESSAGE("Wrong quantifier minimum", q->min == 0);
		CPPUNIT_ASSERT_MESSAGE("Wrong quantifier maximum", q->max == UINT_MAX);
		CPPUNIT_ASSERT_EQUAL_MESSAGE("Wrong quantifier type", false, q->greedy);
		CPPUNIT_ASSERT_EQUAL_MESSAGE("Wrong Character Value", (unsigned int)'f', c->getChar());
		
		q = (Quantified*)seq->expr[6];
		expected = Alternative::TYPE;
		CPPUNIT_ASSERT_EQUAL_MESSAGE("Wrong element type", expected, q->expr->type());
		CPPUNIT_ASSERT_MESSAGE("Wrong quantifier minimum", q->min == 2);
		CPPUNIT_ASSERT_MESSAGE("Wrong quantifier maximum", q->max == 4);
		CPPUNIT_ASSERT_EQUAL_MESSAGE("Wrong quantifier type", true, q->greedy);
		Alternative* a = (Alternative*)q->expr;
		CPPUNIT_ASSERT_MESSAGE("Wrong number of alternatives", a->expr.size() == 2);
		expected = Sequence::TYPE;
		CPPUNIT_ASSERT_EQUAL_MESSAGE("Expected a sequence object on the alternative", expected, a->expr[0]->type());
		CPPUNIT_ASSERT_EQUAL_MESSAGE("Expected a sequence object on the alternative", expected, a->expr[1]->type());
		
		seq = (Sequence*)a->expr[0];
		CPPUNIT_ASSERT_MESSAGE("Wrong number of elements on sequence", seq->expr.size() == 3);
		expected = Character::TYPE;
		CPPUNIT_ASSERT_EQUAL_MESSAGE("Wrong type of element on the sequence", expected, seq->expr[0]->type());
		CPPUNIT_ASSERT_EQUAL_MESSAGE("Wrong type of element on the sequence", expected, seq->expr[1]->type());
		CPPUNIT_ASSERT_EQUAL_MESSAGE("Wrong type of element on the sequence", expected, seq->expr[2]->type());
		c = (Character*) seq->expr[0];
		CPPUNIT_ASSERT_EQUAL_MESSAGE("Wrong value on character element", (unsigned int)'g', c->getChar());
		c = (Character*) seq->expr[1];
		CPPUNIT_ASSERT_EQUAL_MESSAGE("Wrong value on character element", (unsigned int)'h', c->getChar());
		c = (Character*) seq->expr[2];
		CPPUNIT_ASSERT_EQUAL_MESSAGE("Wrong value on character element", (unsigned int)'i', c->getChar());
		
		seq = (Sequence*)a->expr[1];
		CPPUNIT_ASSERT_MESSAGE("Wrong number of elements on sequence", seq->expr.size() == 3);
		expected = Character::TYPE;
		CPPUNIT_ASSERT_EQUAL_MESSAGE("Wrong type of element on the sequence", expected, seq->expr[0]->type());
		CPPUNIT_ASSERT_EQUAL_MESSAGE("Wrong type of element on the sequence", expected, seq->expr[1]->type());
		CPPUNIT_ASSERT_EQUAL_MESSAGE("Wrong type of element on the sequence", expected, seq->expr[2]->type());
		c = (Character*) seq->expr[0];
		CPPUNIT_ASSERT_EQUAL_MESSAGE("Wrong value on character element", (unsigned int)'j', c->getChar());
		c = (Character*) seq->expr[1];
		CPPUNIT_ASSERT_EQUAL_MESSAGE("Wrong value on character element", (unsigned int)'k', c->getChar());
		c = (Character*) seq->expr[2];
		CPPUNIT_ASSERT_EQUAL_MESSAGE("Wrong value on character element", (unsigned int)'l', c->getChar());
		
		seq = (Sequence*)re2.expr[1];
		CPPUNIT_ASSERT_EQUAL_MESSAGE("The second regex sequence must contain 3 elements.", 3, (int)seq->expr.size());
		expected = CharClass::TYPE;
		CPPUNIT_ASSERT_EQUAL_MESSAGE("Wrong element type", expected, seq->expr[0]->type());
		expected = Quantified::TYPE;
		CPPUNIT_ASSERT_EQUAL_MESSAGE("Wrong element type", expected, seq->expr[1]->type());
		expected = Character::TYPE;
		CPPUNIT_ASSERT_EQUAL_MESSAGE("Wrong element type", expected, seq->expr[2]->type());
		
		CharClass *h = (CharClass*) seq->expr[0];
		CPPUNIT_ASSERT_EQUAL_MESSAGE("Wrong range start on character class", (unsigned int)'m', h->range->start);
		CPPUNIT_ASSERT_EQUAL_MESSAGE("Wrong range end on character class", (unsigned int)'o', h->range->end);
		
		q = (Quantified*) seq->expr[1];
		CPPUNIT_ASSERT_EQUAL_MESSAGE("Wrong quantifier minimum", (unsigned int)1, q->min);
		CPPUNIT_ASSERT_EQUAL_MESSAGE("Wrong quantifier maximum", UINT_MAX, q->max);
		CPPUNIT_ASSERT_EQUAL_MESSAGE("Wrong quantifier type", true, q->greedy);
		expected = CharClass::TYPE;
		CPPUNIT_ASSERT_EQUAL_MESSAGE("Wrong quantified element type", expected, q->expr->type());
		
		h = (CharClass*) q->expr;
		CPPUNIT_ASSERT_MESSAGE("Wrong range start on character class", h->range->start == 1);
		CPPUNIT_ASSERT_MESSAGE("Wrong range end on character class", h->range->end == 'o');
		CPPUNIT_ASSERT_MESSAGE("Wrong range start on character class", h->range->next != nullptr);
		CPPUNIT_ASSERT_MESSAGE("Wrong range start on character class", h->range->next->start == 't');
		CPPUNIT_ASSERT_MESSAGE("Wrong range end on character class", h->range->next->end == UINT_MAX);
		
		c = (Character*) seq->expr[2];
		CPPUNIT_ASSERT_EQUAL_MESSAGE("Invalid character value", (unsigned int) 10, c->getChar());
		
	}
};




#endif /* REGEX_TEST_H_ */
