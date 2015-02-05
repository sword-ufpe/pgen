/*
 * Regex.cpp
 * This is a file that implements the class Regex, a class that is an ICompilable
 * object that can parse a regular expression (text) and generate a C99 compiled
 * code to parse it.
 *
 *  Created on: Oct 4, 2014
 *      Author: Dimas Melo Filho
 */

// STL
#include <iostream>
#include <climits>
#include <memory>
// pgen
#include "Regex.h"
#include "ICompilable.h"
#include "Character.h"
#include "Quantified.h"
#include "Sequence.h"
#include "CharClass.h"
#include "../misc/Util.h"
#include "../misc/RegexException.h"

using namespace std;
namespace pgen 
{
	
	Regex::Regex(string expression) 
	{
		this->parse(expression);
	}

	Regex::~Regex() 
	{
	}

	ICompilable* Regex::parse(string expression) 
	{
		unsigned int pos = 0;
		this->group = 0;
		return this->parse(expression,pos,this);
	}

	ICompilable* Regex::parse(string expression, unsigned int &pos, Alternative* alt) 
	{
	#define STATE_CALL(st, ret) stateStack.push_back(ret); state = st
	#define STATE_RETURN state = stateStack.back(); stateStack.pop_back() 
		RegexState state = RegexState::MakeAlternative;
		deque<ICompilable*> seqStack;	//< Stack of Sequences
		deque<RegexState> stateStack;	//< Stack of states for parsing the expressions
		Sequence* seq = nullptr;
		ICompilable* top = nullptr;
		CharClass* charClass = nullptr;
		string error;
		bool invertClass;
		unsigned int codeChar, codeCount, min, lastChar;
		while (pos < expression.length()) 
		{
			unsigned int c = Util::next_utf8(expression, (int&)pos);
			switch (state) 
			{
			// 1. When on Alternative state
			case RegexState::MakeAlternative:
				switch (c) 
				{
				// 1.1. Escape start on alternative state
				case '\\':
					STATE_CALL(RegexState::EscapeSequence, state);
					break;
				// 1.2. 0-N Quantifier on alternative state
				case '*':
					if (seqStack.empty()) 
					{
						error = "Expected expression before quantifier '*'";
						goto parse_throw_exception;
					}
					top = seqStack.back();
					seqStack.pop_back();
					seqStack.push_back(new Quantified(top, 0, Quantified::INF));
					top = nullptr;
					break;
				// 1.3. 1-N Quantifier on alternative state
				case '+':
					if (seqStack.empty()) 
					{
						error = "Expected expression before quantifier '+'";
						goto parse_throw_exception;
					}					
					top = seqStack.back();
					seqStack.pop_back();
					seqStack.push_back(new Quantified(top, 1, Quantified::INF));
					top = nullptr;
					break;
				// 1.4. 0-1 Quantifier on alternative state
				case '?':
					if (seqStack.empty()) 
					{
						error = "Expected expression before quantifier '?'";
						goto parse_throw_exception;
					}
					top = seqStack.back();
					if (top->type() == Quantified::TYPE) 
					{
						reinterpret_cast<Quantified*>(top)->greedy = false;
					} 
					else 
					{
						seqStack.pop_back();
						seqStack.push_back(new Quantified(top, 0, 1));
					}
					top = nullptr;
					break;
				// 1.5. Explicit M-N Quantifier on Alternative State
				case '{':
					if (seqStack.empty()) 
					{
						error = "Expected expression before quantifier '{x,y}'";
						goto parse_throw_exception;
					}
					codeChar = 0; codeCount = 0;
					STATE_CALL(RegexState::ExplicitQuantifier1, state);
					break;
				// 1.6. Class start on Alternative State
				case '[':
					charClass = new CharClass();
					lastChar = UINT_MAX;
					invertClass = false;
					STATE_CALL(RegexState::ClassFirst, state);
					break;
				// 1.7. New alternative on Alternative State
				case '|':
					seq = new Sequence();
					while (!seqStack.empty()) 
					{
						seq->add(seqStack.front());
						seqStack.pop_front();
					}
					alt->add(seq);
					seq = nullptr;
					break;
				// 1.8. New Group (subset of alternatives) on Alternative State
				case '(':
					this->group++;
					seqStack.push_back(this->parse(expression, pos, new Alternative()));
					break;
				// 1.9. End of the current alternative group, the method must return
				case ')':
					if (this->group-- <= 0) 
					{
						error = "Unexpected group closure ')'. There are no open groups left to close.";
						goto parse_throw_exception;
					}
					goto parse_exit;
				// 1.10. End of Stream, the method must return the alternative set
				case 0x00:
				case 0x03:
				case 0x04:
					goto parse_exit;
				// 1.11. The dot (the class of all characters)
				case '.':
					charClass = new CharClass();
					charClass->add(0,UINT_MAX);
					seqStack.push_back(charClass);
					charClass = nullptr;
					break;
				// 1.12. Any other character is pushed as a character to be matched.
				default:
					seqStack.push_back(new Character(c));
					break;
				}
				break;
			// 2. When inside an escape sequence (EscapeSequence State)
			case RegexState::EscapeSequence:
				switch (c) 
				{
				// 2.1. \a = alert/bell (0x07)
				case 'a':
					seqStack.push_back(new Character(0x07));
					STATE_RETURN;
					break;
				// 2.2. \b = backspace (0x08)
				case 'b':
					seqStack.push_back(new Character(0x08));
					STATE_RETURN;
					break;
				// 2.3. \f = form feed (0x0C)
				case 'f':
					seqStack.push_back(new Character(0x0C));
					STATE_RETURN;
					break;
				// 2.4. \n = new line (0x0A)
				case 'n':
					seqStack.push_back(new Character(0x0A));
					STATE_RETURN;
					break;
				// 2.5. \r = carriage return (0x0D)
				case 'r':
					seqStack.push_back(new Character(0x0D));
					STATE_RETURN;
					break;
				// 2.6. \t = tab (0x09)
				case 't':
					seqStack.push_back(new Character(0x09));
					STATE_RETURN;
					break;
				// 2.7. \u = the beggining of an unicode escape sequence (\uXXXX)
				case 'u':
					codeChar = 0; codeCount = 0;
					state = RegexState::EscapeUnicode;
					break;
				// 2.8. \v = vertical tab (0x0B)
				case 'v':
					seqStack.push_back(new Character(0x0B));
					STATE_RETURN;
					break;
				// 2.9. \x = the beggining of an hexadecimal escape sequence (\xXX)
				case 'x':
					codeChar = 0; codeCount = 0;
					state = RegexState::EscapeHex;
					break;
				// 2.10. \[0-7] = the beggining of an octal escape sequence (\ooo)
				case '0': case '1':	case '2': case '3': case '4': case '5': case '6': case '7':
					codeChar = 0; codeCount = 0;
					state = RegexState::EscapeOctal;
					break;
				// 2.11. \d = the class of all digits. Equivalent to [0-9]
				case 'd':
					{
						CharClass* tempClass = new CharClass();
						tempClass->add('0','9');
						seqStack.push_back(tempClass);
						STATE_RETURN;
						break;
					}
				// 2.12. If the end of stream was reached, it is an error.
				case 0x00: case 0x03: case 0x04:
					throw RegexException(expression, pos, "Unexpected end of expression.");
				// 2.13. For any other character, just use it (despite its original meaning).
				default:
					seqStack.push_back(new Character(c));
					STATE_RETURN;
					break;
				}
				break;
			// 3. When inside an unicode escape sequence (EscapeUnicode state)
			case RegexState::EscapeUnicode:
				if ((c >= '0' && c <= '9') || (c >= 'A' && c <= 'F') || (c >= 'a' && c <= 'f')) 
				{
					codeChar *= 16;
					codeChar += Util::hex2int(c);
					if (++codeCount == 4) 
					{
						seqStack.push_back(new Character(codeChar));
						STATE_RETURN;
					}
				} 
				else 
				{
					throw new RegexException(expression, pos, "Invalid character. Expected unicode hex code.");
				}
				break;
			// 4. When inside an hexadecimal escape sequence (EscapeHex state)
			case RegexState::EscapeHex:
				if ((c >= '0' && c <= '9') || (c >= 'A' && c <= 'F') || (c >= 'a' && c <= 'f')) 
				{
					codeChar *= 16;
					codeChar += Util::hex2int(c);
					if (++codeCount == 2) 
					{
						seqStack.push_back(new Character(codeChar));
						STATE_RETURN;
					}
				} 
				else 
				{
					throw RegexException(expression, pos, "Invalid character. Expected escape hex code.");
				}
				break;
			// 5. When inside an octal escape sequence (EscapeOctal state)
			case RegexState::EscapeOctal:
				if (c >= '0' && c <= '7') 
				{
					codeChar *= 8;
					codeChar += Util::oct2int(c);
					if (++codeCount == 3) 
					{
						seqStack.push_back(new Character(codeChar));
						STATE_RETURN;
					}
				} 
				else 
				{
					throw RegexException(expression, pos, "Invalid character. Expected escape octal code.");
				}
				break;
			// 6. When beggining an explicit quantifier '{m,n}', after the first '{' was read (ExplicitQuantifier1 state)
			// codeChar and codeCount must be 0
			case RegexState::ExplicitQuantifier1:
				switch (c) 
				{
				// 6.1. If its a digit, add the digit to the current first numeric value
				case '0': case '1': case '2': case '3': case '4': case '5': case '6': case '7': case '8': case '9':
					if (++codeCount <= 10) 
					{
						codeChar *= 10;
						codeChar += Util::dec2int(c);
					}
					break;
				// 6.2. If the end of the quantifier was reached, it was a fixed-value quantifier, deal with it
				case '}':
					if (seqStack.empty())
					{
						throw RegexException(expression, pos, "Expected expression before quantifier '{}'");
					}
					top = seqStack.back();
					seqStack.pop_back();
					seqStack.push_back(new Quantified(top, codeChar, codeChar));
					top = nullptr;
					STATE_RETURN;
					break;
				// 6.3. If a comma separator is found, there is another number on the quantifier. Then the number we just read is the minimum
				// the next number is going to be the maximum
				case ',':
					min = codeChar;
					codeChar = codeCount = 0;
					state = RegexState::ExplicitQuantifier2;
					break;
				// 6.4. If any other character is read, throw an error.
				default:
					throw RegexException(expression, pos, "Invalid character. Expected decimal quantifier.");
				}
				break;
			// 7. When reading the maximum value of an explicit quantifier  (ExplicitQuantifier2 state)
			// min must contain the minimum value for the quantifier 
			// codeChar and codeCount must contain 0
			case RegexState::ExplicitQuantifier2:
				switch (c) 
				{
				// 7.1. When a digit is found, it is added to the current maximum number
				case '0': case '1': case '2': case '3': case '4': case '5': case '6': case '7': case '8': case '9':
					if (++codeCount <= 10) 
					{
						codeChar *= 10;
						codeChar += Util::dec2int(c);
					}
					break;
				// 7.2. When the end of the quantifier is reached, create the quantifier.
				// If the maximum wasn't specified, it is set to UINT_MAX
				case '}':
					if (seqStack.empty())
					{
						throw RegexException(expression, pos, "Expected expression before quantifier '{}'");
					}
					top = seqStack.back();
					seqStack.pop_back();
					if (codeChar == 0) 
					{ 
						codeChar = UINT_MAX;
					}
					seqStack.push_back(new Quantified(top, min, codeChar));
					top = nullptr;
					STATE_RETURN;
					break;
				// 7.3. If any other character is received, it's an error.
				default:
					throw RegexException(expression, pos, "Invalid character. Expected decimal quantifier.");
				}
				break;
			// 8. When starting a new character class, right after the '['. (ClassFirst state)
			case RegexState::ClassFirst:
				switch (c) 
				{
					// 8.1. If it is an escape character, process it.
					case '\\':
						STATE_CALL(RegexState::EscapeSequence, RegexState::ClassReturnEscape);
						break;
					// 8.2. If it is an negation character, set a flag to indicate the class is negated.
					case '^':
						invertClass = true;
						break;
					// 8.3. If it is the end of the stream, throw an error.
					case 0x00: case 0x03: case 0x04:
						throw RegexException(expression, pos, "Unexpected end of expression inside a Character Class.");
					// 8.4. If any other character, just set it as the lastChar.
					default:
						lastChar = c;
						state = RegexState::Class;
						break;
				}
				break;
			// 9. When returning from an Escape sequence to the Class state, reads the escaped character to the lastChar variable.
			case RegexState::ClassReturnEscape:
				if (seqStack.back()->type() == CharClass::TYPE) 
				{
					CharClass* tempClass = reinterpret_cast<CharClass*>(seqStack.back());
					charClass->add(tempClass);
					lastChar = UINT_MAX;
				} 
				else 
				{
					lastChar = reinterpret_cast<Character*>(seqStack.back())->getChar();
				}
				delete seqStack.back();
				seqStack.pop_back();
				state = RegexState::Class;
				// no breaks here, intentionally
			// 10. When in the middle of a Character Class Definition
			case RegexState::Class:
				RegexState_Class:
				switch (c) 
				{
					// 10.1. When there is an escape character, parse it.
					case '\\':
						if (lastChar != UINT_MAX) 
						{
							charClass->add(lastChar);
						}
						STATE_CALL(RegexState::EscapeSequence, RegexState::ClassReturnEscape);
						break;
					// 10.2. When it's the end of the class.
					case ']':
						if (lastChar != UINT_MAX) 
						{
							charClass->add(lastChar);
						}
						if (invertClass) 
						{
							charClass->invert();
						}
						seqStack.push_back(charClass);
						charClass = nullptr;
						STATE_RETURN;
						break;
					// 10.3. When it's the '-' char, it could be the beggining of a range, or just the char itself.
					case '-':
						if (lastChar != UINT_MAX) 
						{	// when there is a char before, it's a range
							state = RegexState::ClassRange;
						} 
						else 
						{ // else it's just the '-' character
							lastChar = '-';
						}
						break;
					// 10.4. When it's the end of the stream, it's an error.
					case 0x00: case 0x03: case 0x04:
						throw RegexException(expression, pos, "Unexpected end of expression inside a Character Class.");
					// 10.5. For any other character, just add it as is.
					default:
						if (lastChar != UINT_MAX) 
						{
							charClass->add(lastChar);
						}
						lastChar = c;
						break;
				}
				break;
			// 11. When returning from an Escape that was called from a ClassRange, add the char as the maximum value
			// of the range
			case RegexState::ClassRangeReturnEscape:
				if (seqStack.back()->type() != CharClass::TYPE) 
				{
					charClass->add(lastChar, reinterpret_cast<Character*>(seqStack.back())->getChar());
					lastChar = UINT_MAX;
					state = RegexState::Class;
					delete seqStack.back();
					seqStack.pop_back();			
					goto RegexState_Class;
				} 
				else 
				{
					// If the escape wasn't a single character, ignore it.
					state = RegexState::ClassRange;
					delete seqStack.back();
					seqStack.pop_back();
				}
				// no breaks here, intentionally
			// 12. When getting the maximum value of a range of characters inside a class definition
			case RegexState::ClassRange:
				switch (c) 
				{
					// 12.1. if an escape character is found, parse it
					case '\\':
						STATE_CALL(RegexState::EscapeSequence, RegexState::ClassRangeReturnEscape);
						break;
					// 12.2. if the end of the class is found, add the lastChar and the '-' char to the class
					case ']':
						charClass->add(lastChar);
						charClass->add('-');
						if (invertClass) 
						{
							charClass->invert();
						}
						seqStack.push_back(charClass);
						charClass = nullptr;
						STATE_RETURN;
						break;
					// 12.3. When it's the end of the stream, it's an error.
					case 0x00: case 0x03: case 0x04:
						throw RegexException(expression, pos, "Unexpected end of expression inside a Character Class.");
					// 12.4. If any other character is found, consider it as the maximum value of a range of characters
					default:
						charClass->add(lastChar, c);
						lastChar = UINT_MAX;
						state = RegexState::Class;
						break;
				}
				break;
			}
		}
	// Exit gracefully (makes sure everything is deleted properly).
	parse_exit:
		// If the expression ends by length, check if it ended on an accepted state
		if (state != RegexState::MakeAlternative) 
		{
			throw RegexException(expression, pos, "Unexpected end of expression.");
		}
		// If there is something on the stack, return it.
		if (!seqStack.empty()) 
		{
			seq = new Sequence();
			while (!seqStack.empty()) 
			{
				seq->add(seqStack.front());
				seqStack.pop_front();
			}
			alt->add(seq);
			seq = nullptr;
		}
		delete seq;
		delete top;
		delete charClass;
		if (alt->expr.size() == 1) return alt->expr[0];
		else return alt;
	// Exit throwing an error (makes sure everything is deleted properly).
	parse_throw_exception:
		while (!seqStack.empty()) 
		{
			delete seqStack.back();
			seqStack.pop_back();
		}
		delete seq;
		delete top;
		delete charClass;
		throw RegexException(expression, pos, error);
	#undef STATE_RETURN
	#undef STATE_CALL
	}

	#ifdef _DEBUG
	void Regex::print(int level)
	{
		for (int i = 0; i < level; i++) 
		{
			cout << '.';
		}
		cout << "REGEX(@" << hex << this << ", SZ=" << dec << expr.size() << ")" << endl;
		level++;
		for (ICompilable* comp: expr) 
		{
			comp->print(level);
		}
	}
	#endif

} /* namespace pgen */
