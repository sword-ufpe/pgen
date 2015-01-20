/*
 * Util.cpp
 *
 *  Created on: Jun 8, 2014
 *      Author: Dimas Melo Filho
 */

#include "Util.h"
#include <string>
#include <sstream>
#include "InvalidCharacter.h"
using namespace std;

namespace pagen {

unsigned int Util::strlen_utf8(const string & text) {
	int sz = 0;
	while (Util::next_utf8(text, sz) != 0) ;
	sz--;
	return sz >= 0 ? sz : 0;
}

unsigned int Util::next_utf8(const string & text, int & pos) {
	register unsigned int c, d;			// character buffer
	c = text[pos++];					// read the leading element (first byte)
	if ((c & 0x80) == 0) return c;		// 1-byte character.
	if ((c & 0xE0) == 0xC0) {			// 2-bytes character.
		c &= 0x3F;
		c <<= 6;
		d = text[pos++];				// 2nd byte
		if ((d & 0x80) == 0)
			throw new InvalidCharacter();
		c |= (d & 0x3F);				// add the bits of the 2nd byte
		return c;
	}
	if ((c & 0xF0) == 0xE0) {			// 3-bytes character.
		c &= 0x1F;
		c <<= 6;
		d = text[pos++];				// 2nd byte
		if ((d & 0x80) == 0)
			new InvalidCharacter();
		c |= (d & 0x3F);				// add the bits of the 2nd byte
		c <<= 6;
		d = text[pos++];				// 3rd byte
		if ((d & 0x80) == 0)
			new InvalidCharacter();
		c |= (d & 0x3F);				// add the bits of the 3rd byte
		return c;
	}
	if ((c & 0xF8) == 0xF0) {			// 4-bytes character.
		c &= 0x0F;
		c <<= 6;
		d = text[pos++];				// 2nd byte
		if ((d & 0x80) == 0)
			new InvalidCharacter();
		c |= (d & 0x3F);				// add the bits of the 2nd byte
		c <<= 6;
		d = text[pos++];				// 3rd byte
		if ((d & 0x80) == 0)
			new InvalidCharacter();
		c |= (d & 0x3F);				// add the bits of the 3rd byte
		c <<= 6;
		d = text[pos++];				// 4th byte
		if ((d & 0x80) == 0)
			new InvalidCharacter();
		c |= (d & 0x3F);				// add the bits of the 4th byte
		return c;
	}
	throw new InvalidCharacter();
}

unsigned int Util::hex2int(unsigned int c) {
	if (c >= 0x30 && c <= 0x39) return c - 0x30;
	if (c >= 'A' && c <= 'F') return c - ('A'-10);
	if (c >= 'a' && c <= 'f') return c - ('a'-10);
	return 0;
}

unsigned int Util::dec2int(unsigned int c) {
	if (c >= 0x30 && c <= 0x39) return c - 0x30;
	return 0;
}

unsigned int Util::oct2int(unsigned int c) {
	if (c >= 0x30 && c <= 0x37) return c - 0x30;
	return 0;
}

string Util::int2cstr(unsigned int i) {
	stringstream s;
	if (i == 0x07) {
		s << "\\a";
	} else if (i == 0x08) {
		s << "\\b";
	} else if (i == 0x09) {
		s << "\\t";
	} else if (i == 0x0A) {
		s << "\\n";
	} else if (i == 0x0B) {
		s << "\\v";
	} else if (i == 0x0C) {
		s << "\\f";
	} else if (i == 0x0D) {
		s << "\\r";
	} else if (i == 0x22) {
		s << "\\\"";
	} else if (i == 0x27) {
		s << "\\'";
	} else if (i == 0x5C) {
		s << "\\\\";
	} else if (i >= 0x20 && i <= 0x7E) {
		s << (char)(i);
	} else {
		while (i > 0) {
			unsigned char d1 = i & 0xF; i >>= 4;
			unsigned char d2 = i & 0xF; i >>= 4;
			s << "\\x";
			s << (char)((d2 < 0xA ? '0' : 'A') + d2);
			s << (char)((d1 < 0xA ? '0' : 'A') + d1);
		}
	}
	return s.str();

}

string Util::cstr(string str) {
	stringstream s;
	for (char c: str) {
		s << Util::int2cstr((unsigned int) c);
	}
	return s.str();
}

} /* namespace pagen */
