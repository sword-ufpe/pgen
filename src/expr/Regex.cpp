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
 * \date 2014-10-04
 * \file
 * This is a file that implements the class Regex, a class that is an ICompilable object that can parse a regular 
 * expression (text).
 * This is a special class, it behaves as an Alternative class that is able to parse a textual Regular Expression into a
 * memory structure that uses all the other classes presented above. This class is used to parse the textual regular 
 * expression into a representation that can be used directly by the rest of the application.
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
#include "RegexEscape.h"
#include "../parser/NamedClassManager.h"
#include "../misc/Util.h"
#include "../misc/RegexException.h"

using namespace std;
namespace pgen 
{
	
	Regex::Regex(string expression, NamedClassManager& ncm) 
	{
		this->parse(expression, ncm);
	}

	Regex::~Regex() 
	{
	}

	ICompilable* Regex::parse(string& expression, NamedClassManager& ncm) 
	{
		unsigned int pos = 0;
		this->group = 0;
		return this->parse(expression,pos,this, ncm);
	}

	ICompilable* Regex::parse(string& expression, unsigned int &pos, Alternative* alt, NamedClassManager& ncm) 
	{
	#define STATE_CALL(st, ret) stateStack.push_back(ret); state = st
	#define STATE_RETURN state = stateStack.back(); stateStack.pop_back() 
		RegexState state = RegexState::MakeAlternative;
		deque<ICompilable*> seqStack;	//< Stack of Sequences
		deque<RegexState> stateStack;	//< Stack of states for parsing the expressions
		Sequence* seq = nullptr;
		ICompilable* top = nullptr;
		string error;
		unsigned int codeChar, codeCount, min;
		while (pos < expression.length()) 
		{
			unsigned int c = Util::next_utf8(expression, pos);
			switch (state) 
			{
			// 1. When on Alternative state
			case RegexState::MakeAlternative:
				switch (c) 
				{
				// 1.1. Escape start on alternative state
				case '\\':
					seqStack.push_back(RegexEscape::parse(expression, pos));
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
					seqStack.push_back(CharClass::parse(expression, pos, ncm));
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
					seqStack.push_back(this->parse(expression, pos, new Alternative(), ncm));
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
					seqStack.push_back(CharClass::dot());
					break;
				// 1.12. Any other character is pushed as a character to be matched.
				default:
					seqStack.push_back(new Character(c));
					break;
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
