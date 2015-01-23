/*
 * Util_Test.h
 *
 *  Created on: Jun 13, 2014
 *      Author: Dimas Melo Filho
 */

#ifndef UTIL_TEST_H_
#define UTIL_TEST_H_

#include "../misc/Util.h"
#include <string>
#include <cppunit/TestFixture.h>
#include <cppunit/TestAssert.h>
#include <cppunit/TestSuite.h>

/**
 * Unit test fixture for the "Util" class.
 */
class UtilTest: public CppUnit::TestFixture {
public:
	static CppUnit::Test * suite() {
		CppUnit::TestSuite * s = new CppUnit::TestSuite("IntervalTest");
		s->addTest(new CppUnit::TestCaller<UtilTest>("test Util::hex2int", &UtilTest::test_hex2int));
		s->addTest(new CppUnit::TestCaller<UtilTest>("test Util::next_utf8", &UtilTest::test_nextutf8));
		return s;
	}
	void setUp() {

	}

	void tearDown() {

	}

	void test_hex2int() {
		CPPUNIT_ASSERT( pgen::Util::hex2int(0x10) == 0 );
		CPPUNIT_ASSERT( pgen::Util::hex2int(0x29) == 0 );
		CPPUNIT_ASSERT( pgen::Util::hex2int(0x30) == 0 );
		CPPUNIT_ASSERT( pgen::Util::hex2int(0x31) == 1 );
		CPPUNIT_ASSERT( pgen::Util::hex2int(0x32) == 2 );
		CPPUNIT_ASSERT( pgen::Util::hex2int(0x33) == 3 );
		CPPUNIT_ASSERT( pgen::Util::hex2int(0x34) == 4 );
		CPPUNIT_ASSERT( pgen::Util::hex2int(0x35) == 5 );
		CPPUNIT_ASSERT( pgen::Util::hex2int(0x36) == 6 );
		CPPUNIT_ASSERT( pgen::Util::hex2int(0x37) == 7 );
		CPPUNIT_ASSERT( pgen::Util::hex2int(0x38) == 8 );
		CPPUNIT_ASSERT( pgen::Util::hex2int(0x39) == 9 );
		CPPUNIT_ASSERT( pgen::Util::hex2int(0x3A) == 0 );
		CPPUNIT_ASSERT( pgen::Util::hex2int(0x3B) == 0 );
		CPPUNIT_ASSERT( pgen::Util::hex2int('A') == 10);
		CPPUNIT_ASSERT( pgen::Util::hex2int('B') == 11);
		CPPUNIT_ASSERT( pgen::Util::hex2int('C') == 12);
		CPPUNIT_ASSERT( pgen::Util::hex2int('D') == 13);
		CPPUNIT_ASSERT( pgen::Util::hex2int('E') == 14);
		CPPUNIT_ASSERT( pgen::Util::hex2int('F') == 15);
		CPPUNIT_ASSERT( pgen::Util::hex2int('G') == 0);
		CPPUNIT_ASSERT( pgen::Util::hex2int(' ') == 0);
		CPPUNIT_ASSERT( pgen::Util::hex2int('!') == 0);
		CPPUNIT_ASSERT( pgen::Util::hex2int('@') == 0);
		CPPUNIT_ASSERT( pgen::Util::hex2int('a') == 10);
		CPPUNIT_ASSERT( pgen::Util::hex2int('b') == 11);
		CPPUNIT_ASSERT( pgen::Util::hex2int('c') == 12);
		CPPUNIT_ASSERT( pgen::Util::hex2int('d') == 13);
		CPPUNIT_ASSERT( pgen::Util::hex2int('e') == 14);
		CPPUNIT_ASSERT( pgen::Util::hex2int('f') == 15);
		CPPUNIT_ASSERT( pgen::Util::hex2int('g') == 0);
	}

	void test_nextutf8() {
		char cstr[5] = {0x00, 0x00, 0x00, 0x00, 0x00};
		// Test range u+0000 ~ u+007F
		for (unsigned char i = 0; i <= 0x7F; i++) {
			cstr[0] = i;
			std::string s(cstr);
			int pos = 0;
			CPPUNIT_ASSERT( pgen::Util::next_utf8(s, pos) == i );
			CPPUNIT_ASSERT( pos == 1 );
		}
		// Test range u+0080 ~ u+07FF
		unsigned int c = 0x80;
		for (unsigned char i1 = 0xC2; i1 <= 0xDF; i1++) {
			cstr[0] = i1;
			for (unsigned char i2 = 0x80; i2 <= 0xBF; i2++) {
				cstr[1] = i2;
				int pos = 0;
				std::string s(cstr);
				CPPUNIT_ASSERT( pgen::Util::next_utf8(s, pos) == c );
				CPPUNIT_ASSERT( pos == 2 );
				c++;
			}
		}
		// Test range u+0800 ~ u+0FFF
		c = 0x800;
		cstr[0] = 0xE0;
		for (unsigned char i1 = 0xA0; i1 <= 0xBF; i1++) {
			cstr[1] = i1;
			for (unsigned char i2 = 0x80; i2 <= 0xBF; i2++) {
				cstr[2] = i2;
				int pos = 0;
				std::string s(cstr);
				CPPUNIT_ASSERT( pgen::Util::next_utf8(s, pos) == c );
				CPPUNIT_ASSERT( pos == 3 );
				c++;
			}
		}
		// Test range u+1000 ~ u+CFFF
		c = 0x1000;
		for (unsigned char i1 = 0xE1; i1 <= 0xEC; i1++) {
			cstr[0] = i1;
			for (unsigned char i2 = 0x80; i2 <= 0xBF; i2++) {
				cstr[1] = i2;
				for (unsigned char i3 = 0x80; i3 <= 0xBF; i3++) {
					cstr[2] = i3;
					int pos = 0;
					std::string s(cstr);
					CPPUNIT_ASSERT( pgen::Util::next_utf8(s, pos) == c );
					CPPUNIT_ASSERT( pos == 3 );
					c++;
				}
			}
		}
		// Test range u+D000 ~ u+D7FF
		c = 0xD000;
		cstr[0] = 0xED;
		for (unsigned char i1 = 0x80; i1 <= 0x9F; i1++) {
			cstr[1] = i1;
			for (unsigned char i2 = 0x80; i2 <= 0xBF; i2++) {
				cstr[2] = i2;
				int pos = 0;
				std::string s(cstr);
				CPPUNIT_ASSERT( pgen::Util::next_utf8(s, pos) == c );
				CPPUNIT_ASSERT( pos == 3 );
				c++;
			}
		}
		// According to the UNICODE 6.2 documentation, there are no characters at the
		// range u+D800 ~ u+DFFF
		// TODO zzz test if range u+D800 ~ u+DFFF returns an invalid character error.
		// Test range u+E000 ~  u+FFFF
		c = 0xE000;
		for (unsigned char i1 = 0xEE; i1 <= 0xEF; i1++) {
			cstr[0] = i1;
			for (unsigned char i2 = 0x80; i2 <= 0xBF; i2++) {
				cstr[1] = i2;
				for (unsigned char i3 = 0x80; i3 <= 0xBF; i3++) {
					cstr[2] = i3;
					int pos = 0;
					std::string s(cstr);
					CPPUNIT_ASSERT( pgen::Util::next_utf8(s, pos) == c );
					CPPUNIT_ASSERT( pos == 3 );
					c++;
				}
			}
		}
		// Test range u+10000 ~ u+3FFFF
		c = 0x10000;
		cstr[0] = 0xF0;
		for (unsigned char i1 = 0x90; i1 <= 0xBF; i1++) {
			cstr[1] = i1;
			for (unsigned char i2 = 0x80; i2 <= 0xBF; i2++) {
				cstr[2] = i2;
				for (unsigned char i3 = 0x80; i3 <= 0xBF; i3++) {
					cstr[3] = i3;
					int pos = 0;
					std::string s(cstr);
					CPPUNIT_ASSERT( pgen::Util::next_utf8(s, pos) == c );
					CPPUNIT_ASSERT( pos == 4 );
					c++;
				}
			}
		}
		// Test range u+40000 ~ u+FFFFF
		c = 0x40000;
		for (unsigned char i1 = 0xF1; i1 <= 0xF3; i1++) {
			cstr[0] = i1;
			for (unsigned char i2 = 0x80; i2 <= 0xBF; i2++) {
				cstr[1] = i2;
				for (unsigned char i3 = 0x80; i3 <= 0xBF; i3++) {
					cstr[2] = i3;
					for (unsigned char i4 = 0x80; i4 <= 0xBF; i4++) {
						cstr[3] = i4;
						int pos = 0;
						std::string s(cstr);
						CPPUNIT_ASSERT( pgen::Util::next_utf8(s, pos) == c );
						CPPUNIT_ASSERT( pos == 4 );
						c++;
					}
				}
			}
		}
		// Test range u+100000 ~ u+10FFFF
		c = 0x100000;
		cstr[0] = 0xF4;
		for (unsigned char i1 = 0x80; i1 <= 0x8F; i1++) {
			cstr[1] = i1;
			for (unsigned char i2 = 0x80; i2 <= 0xBF; i2++) {
				cstr[2] = i2;
				for (unsigned char i3 = 0x80; i3 <= 0xBF; i3++) {
					cstr[3] = i3;
					int pos = 0;
					std::string s(cstr);
					CPPUNIT_ASSERT( pgen::Util::next_utf8(s, pos) == c );
					CPPUNIT_ASSERT( pos == 4 );
					c++;
				}
			}
		}
		// TODO zzz write tests to check invalid characters.
	}
};

#endif /* UTIL_TEST_H_ */
