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
 * \date 2014-07-07
 * \file
 * This is the most different class from this directory. Its main function is to provide a way to compile many regular 
 * expressions into a single file, while removing the redundant expressions. That is, the code for a single expression 
 * will not be added twice to the compiled file.
 * It also provides the helper code that is needed by many of the compiled functions.
 */

// STL
#include <sstream>
#include <algorithm>
// pgen
#include "Code.h"
#include "Alternative.h"
#include "Character.h"
#include "CharClass.h"
#include "Quantified.h"
#include "Range.h"
#include "Sequence.h"
#include "Text.h"
#include "../misc/Util.h"

namespace pgen 
{
	const string Code::headerCode(
		"typedef struct _ast_node {"																				"\n"
		" int tokenId;"																								"\n"
		" char* data;"																								"\n"
		" struct _ast_node* nextSibling;"																			"\n"
		" struct _ast_node* firstChild;"																			"\n"
		"} ast_node;"																								"\n"
		""																											"\n"
		"typedef struct _token {"																					"\n"
		" int type;"																								"\n"
		" char* value;"																								"\n"
		"} token;"																									"\n"
		""																											"\n"
		"typedef struct _token_list {"																				"\n"
		" token* items;"																							"\n"
		" char* data;"																								"\n"
		" int count;"																								"\n"
		" char* data_ptr;"																							"\n"
		"} token_list;"																								"\n"
		""																											"\n"
		"typedef struct _parse_result {"																			"\n"
		" token_list* tokens;"																						"\n"
		" ast_node* ast;"																							"\n"
		"} parse_result;"																							"\n"
		""																											"\n"
		"ast_node* ast_new_node();"																					"\n"
		"void ast_add_child(ast_node* parent, ast_node* child);"													"\n"
		"void ast_add_sibling(ast_node* sibling, ast_node* sibling_new);"											"\n"
		"void ast_free(ast_node* node);"																			"\n"
		"void ast_clear(ast_node* node);"																			"\n"
		"ast_node* ast_invert_siblings(ast_node* node, ast_node* newNextSibling);"									"\n"
		"void token_list_free(token_list* tokens);"																	"\n"
		"parse_result* parse_result_new();"																			"\n"
		"void parse_result_free(parse_result* pr);"																	"\n"
	);
	const string Code::helperCode(
		"#include <stdlib.h>"																						"\n"
		"#include <stdio.h>"																						"\n"
		"#include <errno.h>"																						"\n"
		"#include <string.h>"																						"\n"
		"#include <errno.h>"																						"\n"
		""																											"\n"
		"unsigned int next_utf8(const char * text, int * pos) {"													"\n"
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
		" return 0xFFFD;"																							"\n"
		"}"																											"\n"
		""																											"\n"
		"unsigned int strlen_utf8(const char * text) {"																	"\n"
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
		"				((int (*)(const char*, chainptr*))chain[1].subchain[0].func)(text, &chain[1].subchain[1]) : \\\n"
		"				((int (*)(const char*))chain[1].subchain[0].func)(text) \\"									"\n"
		"		) : 0 \\"																							"\n"
		"	) : ( \\"																								"\n"
		"		chain[0].func != NULL ? \\"																			"\n"
		"		( \\"																								"\n"
		"			chain[1].func != NULL ? \\"																		"\n"
		"				((int (*)(const char*, chainptr*))chain[0].func)(text, &chain[1]) : \\"						"\n"
		"				((int (*)(const char*))chain[0].func)(text) \\"												"\n"
		"		) : 0 \\"																							"\n"
		"	) \\"																									"\n"
		")"																											"\n"
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
		"ast_node* ast_invert_siblings(ast_node* node, ast_node* newNextSibling) {"									"\n"
		" ast_node* next;"																							"\n"
		" while (node != 0) {"																						"\n"
		"  node->firstChild = ast_invert_siblings(node->firstChild, NULL);"											"\n"
		"  next = node->nextSibling;"																				"\n"
		"  node->nextSibling = newNextSibling;"																		"\n"
		"  newNextSibling = node;"																					"\n"
		"  node = next;"																							"\n"
		" }"																										"\n"
		" return newNextSibling;"																					"\n"
		"}"																											"\n"
		/** token_list_init(int num_tokens, int data_len) */
		"token_list* token_list_init(int num_tokens, int data_len) {"      											"\n"
		" token_list* tokens = (token_list*) malloc(sizeof(token_list));"  											"\n"
		" tokens->items = (token*) malloc(num_tokens * sizeof(token));"    											"\n"
		" tokens->data = (char*) malloc(data_len + num_tokens);"           											"\n"	// data_len + space for num_token times 0x00
		" tokens->data_ptr = tokens->data;"                                											"\n"
		" tokens->count = 0;"                                              											"\n"
		" return tokens;"																							"\n"
		"}"																											"\n"
		""																											"\n"
		/** Add the token_list_free(token_list* tokens) */
		"void token_list_free(token_list* tokens) {"                       											"\n"
		" free(tokens->items);"                                            											"\n"
		" free(tokens->data);"                                             											"\n"
		" free(tokens);"                                                   											"\n"
		"}"                                                                											"\n"
		""																											"\n"
		/** Add the token_list_add(token_list* tokens, int type, char* value, int value_len) */
		"void token_list_add(token_list* tokens, int type, const char* value, int value_len) {" 							"\n"
		" token* tok = &tokens->items[tokens->count];"                    											"\n"
		" tok->type = type;"                                             											"\n"
		" tok->value = tokens->data_ptr;"                                											"\n"
		" memcpy(tok->value, value, value_len);"                      												"\n"
		" tok->value[value_len] = 0;"                               	    										"\n"
		" tokens->data_ptr += value_len+1;"                               											"\n"
		" tokens->count++;"																							"\n"
		"}"                                                                											"\n"
		""																											"\n"
		"parse_result* parse_result_new() {"																		"\n"
		" parse_result* pr = (parse_result*) malloc(sizeof(parse_result));"											"\n"
		" if (pr != NULL) memset(pr, 0, sizeof(parse_result));"														"\n"
		" return pr;"																								"\n"
		"}"																											"\n"
		""																											"\n"
		"void parse_result_free(parse_result* pr) {"																"\n"
		" if (pr->ast != NULL) ast_free(pr->ast);"																	"\n"
		" if (pr->tokens != NULL) token_list_free(pr->tokens);"														"\n"
		" free(pr);"																								"\n"
		"}"																											"\n"
		""																											"\n"
		);
		
	Code::Code() 
	{
	}

	void Code::add(ICompilable* obj) 
	{
		string funcname = obj->funcname();
		if (methodList.find(funcname) == methodList.end()) 
		{
			// Add dependences of non-trivial types (types which depend on sub-objects or sequences of objects).
			switch (obj->type()) 
			{
			// Types in which all dependences are used by funcname
			case Alternative::TYPE:
			case Quantified::TYPE: 
				{
					vector<ICompilable*> deps = obj->dependences();
					for (ICompilable* dep: deps) 
					{
						add(dep);
					}
					break;
				}
			// Sequence type, all dependences are used by cfuncname except for the last one which is used by funcname
			case Sequence::TYPE: 
				{
					vector<ICompilable*> deps = obj->dependences();
					for (unsigned int i = 0; i < deps.size()-1; i++) 
					{
						cadd(deps[i]);
					}
					add(deps.back());
					break;
				}
			}
			// Add the method name for the object
			_code << obj->compile();
			methodList.insert(funcname);
			prototypeList.insert(obj->prototype());
		}
	}

	void Code::cadd(ICompilable* obj) 
	{
		string cfuncname = obj->cfuncname();
		if (methodList.find(cfuncname) == methodList.end()) 
		{
			// Add dependences of non-trivial types (types which depend on sub-objects or sequences of objects).
			switch (obj->type())
			{
			// Types in which all dependences are used by funcname
			case Alternative::TYPE:
			case Quantified::TYPE: 
				{
					vector<ICompilable*> deps = obj->dependences();
					for (ICompilable* dep: deps) 
					{
						add(dep);
					}
					break;
				}
			// Sequence type, all dependences are used by cfuncname
			case Sequence::TYPE: 
				{
					vector<ICompilable*> deps = obj->dependences();
					for (ICompilable* dep: deps) 
					{
						cadd(dep);
					}
					break;
				}
			}
			// Add the method name for the object
			_code << obj->ccompile();
			methodList.insert(cfuncname);
			prototypeList.insert(obj->cprototype());
		}
	}

	const std::string Code::code() 
	{
		stringstream s;
		for (auto proto: prototypeList)
		{
			s << proto << ";\n";
		}
		s << _code.str();
		return s.str();
	}

	const std::string & Code::helper() 
	{
		return Code::helperCode;
	}

	const std::string & Code::getHelper() 
	{
		return Code::helperCode;
	}
	
	const std::string & Code::getHeader()
	{
		return Code::headerCode;
	}

}; /* namespace pgen */
