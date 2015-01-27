/**
 * \author Dimas Melo Filho
 * \date 2014-06-13
 * \tabsize 4
 * \file
 * This is the main file of the "test" build. This file contains a main method that invokes all unit tests.
 */

// CppUnit
#include <cppunit/ui/text/TestRunner.h>
// pgen
#include "Test.h"
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
#include "TokenType_Test.h"
#include "LLStar_Test.h"

int main(int argc, char* argv[]) 
{
	CppUnit::TextUi::TestRunner runner;
	runner.addTest(pgen::RangeTest::suite());
	runner.addTest(pgen::UtilTest::suite());
	runner.addTest(pgen::CharClassTest::suite());
	runner.addTest(pgen::QuantifiedTest::suite());
	runner.addTest(pgen::CharacterTest::suite());
	runner.addTest(pgen::SequenceTest::suite());
	runner.addTest(pgen::TextTest::suite());
	runner.addTest(pgen::RegexTest::suite());
	runner.addTest(pgen::AlternativeTest::suite());
	runner.addTest(pgen::CodeTest::suite());
	runner.addTest(pgen::LanguageTest::suite());
	runner.addTest(pgen::TokenTypeTest::suite());
	runner.addTest(pgen::LLStarTest::suite());
	runner.run();
	return 0;
}
