/*
 * Range.cpp
 *
 *  Created on: Jun 13, 2014
 *      Author: Dimas Melo Filho
 */

// STL
#include <sstream>
#include <iostream>
// pgen
#include "Range.h"

using namespace std;
namespace pgen {

	bool Range::in(unsigned int c) 
	{
		return (c >= start && c <= end);
	}

	bool Range::before(unsigned int c) 
	{
		return (c < start);
	}

	bool Range::after(unsigned int c) 
	{
		return (c > end);
	}

	bool Range::join(unsigned int c) 
	{
		if (c == 0)
		{ 
			return true;
		}
		if (c+1 < start || c-1 > end) 
		{ 
			return false;
		}
		start = (c < start ? c : start);
		end = (c > end ? c : end);
		return true;
	}

	bool Range::join(unsigned int s, unsigned int e) 
	{
		if (e+1 < start || s-1 > end)
		{ 
			return false;
		}
		start = (s < start ? s : start);
		end = (e > end ? e : end);
		if (start == 0) 
		{ 
			start++;
		}
		if (end == 0) 
		{ 
			end++;
		}
		return true;
	}

	bool Range::join(const Range & r) 
	{
		return join(r.start, r.end);
	}

	bool Range::join(Range * r) 
	{
		return join(r->start, r->end);
	}

	bool Range::operator==(const Range & r) const 
	{
		return (this->start == r.start && this->end == r.end);
	}

	bool Range::operator!=(const Range & r) const 
	{
		return (this->start != r.start || this->end != r.end);
	}

	Range::Range(unsigned int start, unsigned int end)
	 : next(nullptr)
	 , prev(nullptr) 
	{
		if (start < end) 
		{
			this->start = start;
			this->end = end;
		} 
		else 
		{
			this->end = start;
			this->start = end;
		}
		if (this->start == 0)
		{ 
			this->start++;
		}
		if (this->end == 0)
		{ 
			this->end++;
		}
	}

	Range::~Range() 
	{
	}

	/**
	 * compiled code:
	 * %cprototype()% {
	 *  int pos = 0, pchain = 0;
	 *  unsigned int c = next_utf8(text, &pos);
	 *  if (c >= %start% && c <= %end%) {
	 *   pchain = %chain%(text + pos);
	 *   if (pchain >= 0) return pos + pchain;
	 *  }
	 *  return -1;
	 * }
	 */
	string Range::ccompile() 
	{
		stringstream s;
		s << this->cprototype() << " {"											"\n"
			 " int pos = 0, pchain = 0;"										"\n"
			 " unsigned int c = next_utf8(text, &pos);"							"\n"
			 " if (c >= " << this->start << " && c <= " << this->end << ") {"	"\n"
			 "  pchain = chain_next(text + pos, chain);"						"\n"
			 "  if (pchain >= 0) return pos + pchain;"							"\n"
			 " }"																"\n"
			 " return -1;"														"\n"
			 "}"																"\n\n";
		return s.str();
	}

	/**
	 * compiled code:
	 * *prototype* {
	 *  int pos = 0;
	 *  unsigned int c = next_utf8(text, &pos);
	 *  if (c >= %start% && c <= %end%) return pos;
	 *  return -1;
	 * }
	 */
	string Range::compile() 
	{
		stringstream s;
		s << this->prototype() << " {"													"\n"
			 " int pos = 0;"															"\n"
			 " unsigned int c = next_utf8(text, &pos);"									"\n"
			 " if (c >= " << this->start << " && c <= " << this->end << ") return pos;"	"\n"
			 " return -1;"																"\n"
			 "}"																		"\n\n";
		return s.str();
	}

	string Range::name() 
	{
		stringstream s;
		s << "ra_" << this->start << "_" << this->end;
		return s.str();
	}

	/**
	 * @returns the type identifier of this class.
	 */
	int Range::type() 
	{ 
		return TYPE; 
	}
	
	#ifdef _DEBUG
	void Range::print(int level)
	{
		for (int i = 0; i < level; i++) 
		{
			cout << '.';
		}
		cout << "RANGE(@" << hex << this << ", RNG=[" << dec << start << ',' << end << "])" << endl;
	}
	#endif

} /* namespace pgen */
