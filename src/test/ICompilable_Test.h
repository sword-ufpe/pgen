/*
 * ICompilable_Test.h
 *
 *  Created on: Jun 24, 2014
 *      Author: Dimas Melo Filho
 */

#ifndef ICOMPILABLE_TEST_H_
#define ICOMPILABLE_TEST_H_

// CppUnit
#include <cppunit/TestAssert.h>
// STL
#include <fstream>
#include <cstdio>
// pgen
#include "../expr/ICompilable.h"
#include "../expr/Code.h"

// DEFINES
#define CPPUNIT_ICOMPILABLE_ASSERT(test, result) (pgen::ICompilableTest::assert(test, result, __FILE__, __LINE__))
#define CPPUNIT_ICOMPILABLE_ASSERTNAME(comp, name) (pgen::ICompilableTest::assertName(comp, name, __FILE__, __LINE__))

using namespace std;
namespace pgen
{
	class ICompilableTest 
	{
	private:
		/**
		 * Create a valid C99 source code with code that reads a file and validates its content
		 * according to the rules of some ICompilable object. This is used to test if the
		 * resulting compiled code is capable of parsing text correctly.
		 * \param fileName name of file to save the source code
		 * \param header the code of the header file
		 * \param comp ICompilable object to use on source code.
		 * \param extra extra code to compile (helper functions, etc).
		 * \remark this method creates the source and saves it on the file specified by fileName.
		 */
		static void createSource(string fileName, string header, pgen::ICompilable & comp, string extra) 
		{
			ofstream o(fileName);
			o << header << endl << pgen::Code::getHelper() << extra;
			o << comp.compile();
			o << "int main(int argc, char* argv[]) {" << endl;
			o << "\tchar* buffer = NULL;" << endl;
			o << "\tFILE *f;" << endl;
			o << "\tif (argc < 2) return -2;" << endl;
			o << "\tf = fopen(argv[1],\"r\");" << endl;
			o << "\tif (f != NULL && fseek(f, 0L, SEEK_END) == 0) {" << endl;
			o << "\t\tlong bufsize = ftell(f);" << endl;
			o << "\t\tif (bufsize == -1) return -2;" << endl;
			o << "\t\tbuffer = (char*)malloc(bufsize+1);" << endl;
			o << "\t\tif (buffer == NULL) return -2;" << endl;
			o << "\t\tif (fseek(f, 0L, SEEK_SET) != 0) return -2;" << endl;
			o << "\t\tif (fread(buffer, 1, bufsize, f) == 0) return -2;" << endl;
			o << "\t\tfclose(f);" << endl;
			o << "\t\treturn " << comp.funcname() << "(buffer);" << endl;
			o << "\t}" << endl;
			o << "\treturn -2;" << endl;
			o << "}" << endl;
		}

		/**
		 * Create a valid C99 source code with code that reads a file and validates its content
		 * according to the rules of some ICompilable object. This is used to test if the
		 * resulting compiled code is capable of parsing text correctly.
		 * This overloaded method uses the CHAINED version of the method.
		 * \param fileName name of file to save the source code
		 * \param header the code of the header file.
		 * \param comp ICompilable object to use on source code.
		 * \param extra extra code to compile (helper functions, etc).
		 * \param chain the names of the chained functions, separated by comma. eg. "func1, func2, func3"
		 * \remark this method creates the source and saves it on the file specified by fileName.
		 */
		static void ccreateSource(string fileName, string header, pgen::ICompilable & comp, string extra, string chain) 
		{
			ofstream o(fileName);
			o << header << endl << pgen::Code::getHelper() << extra;
			o << comp.ccompile();
			o << "chainptr testChain[] = {" << chain << ", NULL};" << endl << endl;
			o << "int main(int argc, char* argv[]) {" << endl;
			o << "\tchar* buffer = NULL;" << endl;
			o << "\tFILE *f;" << endl;
			o << "\tif (argc < 2) return -2;" << endl;
			o << "\tf = fopen(argv[1],\"r\");" << endl;
			o << "\tif (f != NULL && fseek(f, 0L, SEEK_END) == 0) {" << endl;
			o << "\t\tlong bufsize = ftell(f);" << endl;
			o << "\t\tif (bufsize == -1) return -2;" << endl;
			o << "\t\tbuffer = malloc(bufsize+1);" << endl;
			o << "\t\tif (buffer == NULL) return -2;" << endl;
			o << "\t\tif (fseek(f, 0L, SEEK_SET) != 0) return -2;" << endl;
			o << "\t\tif (fread(buffer, 1, bufsize, f) == 0) return -2;" << endl;
			o << "\t\tfclose(f);" << endl;
			o << "\t\treturn " << comp.cfuncname() << "(buffer, testChain);" << endl;
			o << "\t}" << endl;
			o << "\treturn -2;" << endl;
			o << "}" << endl;
		}

	public:
		/**
		 * Create an executable file that uses the logic of the ICompilable object to parse
		 * data from a file. The executable program can be used to test if the code generated by
		 * the ICompilable object is capable of parsing input data correctly.
		 * This overloaded version of the method uses the CHAINED version of the method.
		 * \param comp ICompilable object to be used as base to create the executable program.
		 * \param header the code of the header file.
		 * \param extra Extra C99 code to add to the final program (i.e. helper functions, etc).
		 * \param chain the names of the chained functions, separated by comma. eg. "func1, func2, func3"
		 * \remark this program uses the private method ICompilableTest::createSource to create
		 * the source code.
		 * \remark the name of the executable created is currently ___test___ with three
		 * underlines before and three after the name "test".
		 * \remark the compiled file is saved to the current operating directory.
		 * \remark the source code created in the process is removed after the compilation is
		 * complete.
		 */
		static void ccompileSource(pgen::ICompilable & comp, string header, string extra, string chain) 
		{
			ccreateSource("___test___.c", header, comp, extra, chain);	// create C source code
			int res = system("gcc -o ___test___ ___test___.c") >> 8;	// compile using gcc
			CPPUNIT_ASSERT( res == 0 ); 								// assert a successfull compilation
			system("rm -f ___test___.c"); 								// remove source
		}

		/**
		 * Create an executable file that uses the logic of the ICompilable object to parse
		 * data from a file. The executable program can be used to test if the code generated by
		 * the ICompilable object is capable of parsing input data correctly.
		 * \param comp ICompilable object to be used as base to create the executable program.
		 * \param header the code of the header file.
		 * \param extra Extra C99 code to add to the final program (i.e. helper functions, etc).
		 * \remark this program uses the private method ICompilableTest::createSource to create
		 * the source code.
		 * \remark the name of the executable created is currently ___test___ with three
		 * underlines before and three after the name "test".
		 * \remark the compiled file is saved to the current operating directory.
		 * \remark the source code created in the process is removed after the compilation is
		 * complete.
		 */
		static void compileSource(pgen::ICompilable & comp, string header, string extra) 
		{
			createSource("___test___.c", header, comp, extra);			// create C source code
			int res = system("gcc -o ___test___ ___test___.c") >> 8;	// compile using gcc
			CPPUNIT_ASSERT( res == 0 ); 								// assert a successfull compilation
			//system("rm -f ___test___.c"); 								// remove source
		}

		/**
		 * Create an executable file that uses the logic of the ICompilable object to parse
		 * data from a file. The executable program can be used to test if the code generated by
		 * the ICompilable object is capable of parsing input data correctly.
		 * \param comp ICompilable object to be used as base to create the executable program.
		 * \param header the code of the header file.
		 * \remark this program uses the private method ICompilableTest::createSource to create
		 * the source code.
		 * \remark the name of the executable created is currently ___test___ with three
		 * underlines before and three after the name "test".
		 * \remark the compiled file is saved to the current operating directory.
		 * \remark the source code created in the process is removed after the compilation is
		 * complete.
		 */
		static void compileSource(pgen::ICompilable & comp, string header) 
		{
			compileSource(comp, header, "");
		}

		/**
		 * Remove files that could possibly be on the user's system after using some methods of
		 * this class.
		 */
		static void clearFiles() 
		{
			system("rm -f ___test___");
			system("rm -f .testfile");
		}

		/**
		 * Pass test text to the compiled program and assert that the result of the parsing is
		 * equal to a value provided by the user.
		 * \param test test text to pass to the compiled program
		 * \param result value to assert that the compiled program returns
		 * \remark the method uses the CPPUNIT_ASSERT to do the assertion. It should fail if the
		 * result of the executable program is different of the value provided by the user.
		 */
		static void assert(string test, int result, string file, int line) 
		{
			system("rm -f .testfile");
			ofstream o(".testfile");
			o << test;
			o.close();
			int res = system("./___test___ .testfile") >> 8;
			stringstream msg;
			msg << "Error on the compiled version of the code. From file: " << file << ":" << line << endl;
			msg << "Expected result: " << result << ", but found " << res << " instead." << endl;
			CPPUNIT_ASSERT_MESSAGE(msg.str(), res == result);
			system("rm -f .testfile");
		}

		/**
		 * Asserts the name of the object.
		 */
		static void assertName(pgen::ICompilable &comp, string name, string file, int line) 
		{
			stringstream msg;
			msg << "Error asserting the name of an ICompilable object." << endl <<
					" Expected '" << name << "' but found '" << comp.name() << "'. On " << file << ":" << line << endl;
			CPPUNIT_ASSERT_MESSAGE(msg.str(), name == comp.name());
		}
		
	}; /* class ICompilableTest */
} /* namespace pgen */

#endif /* ICOMPILABLE_TEST_H_ */
