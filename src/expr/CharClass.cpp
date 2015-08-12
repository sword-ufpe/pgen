/**
 * pgen, Parser Generator.
 * Copyright (C) 2015 Dimas Melo Filho
 * 
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA. 
 * 
 * The author can be reached by e-mail: dldmf@cin.ufpe.br.
 * 
 * \author Dimas Melo Filho <dldmf@cin.ufpe.br>
 * \date 2014-07-09
 * \file
 * This file implements an ICompilable that handles all the logic behind a Regular Expression Class of Characters.
 * It provides methods both for parsing regular expressions and for generating C code.
 * The CharClass represents a class of characters that can be matched on a position of the Regular Expression. For 
 * instance, if someone uses the following regular expression '[0-9A-F]', it will be encoded using this class. The 
 * CharClass is also able to parse classes that contain other named classes such as '[:alpha:]'. If the named class is 
 * not a standard POSIX class, it must be declared on the YAML file first.
 */

// STL
#include <climits>
#include <string>
#include <sstream>
#include <iostream>
// Other
#include "Character.h"
#include "CharClass.h"
#include "RegexState.h"
#include "RegexEscape.h"
#include "../parser/NamedClassManager.h"
#include "../misc/Util.h"
#include "../misc/RegexException.h"

using namespace std;
namespace pgen 
{
	
	CharClass* CharClass::dot()
	{
		CharClass* c = new CharClass();
		c->add(0,UINT_MAX);
		return c;
	}
	
	string CharClass::parseClassName(string &expression, unsigned int& pos)
	{
		unsigned int c = Util::next_utf8(expression, pos);;
		stringstream s;
		while (c != ':' && pos < expression.length())
		{
			switch (c)
			{
				case 0x00: case 0x03: case 0x04:
					throw RegexException(expression, pos, "Unexpected end of expression inside a Character Class Name.");
				case ']':
					throw RegexException(expression, pos, "Expected: end of named class ':', but found end of class definition ']'");
			}
			s << Util::int2cstr(c);
			c = Util::next_utf8(expression, pos);
		}
		if (pos >= expression.length())
		{
			throw RegexException(expression, pos, "Expected end of named class.");
		}
		return s.str();
	}

	CharClass* CharClass::parse(string& expression, unsigned int& pos, NamedClassManager& ncm)
	{
		stringstream namedClass;
		unsigned int c = Util::next_utf8(expression, pos);
		unsigned int lastChar = UINT_MAX;
		ICompilable *escape = nullptr;
		CharClass* cClass = new CharClass();
		switch (c)
		{
			// If it's an escape character, process it.
			case '\\':
				escape = RegexEscape::parse(expression, pos);
				switch (escape->type())
				{
					case CharClass::TYPE:
						cClass->add((CharClass*)escape);
						delete escape;
						break;
					case Character::TYPE:
						lastChar = ((Character*)escape)->getChar();
						delete escape;
						break;
					default:
						delete escape;
						delete cClass;
						throw RegexException(expression, pos, "Unknown escape type, while trying to parse a character class.");
				}
				break;
			// If it is an negation character, set a flag to indicate the class is negated.
			case '^':
				cClass->negated = true;
				break;
			// If it is the start of a named class's name
			case ':':
				cClass->add(&(ncm.get(CharClass::parseClassName(expression, pos))));
				break;
			// If it is the end of the stream, throw an error.
			case 0x00: case 0x03: case 0x04:
				delete cClass;
				throw RegexException(expression, pos, "Unexpected end of expression inside a Character Class.");
			// If any other character, just set it as the lastChar.
			default:
				lastChar = c;
				break;				
		}
		
		do
		{
			c = Util::next_utf8(expression, pos);
			switch (c)
			{
				// When there is an escape character, parse it.
				case '\\':
					if (lastChar != UINT_MAX)
					{
						cClass->add(lastChar);
						lastChar = UINT_MAX;
					}
					escape = RegexEscape::parse(expression, pos);
					switch (escape->type())
					{
						case CharClass::TYPE:
							cClass->add((CharClass*)escape);
							delete escape;
							break;
						case Character::TYPE:
							lastChar = ((Character*)escape)->getChar();
							delete escape;
							break;
						default:
							delete cClass;
							delete escape;
							throw RegexException(expression, pos, "Unknown escape type, while trying to parse a character class.");
					}	
					break;
				// When it's the end of the class.
				case ']':
					if (lastChar != UINT_MAX)
					{
						cClass->add(lastChar);
						lastChar = UINT_MAX;
					}
					cClass->normalize();
					return cClass;
				// When it's the '-' char, it could be the beggining of a range, or just the char itself.
				case '-':
					if (pos >= expression.length())
					{
						delete cClass;
						throw RegexException(expression, pos, "Unexpected end of expression, expeting character class range.");
					}
					if (lastChar != UINT_MAX)
					{
						c = Util::next_utf8(expression, pos);
						switch (c)
						{
							// if an escape character is found, parse it
							case '\\':
								escape = RegexEscape::parse(expression, pos);
								switch (escape->type())
								{
									case Character::TYPE:
										c = ((Character*)escape)->getChar();
										delete escape;
										cClass->add(lastChar, c);
										lastChar = UINT_MAX;
										break;
									case CharClass::TYPE:
										cClass->add(lastChar);
										cClass->add('-');
										cClass->add((CharClass*)escape);
										delete escape;
										break;
									default:
										delete escape;
										delete cClass;
										throw RegexException(expression, pos, "Expected character range end or character class, but found character class instead.");						
								}
								break;
							// If it is the start of a named class's name
							case ':':
								cClass->add(lastChar);
								cClass->add('-');
								cClass->add(&ncm.get(CharClass::parseClassName(expression, pos)));
								lastChar = UINT_MAX;
								break;
							// When it's the end of the class, add the lastChar and the '-' char and return.
							case ']':
								cClass->add(lastChar);
								cClass->add('-');
								cClass->normalize();
								return cClass;
							// When it's the end of the stream, it's an error.
							case 0x00: case 0x03: case 0x04:
								delete cClass;
								throw RegexException(expression, pos, "Unexpected end of expression inside a Character Class Range.");
							// If any other character is found, consider it as the maximum value of a range of characters
							default:
								cClass->add(lastChar, c);
								lastChar = UINT_MAX;
								break;
						}
					}
					else
					{
						lastChar = '-';
					}
					break;
				// If it is the start of a named class's name
				case ':':
					if (lastChar != UINT_MAX)
					{
						cClass->add(lastChar);
						lastChar = UINT_MAX;
					}
					cClass->add(&ncm.get(CharClass::parseClassName(expression, pos)));
					break;
				// When it's the end of the stream, it's an error.
				case 0x00: case 0x03: case 0x04:
					delete cClass;
					throw RegexException(expression, pos, "Unexpected end of expression inside a Character Class.");
				// For any other character, just add it as is.
				default:
					if (lastChar != UINT_MAX) 
					{
						cClass->add(lastChar);
					}
					lastChar = c;
					break;
			}
		} while (pos < expression.length());
		throw RegexException(expression, pos, "Unexpected end of expression inside a Character Class. Missing ']'.");
	}
	
	void CharClass::add(unsigned int c) 
	{
		add(c,c);
		// TODO zzz create a more efficient way of doing this with a single char, if possible.
	}

	void CharClass::add(unsigned int cstart, unsigned int cend) 
	{
		Range * v = this->range, * t = nullptr;
		// If this is the first interval, just add it
		if (v == nullptr) 
		{
			this->range = new Range(cstart, cend);
			return;
		}
		// If this isn't the first interval, try to find an interval to merge with
		// The search should stop if one of the following conditions is satisfied:
		// 1. An insert happened (in which case the method must merge, stop and return)
		// 2. The end of the list of intervals was reached (then the interval must be added to the end)
		// 3. All the following intervals' boundareis are greater than the proposed new interval (then the interval must be added to the current position)
		while (v != nullptr && cend+1 >= v->start) 
		{
			if (v->join(cstart, cend)) 
			{ // if an insert happened
				this->merge(v); // merge the interval that changed nad return
				return;
			}
			t = v;
			v = v->next;
		}
		// if the search reached the end of the list, append a new interval to the end of the list.
		if (v == nullptr) 
		{
			v = new Range(cstart, cend);
			v->prev = t;	// if the code reached this point, "t" will not be null. it represents the previous interval.
			v->next = t->next;
			t->next = v;
		}
		// else if there was no merging and it is not the end of the list, create a new interval and insert it on the list accordingly.
		else 
		{
			t = new Range(cstart, cend);
			t->next = v;
			t->prev = v->prev;
			v->prev = t;
			if (t->prev == nullptr) this->range = t;
			else t->prev->next = t;
		}
	}
	
	void CharClass::add(const CharClass& c)
	{
		Range * r = c.range;
		while (r != nullptr) 
		{
			this->add(r->start, r->end);
			r = r->next;
		}
	}

	void CharClass::add(CharClass * c) 
	{
		this->add(const_cast<const CharClass&>(*c));
	}

	void CharClass::merge(Range * v) 
	{
		Range * t;
		// merge backwards on the linked list, removing the merged elements
		while (v->prev != nullptr && v->prev->join(v)) 
		{
			t = v;
			v = t->prev;
			v->next = t->next;
			if (v->next != nullptr) 
			{
				v->next->prev = v;
			}
			delete t;
		}
		// merge forward on the linked list, removing the merged elements
		while (v->next != nullptr && v->next->join(v)) 
		{
			t = v;
			v = t->next;
			v->prev = t->prev;
			if (v->prev != nullptr) 
			{ 
				v->prev->next = v;
			}
			delete t;
		}
		// if the head element was merged, update the head pointer.
		if (v->prev == nullptr) this->range = v;
	}

	void CharClass::normalize() 
	{
		if (negated) 
		{
			invert();
			negated = false;
		}
	}

	void CharClass::invert() 
	{
		unsigned int s = 1, e;
		Range * v = range;
		if (v == nullptr) 
		{
			range = new Range(1,UINT_MAX);
			return;
		}
		// if the first interval starts from 1, then the inverse starts after 1.
		if (v != nullptr && v->start == 1) 
		{
			e = v->end;
			s = v->end+1;
			range = v->next;
			delete v;
		}
		// change every following interval to its negation
		v = range;
		while (v != nullptr) 
		{
			e = v->end;
			v->end = v->start-1;
			v->start = s;
			s = e + 1;
			if (v->next == nullptr) break;
			v = v->next;
		}

		// if the last interval didn't end in UINT_MAX create a new interval that contains
		// all the remaining characters from the current character to UINT_MAX
		if (e < UINT_MAX) 
		{
			if (v != nullptr) 
			{
				v->next = new Range(s,UINT_MAX);
				v->next->prev = v;
			} 
			else 
			{
				range = new Range(s,UINT_MAX);
			}
		}
	}

	CharClass::CharClass()
	 : range(nullptr)
	 , negated(false) 
	{
	}
	
	CharClass::CharClass(const CharClass& cClass)
	 : range(nullptr)
	 , negated(cClass.negated)
	{
		this->add(cClass);
	}
	
	CharClass::~CharClass() 
	{
		Range * next;
		while (range != nullptr) 
		{
			next = range->next;
			delete range;
			range = next;
		}
	}

	bool CharClass::operator==(const CharClass & c) const 
	{
		if (c.negated != this->negated) return false;
		Range * vc = c.range, * vt = this->range;
		while (vc != nullptr && vt != nullptr) 
		{
			if (*vc != *vt) return false;
			vc = vc->next;
			vt = vt->next;
		}
		return (vc == nullptr && vt == nullptr);
	}

	bool CharClass::operator!=(const CharClass &c) const 
	{
		return !(*this == c);
	}

	/**
	 * compiled code:
	 * %cprototype()% {
	 *  int pos = 0, pchain = 0;
	 *  unsigned int c = next_utf8(text, pos);
	 *  %for each range r[n]:%
	 *  if (c >= %r[n].start% && c <= %r[n].end%) {
	 *   pchain = chain_next(text + pos, chain);
	 *   if (pchain >= 0) return pchain + pos;
	 *  }
	 *  %end for%
	 *  return -1;
	 */
	string CharClass::ccompile() 
	{
		stringstream s;
		s << 
			this->cprototype() << " {" 												"\n"
			" int pos = 0, pchain = 0;"												"\n"
			" unsigned int c = next_utf8(text, &pos);"								"\n";
		Range * r = this->range;
		while (r != nullptr) 
		{
			s << 
				" if (c >= " << r->start << " && " << "c <= " << r->end << ") {"	"\n"
				"  pchain = chain_next(text + pos, chain);"							"\n"
				"  if (pchain >= 0) return pchain + pos;"							"\n"
				" }"																"\n";
			r = r->next;
		}
		s << 
			" return -1;" 															"\n"
			"}"																		"\n\n";
		return s.str();
	}

	/**
	 * compiled code:
	 * %prototype()% {
	 *  int pos = 0;
	 *  unsigned int c = next_utf8(text, pos);
	 *  %for each range r[n]:%
	 *  if (c >= %r[n].start% && c <= %r[n].end%) return pos;
	 *  %end for%
	 *  return -1;
	 */
	string CharClass::compile() 
	{
		stringstream s;
		s << 
			this->prototype() << " {"														"\n"
			" int pos = 0;"																	"\n"
			" unsigned int c = next_utf8(text, &pos);"										"\n";
		Range * r = this->range;
		while (r != nullptr) 
		{
			s << " if (c >= " << r->start << " && " << "c <= " << r->end << ") return pos;"	"\n";
			r = r->next;
		}
		s << 
			" return -1;" 																	"\n"
			"}"																				"\n";
		return s.str();
	}

	string CharClass::name() 
	{
		stringstream s;
		s << "cl";
		Range * r = this->range;
		while (r != nullptr) 
		{
			s << "_" << r->start << "_" << r->end;
			r = r->next;
		}
		return s.str();
	}

	/**
	 * \returns the type identifier of this class.
	 */
	int CharClass::type() 
	{ 
		return TYPE; 
	}
	
	#ifdef _DEBUG
	void CharClass::print(int level)
	{
		int sz = 0;
		for (int i = 0; i < level; i++) 
		{
			cout << '.';
		}
		Range* r = range;
		while (r != nullptr)
		{
			++sz;
			r = r->next;
		}
		cout << "CLASS(@" << hex << this << ", SZ=" << dec << sz << ")" << endl;
		level++;
		for (r = range; r != nullptr; r = r->next)
		{
			r->print(level);
		}
	}
	#endif

} /* namespace pgen */
