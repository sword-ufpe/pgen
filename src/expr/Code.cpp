/*
 * Lexicon.cpp
 *
 *  Created on: Jun 7, 2014
 *      Author: Dimas Melo Filho
 */


#include "Code.h"
#include "Alternative.h"
#include "Character.h"
#include "CharClass.h"
#include "Quantified.h"
#include "Range.h"
#include "Sequence.h"
#include "Text.h"
#include "../misc/Util.h"
#include <sstream>
#include <algorithm>

namespace pgen {

const string Code::helperCode(
	"#include <stdlib.h>"																						"\n"
	"#include <stdio.h>"																						"\n"
	"#include <errno.h>"																						"\n"
	"#include <string.h>"																						"\n"
	""																											"\n"
	"unsigned int next_utf8(char * text, int * pos) {"															"\n"
	" register unsigned int c,d;"																				"\n"
	" c = text[(*pos)++];"																						"\n"
	" if ((c & 0x80) == 0) return c;"																			"\n"
	" if ((c & 0xE0) == 0xC0) {"																				"\n"
	"  c &= 0x3F;"																								"\n"
	"  c <<= 6;"																								"\n"
	"  d = text[(*pos)++];"																						"\n"
	"  if ((d & 0x80) == 0) goto utf8_error;"																	"\n"
	"  c |= (d & 0x3F);"																						"\n"
	"  return c;"																								"\n"
	" }"																										"\n"
	" if ((c & 0xF0) == 0xE0) {"																				"\n"
	"  c &= 0x1F;"																								"\n"
	"  c <<= 6;"																								"\n"
	"  d = text[(*pos)++];"																						"\n"
	"  if ((d & 0x80) == 0) goto utf8_error;"																	"\n"
	"  c |= (d & 0x3F);"																						"\n"
	"  c <<= 6;"																								"\n"
	"  d = text[(*pos)++];"																						"\n"
	"  if ((d & 0x80) == 0) goto utf8_error;"																	"\n"
	"  c |= (d & 0x3F);"																						"\n"
	"  return c;"																								"\n"
	" }"																										"\n"
	" if ((c & 0xF8) == 0xF0) {"																				"\n"
	"  c &= 0x0F;"																								"\n"
	"  c <<= 6;"																								"\n"
	"  d = text[(*pos)++];"																						"\n"
	"  if ((d & 0x80) == 0) goto utf8_error;"																	"\n"
	"  c |= (d & 0x3F);"																						"\n"
	"  c <<= 6;"																								"\n"
	"  d = text[(*pos)++];"																						"\n"
	"  if ((d & 0x80) == 0) goto utf8_error;"																	"\n"
	"  c |= (d & 0x3F);"																						"\n"
	"  c <<= 6;"																								"\n"
	"  d = text[(*pos)++];"																						"\n"
	"  if ((d & 0x80) == 0) goto utf8_error;"																	"\n"
	"  c |= (d & 0x3F);"																						"\n"
	"  return c;"																								"\n"
	" }"																										"\n"
	" utf8_error:"																								"\n"
	" printf(\"Invalid UTF-8 character on source file.\");"														"\n"
	" abort();"																									"\n"
	"}"																											"\n"
	""																											"\n"
	"unsigned int strlen_utf8(char * text) {"																	"\n"
	" int pos = 0;"																								"\n"
	" while (next_utf8(text, &pos) != 0);"																		"\n"
	" return (pos-1);"																							"\n"
	"}"																											"\n"
	""																											"\n"
	"typedef struct _stack_int {"																				"\n"
	" int* data;"																								"\n"
	" unsigned int size;"																						"\n"
	" int top;"																									"\n"
	"} stack_int;"																								"\n"
	""																											"\n"
	"void stack_int_init(stack_int* s) {"																		"\n"
	" s->size = 32;"																							"\n"
	" s->data = (int*) malloc(sizeof(int) * s->size);"															"\n"
	" s->top = 0;"																								"\n"
	"}"																											"\n"
	""																											"\n"
	"void stack_int_free(stack_int* s) {"																		"\n"
	" if (s != NULL && s->data != NULL) free(s->data);"															"\n"
	"}"																											"\n"
	""																											"\n"
	"int stack_int_grow(stack_int* s) {"																		"\n"
	" s->size *= 2;"																							"\n"
	" void* ndata = realloc(s->data, sizeof(int) * s->size);"													"\n"
	" if (ndata == NULL) s->size /= 2;"																			"\n"
	" else s->data = (int*) ndata;"																				"\n"
	" return (s->data == ndata);"																				"\n"
	"}"																											"\n"
	""																											"\n"
	"void stack_int_push(stack_int* s, int val) {"																"\n"
	" if (s->top >= s->size && !stack_int_grow(s)) {"															"\n"
	"  printf(\"Could not allocate enough memory for a regex stack.\");"										"\n"
	"  abort();"																								"\n"
	" }"																										"\n"
	" s->data[s->top++] = val;"																					"\n"
	"}"																											"\n"
	""																											"\n"
	"int stack_int_pop(stack_int *s) {"																			"\n"
	" if (s->top > 0) return s->data[--s->top];"																"\n"
	" return 0;"																								"\n"
	"}"																											"\n"
	""																											"\n"
	/** The following union makes sure that the space used by the function pointer array
	 * also allows saving data pointers on them.
	 */
	"typedef union _chainptr {"																					"\n"
	" int (*func)();"																							"\n"
	" union _chainptr * subchain;"																				"\n"
	"} chainptr;"																								"\n"
	/**
	 * The following function is a dummy function to be used as an indicator that the
	 * next pointer in the chain function array is a pointer to the next chain array
	 * instead of the next chain function.
	 */
	"int CHAIN_JUMP() { return -1; }"																			"\n"
	/** the next lines defines a macro called chain_next(text,chain) which is equivalent of a function as follows:
	 * if (chain[0].func == CHAIN_JUMP) {
	 *   if (chain[1].subchain[0].func != NULL) {
	 *   	if (chain[1].subchain[1].func != NULL) {
	 *   		return (int (*)(char*, chainptr*))(chain[1].subchain[0].func)(text, &chain[1].subchain[1]);
	 *   	} else {
	 *   		return (int (*)(char*))(chain[1].subchain[0].func)(text);
	 *   	}
	 *   } else {
	 *   	return 0;
	 *   }
	 * } else if (chain[0].func != NULL) { // the next function in the chain should be called
	 *   if (chain[1].func != NULL) { // the next function in the chain is not the last one
	 *     return (int (*)(char*, int (*)()))(chain[0].func) (text, &chain[1]); // return what the next chain function returns.
	 *   } else { // the next function i the chain is the last one
	 *     return (int (*)(char*))(chain[0].func) (text); // return what the last chain function returns.
	 *   }
	 * } else { // there are no chain functions left to be called.
	 *   return 0;
	 * }
	 * As such, this macro must be used to call the next chain function on the chained version of the compiled methods.
	 * this macro can be interpreted as a value an must be summed with the methods' own index increment.
	 */
	"#define chain_next(text, chain) \\"																		"\n"
	"( \\"																										"\n"
	"	chain[0].func == CHAIN_JUMP ? \\"																		"\n"
	"	( \\"																									"\n"
	"		chain[1].subchain[0].func != NULL ? \\"																"\n"
	"		( \\"																								"\n"
	"			chain[1].subchain[1].func != NULL ? \\"															"\n"
	"				((int (*)(char*, chainptr*))chain[1].subchain[0].func)(text, &chain[1].subchain[1]) : \\"	"\n"
	"				((int (*)(char*))chain[1].subchain[0].func)(text) \\"										"\n"
	"		) : 0 \\"																							"\n"
	"	) : ( \\"																								"\n"
	"		chain[0].func != NULL ? \\"																			"\n"
	"		( \\"																								"\n"
	"			chain[1].func != NULL ? \\"																		"\n"
	"				((int (*)(char*, chainptr*))chain[0].func)(text, &chain[1]) : \\"							"\n"
	"				((int (*)(char*))chain[0].func)(text) \\"													"\n"
	"		) : 0 \\"																							"\n"
	"	) \\"																									"\n"
	")"																											"\n"
	""																											"\n"
	"typedef struct _ast_node {"																				"\n"
	" int tokenId;"																								"\n"
	" char* data;"																								"\n"
	" struct _ast_node* nextSibling;"																			"\n"
	" struct _ast_node* firstChild;"																			"\n"
	"} ast_node;"																								"\n"
	""																											"\n"
	"ast_node* ast_new_node() {"																				"\n"
	" ast_node* node = (ast_node*)malloc(sizeof(ast_node));"													"\n"
	" node->tokenId = -1;"																						"\n"
	" node->data = NULL;"																						"\n"
	" node->nextSibling = NULL;"																				"\n"
	" node->firstChild = NULL;"																					"\n"
	" return node;"																								"\n"
	"}"																											"\n"
	""																											"\n"
	"void ast_add_child(ast_node* parent, ast_node* child) {"													"\n"
	" child->nextSibling = parent->firstChild;"																	"\n"
	" parent->firstChild = child;"																				"\n"
	"}"																											"\n"
	""																											"\n"
	"void ast_add_sibling(ast_node* sibling, ast_node* sibling_new) {"											"\n"
	" sibling_new->nextSibling = sibling->nextSibling;"															"\n"
	" sibling->nextSibling = sibling_new;"																		"\n"
	"}"																											"\n"
	""																											"\n"
	"void ast_free(ast_node* node) {"																			"\n"
	" if (node == NULL) return;"																				"\n"
	" ast_free(node->firstChild);"																				"\n"
	" ast_free(node->nextSibling);"																				"\n"
	" free(node);"																								"\n"
	"}"																											"\n"
	""																											"\n"
	"void ast_clear(ast_node* node) {"																			"\n"
	" ast_free(node->firstChild);"																				"\n"
	" ast_free(node->nextSibling);"																				"\n"
	" node->firstChild = 0;"																					"\n"
	" node->nextSibling = 0;"																					"\n"
	"}"																											"\n"
	""																											"\n"
	);
	
//-------------------------------------------------------------------------------------------
Code::Code() {
	_code << helperCode;
}

//-------------------------------------------------------------------------------------------
void Code::add(ICompilable* obj) {
	string funcname = obj->funcname();
	if (methodList.find(funcname) == methodList.end()) {
		// Add dependences of non-trivial types (types which depend on sub-objects or sequences of objects).
		switch (obj->type()) {
		// Types in which all dependences are used by funcname
		case Alternative::TYPE:
		case Quantified::TYPE: 
			{
				vector<ICompilable*> deps = obj->dependences();
				for (ICompilable* dep: deps) {
					add(dep);
				}
				break;
			}
		// Sequence type, all dependences are used by cfuncname except for the last one which is used by funcname
		case Sequence::TYPE: 
			{
				vector<ICompilable*> deps = obj->dependences();
				for (unsigned int i = 0; i < deps.size()-1; i++) {
					cadd(deps[i]);
				}
				add(deps.back());
				break;
			}
		}
		// Add the method name for the object
		_code << obj->compile();
		methodList.insert(funcname);
	}
}

//-------------------------------------------------------------------------------------------
void Code::cadd(ICompilable* obj) {
	string cfuncname = obj->cfuncname();
	if (methodList.find(cfuncname) == methodList.end()) {
		// Add dependences of non-trivial types (types which depend on sub-objects or sequences of objects).
		switch (obj->type()) {
		// Types in which all dependences are used by funcname
		case Alternative::TYPE:
		case Quantified::TYPE: 
			{
				vector<ICompilable*> deps = obj->dependences();
				for (ICompilable* dep: deps) {
					add(dep);
				}
				break;
			}
		// Sequence type, all dependences are used by cfuncname
		case Sequence::TYPE: 
			{
				vector<ICompilable*> deps = obj->dependences();
				for (ICompilable* dep: deps) {
					cadd(dep);
				}
				break;
			}
		}
		// Add the method name for the object
		_code << obj->ccompile();
		methodList.insert(cfuncname);
	}
}

//-------------------------------------------------------------------------------------------
const std::string Code::code() {
	return _code.str();
}

//-------------------------------------------------------------------------------------------
const std::string & Code::helper() {
	return Code::helperCode;
}

//-------------------------------------------------------------------------------------------
const std::string & Code::getHelper() {
	return Code::helperCode;
}

};
