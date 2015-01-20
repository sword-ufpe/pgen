/*
 * Test.cpp
 *
 *  Created on: Jun 13, 2014
 *      Author: Dimas Melo Filho
 */

#include "Range_Test.h"
#include "Util_Test.h"
#include "CharClass_Test.h"
#include "Quantified_Test.h"
#include "Character_Test.h"
#include "Sequence_Test.h"
#include "Text_Test.h"
#include "Regex_Test.h"
#include "Alternative_Test.h"
#include "Code_Test.h"
#include "Language_Test.h"
#include <cppunit/ui/text/TestRunner.h>

int main(int argc, char* argv[]) {
	CppUnit::TextUi::TestRunner runner;
	runner.addTest(RangeTest::suite());
	runner.addTest(UtilTest::suite());
	runner.addTest(CharClassTest::suite());
	runner.addTest(QuantifiedTest::suite());
	runner.addTest(CharacterTest::suite());
	runner.addTest(SequenceTest::suite());
	runner.addTest(TextTest::suite());
	runner.addTest(RegexTest::suite());
	runner.addTest(AlternativeTest::suite());
	runner.addTest(CodeTest::suite());
	runner.addTest(LanguageTest::suite());
	runner.run();
	return 0;
}
