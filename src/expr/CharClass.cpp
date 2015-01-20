/*
 * Class.cpp
 *
 *  Created on: Jun 9, 2014
 *      Author: Dimas Melo Filho
 */

#include "CharClass.h"
#include <climits>
#include <string>
#include <sstream>
using namespace std;

namespace pagen {

//-------------------------------------------------------------------------------------------
void CharClass::add(unsigned int c) {
	add(c,c);
	// TODO zzz create a more efficient way of doing this with a single char, if possible.
}

//-------------------------------------------------------------------------------------------
void CharClass::add(unsigned int cstart, unsigned int cend) {
	Range * v = this->range, * t = nullptr;
	// If this is the first interval, just add it
	if (v == nullptr) {
		this->range = new Range(cstart, cend);
		return;
	}
	// If this isn't the first interval, try to find an interval to merge with
	// The search should stop if one of the following conditions is satisfied:
	// 1. An insert happened (in which case the method must merge, stop and return)
	// 2. The end of the list of intervals was reached (then the interval must be added to the end)
	// 3. All the following intervals' boundareis are greater than the proposed new interval (then the interval must be added to the current position)
	while (v != nullptr && cend+1 >= v->start) {
		if (v->join(cstart, cend)) { // if an insert happened
			this->merge(v); // merge the interval that changed nad return
			return;
		}
		t = v;
		v = v->next;
	}
	// if the search reached the end of the list, append a new interval to the end of the list.
	if (v == nullptr) {
		v = new Range(cstart, cend);
		v->prev = t;	// if the code reached this point, "t" will not be null. it represents the previous interval.
		v->next = t->next;
		t->next = v;
	}
	// else if there was no merging and it is not the end of the list, create a new interval and insert it on the list accordingly.
	else {
		t = new Range(cstart, cend);
		t->next = v;
		t->prev = v->prev;
		v->prev = t;
		if (t->prev == nullptr) this->range = t;
		else t->prev->next = t;
	}
}

//-------------------------------------------------------------------------------------------
void CharClass::add(CharClass * c) {
	Range * r = c->range;
	while (r != nullptr) {
		this->add(r->start, r->end);
		r = r->next;
	}
}

//-------------------------------------------------------------------------------------------
void CharClass::merge(Range * v) {
	Range * t;
	// merge backwards on the linked list, removing the merged elements
	while (v->prev != nullptr && v->prev->join(v)) {
		t = v;
		v = t->prev;
		v->next = t->next;
		if (v->next != nullptr) v->next->prev = v;
		delete t;
	}
	// merge forward on the linked list, removing the merged elements
	while (v->next != nullptr && v->next->join(v)) {
		t = v;
		v = t->next;
		v->prev = t->prev;
		if (v->prev != nullptr) v->prev->next = v;
		delete t;
	}
	// if the head element was merged, update the head pointer.
	if (v->prev == nullptr) this->range = v;
}

//-------------------------------------------------------------------------------------------
void CharClass::normalize() {
	if (negated) {
		invert();
		negated = false;
	}
}

//-------------------------------------------------------------------------------------------
void CharClass::invert() {
	unsigned int s = 1, e;
	Range * v = range;
	if (v == nullptr) {
		range = new Range(1,UINT_MAX);
		return;
	}
	// if the first interval starts from 1, then the inverse starts after 1.
	if (v != nullptr && v->start == 1) {
		e = v->end;
		s = v->end+1;
		range = v->next;
		delete v;
	}
	// change every following interval to its negation
	v = range;
	while (v != nullptr) {
		e = v->end;
		v->end = v->start-1;
		v->start = s;
		s = e + 1;
		if (v->next == nullptr) break;
		v = v->next;
	}

	// if the last interval didn't end in UINT_MAX create a new interval that contains
	// all the remaining characters from the current character to UINT_MAX
	if (e < UINT_MAX) {
		if (v != nullptr) {
			v->next = new Range(s,UINT_MAX);
			v->next->prev = v;
		} else {
			range = new Range(s,UINT_MAX);
		}
	}
}

//-------------------------------------------------------------------------------------------
CharClass::CharClass():
range(nullptr),
negated(false) {

}

//-------------------------------------------------------------------------------------------
CharClass::~CharClass() {
	Range * next;
	while (range != nullptr) {
		next = range->next;
		delete range;
		range = next;
	}
}

//-------------------------------------------------------------------------------------------
bool CharClass::operator==(const CharClass & c) const {
	if (c.negated != this->negated) return false;
	Range * vc = c.range, * vt = this->range;
	while (vc != nullptr && vt != nullptr) {
		if (*vc != *vt) return false;
		vc = vc->next;
		vt = vt->next;
	}
	return (vc == nullptr && vt == nullptr);
}

//-------------------------------------------------------------------------------------------
bool CharClass::operator!=(const CharClass &c) const {
	return !(*this == c);
}

//-------------------------------------------------------------------------------------------
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
string CharClass::ccompile() {
	stringstream s;
	s << this->prototype() << " {" << endl;
	s << "\tint pos = 0, pchain = 0;" << endl;
	s << "\tunsigned int c = next_utf8(text, &pos);" << endl;
	Range * r = this->range;
	while (r != nullptr) {
		s << "\tif (c >= " << r->start << " && " << "c <= " << r->end << ") {" << endl;
		s << "\t\tpchain = chain_next(text + pos, chain);" << endl;
		s << "\t\tif (pchain >= 0) return pchain + pos;" << endl;
		s << "\t}" << endl;
		r = r->next;
	}
	s << "return -1;" << endl << "}" << endl;
	return s.str();
}
//-------------------------------------------------------------------------------------------
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
string CharClass::compile() {
	stringstream s;
	s << this->prototype() << " {" << endl;
	s << "\tint pos = 0;" << endl;
	s << "\tunsigned int c = next_utf8(text, &pos);" << endl;
	Range * r = this->range;
	while (r != nullptr) {
		s << "if (c >= " << r->start << " && " << "c <= " << r->end << ") return pos;" << endl;
		r = r->next;
	}
	s << "return -1;" << endl << "}" << endl;
	return s.str();
}

//-------------------------------------------------------------------------------------------
string CharClass::name() {
	stringstream s;
	s << "cl";
	Range * r = this->range;
	while (r != nullptr) {
		s << "_" << r->start << "_" << r->end;
		r = r->next;
	}
	return s.str();
}

//-------------------------------------------------------------------------------------------
int CharClass::type() { return TYPE; }

} /* namespace pagen */
