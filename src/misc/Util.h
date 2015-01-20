/*
 * Util.h
 *
 *  Created on: Jun 8, 2014
 *      Author: Dimas Melo Filho
 */

#ifndef UTIL_H_
#define UTIL_H_

#include <string>
using namespace std;

namespace pagen {

class Util {
public:
	/**
	 * Count the number of utf8 chars in a given string.
	 * \author Dimas Melo Filho
	 * \param text The string to count the chars of.
	 * \return The number of utf8 characters on the string.
	 * \remark The number of characters can be different of the number of bytes if
	 * the text contains multi-byte characters.
	 * \throws std::exception when an invalid character is encountered or when reading an invalid pointer.
	 */
	static unsigned int strlen_utf8(const string & text);
	/**
	 * Returns the next utf-8 character in the text and increments the position (which is
	 * passed as parameter) by the amount of consumed characters. This amount varies
	 * according to the utf-8 character.
	 * This method was developed according to the Unicode 6.3.0 standard.
	 * \author Dimas Melo Filho
	 * \param text A text to read the next character from
	 * \param pos The current position in text, which is counted in 8-bit byte octets.
	 * \return the unsigned int code of the next character.
	 * \remark The pos parameter tells the method what is the current position that should be read. However its value is changed within the method.
	 * \throws std::exception when an invalid character is encountered or when reading an invalid pointer (i.e. not at the first byte of a character).
	 */
	static unsigned int next_utf8(const string & text, int & pos);

	/**
	 * Converts an utf-8 character of the class "[0-9A-Fa-f]" to its integer (base 16) value
	 * considering that it represents a base 16 digit.
	 * \param c base 16 utf-8 hexadecimal digit (character).
	 * \return integer (base 10) value of the character
	 * \remark For instance, if the value of the character 'B' (0x42) is passed to the method, it returns the integer 11, which corresponds to the value of B on base 16.
	 */
	static unsigned int hex2int(unsigned int c);

	/**
	 * Converts an utf-8 character of the class "[0-9]" to its integer (base 10) value
	 * considering that it represents a base 10 digit.
	 * \param c base 10 utf-8 decimal digit (character).
	 * \return integer (base 10) value of the character.
	 * \remark For instance, if the value of the character '9' (0x39) is passed to the method, it returns the integer 9.
	 */
	static unsigned int dec2int(unsigned int c);

	/**
	 * Converts an utf-8 character of the class "[0-7]" to its integer (base 8) value
	 * considering that it represents a base 8 digit.
	 * \param c base 8 utf-8 hexadecimal digit (character).
	 * \return integer (base 10) value of the character
	 */
	static unsigned int oct2int(unsigned int c);

	/**
	 * Converts an integer to a valid C string character. If the integer represents one of
	 * [0-9A-Za-z_] it returns the character itself. If it represents other characters, it returns
	 * hex representations "\x##" of the characters.
	 * \param i the integer to convert to a C string.
	 * \return an STL string containing the corresponding characters.
	 */
	static string int2cstr(unsigned int i);

	/**
	 * Returns a valid C representation of the string to be used between double quotes.
	 * Like "myString\x08\n".
	 * \param str the string to be converted.
	 * \return a string containing a valid C representation of the string.
	 */
	static string cstr(string str);

};

} /* namespace pagen */

#endif /* UTIL_H_ */
