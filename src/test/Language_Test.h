/*
 * Language_Test.h
 *
 *  Created on: Jan 07, 2015
 *      Author: Dimas Melo Filho
 */

#ifndef LANGUAGE_TEST_H_
#define LANGUAGE_TEST_H_

// CppUnit
#include <cppunit/TestFixture.h>
#include <cppunit/TestAssert.h>
#include <cppunit/TestSuite.h>
#include <cppunit/TestCaller.h>
#include <cppunit/Test.h>
// STL
#include <iostream>
#include <sstream>
// pgen
#include "../parser/Language.h"
#include "ICompilable_Test.h"

using namespace std;
namespace pgen
{

	/**
	 * Unit Test Fixture for the "Language" class.
	 */
	class LanguageTest 
	 : public CppUnit::TestFixture 
	{
	public:
		stringstream ss;
			
		/**
		 * Returns the suite of tests for this fixture.
		 */
		static CppUnit::Test * suite() 
		{
			CppUnit::TestSuite * s = new CppUnit::TestSuite("LanguageTest");
			s->addTest(new CppUnit::TestCaller<LanguageTest>("testCompile", &LanguageTest::testCompile));
			return s;
		}

		/**
		 * Test preparations.
		 */
		void setUp() 
		{
			
	//		ofstream o("___test___.c");
	//		//o << code.code() << endl;
	//		o << "int main(int argc, char* argv[]) {" << endl;
	//		o << "\tchar* buffer = NULL;" << endl;
	//		o << "\tFILE *f;" << endl;
	//		o << "\tif (argc < 2) return -2;" << endl;
	//		o << "\tf = fopen(argv[1],\"r\");" << endl;
	//		o << "\tif (f != NULL && fseek(f, 0L, SEEK_END) == 0) {" << endl;
	//		o << "\t\tlong bufsize = ftell(f);" << endl;
	//		o << "\t\tif (bufsize == -1) return -2;" << endl;
	//		o << "\t\tbuffer = (char*)malloc(bufsize+1);" << endl;
	//		o << "\t\tif (buffer == NULL) return -2;" << endl;
	//		o << "\t\tif (fseek(f, 0L, SEEK_SET) != 0) return -2;" << endl;
	//		o << "\t\tif (fread(buffer, 1, bufsize, f) == 0) return -2;" << endl;
	//		o << "\t\tfclose(f);" << endl;
	//		o << "\t\treturn " << regex.funcname() << "(buffer);" << endl;
	//		o << "\t}" << endl;
	//		o << "\treturn -2;" << endl;
	//		o << "}" << endl;
	//		int res = system("gcc -o ___test___ ___test___.c") >> 8;	// compile using gcc
	//		CPPUNIT_ASSERT( res == 0 ); 								// assert a successfull compilation
			//system("rm -f ___test___.c"); 								// remove source
		}

		/**
		 * Post-test procedures.
		 */
		void tearDown() 
		{
		}

		void testCompile() 
		{		
			Language language;
			language.load("test.yaml");
			//ICompilableTest::clearFiles();
		}

		void testName() 
		{
			//CPPUNIT_ICOMPILABLE_ASSERTNAME(alt, "al_sq_ch_114_ch_101_sq_ch_100_ch_105");
		}
		
	}; /* class LanguageTest */
} /* namespace pgen */

#endif /* LANGUAGE_TEST_H_ */
