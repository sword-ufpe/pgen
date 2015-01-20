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

namespace pagen {

const string Code::helperCode(
		"#include <stdlib.h>\n"
		"#include <stdio.h>\n"
		"#include <errno.h>\n"
		"#include <string.h>\n"
		"\n"
		"unsigned int next_utf8(char * text, int * pos) {\n"
		"\tregister unsigned int c,d;\n"
		"\tc = text[(*pos)++];\n"
		"\tif ((c & 0x80) == 0) return c;\n"
		"\tif ((c & 0xE0) == 0xC0) {\n"
		"\t\tc &= 0x3F;\n"
		"\t\tc <<= 6;\n"
		"\t\td = text[(*pos)++];\n"
		"\t\tif ((d & 0x80) == 0) goto utf8_error;\n"
		"\t\tc |= (d & 0x3F);\n"
		"\t\treturn c;\n"
		"\t}\n"
		"\tif ((c & 0xF0) == 0xE0) {\n"
		"\t\tc &= 0x1F;\n"
		"\t\tc <<= 6;\n"
		"\t\td = text[(*pos)++];\n"
		"\t\tif ((d & 0x80) == 0) goto utf8_error;\n"
		"\t\tc |= (d & 0x3F);\n"
		"\t\tc <<= 6;\n"
		"\t\td = text[(*pos)++];\n"
		"\t\tif ((d & 0x80) == 0) goto utf8_error;\n"
		"\t\tc |= (d & 0x3F);\n"
		"\t\treturn c;\n"
		"\t}\n"
		"\tif ((c & 0xF8) == 0xF0) {\n"
		"\t\tc &= 0x0F;\n"
		"\t\tc <<= 6;\n"
		"\t\td = text[(*pos)++];\n"
		"\t\tif ((d & 0x80) == 0) goto utf8_error;\n"
		"\t\tc |= (d & 0x3F);\n"
		"\t\tc <<= 6;\n"
		"\t\td = text[(*pos)++];\n"
		"\t\tif ((d & 0x80) == 0) goto utf8_error;\n"
		"\t\tc |= (d & 0x3F);\n"
		"\t\tc <<= 6;\n"
		"\t\td = text[(*pos)++];\n"
		"\t\tif ((d & 0x80) == 0) goto utf8_error;\n"
		"\t\tc |= (d & 0x3F);\n"
		"\t\treturn c;\n"
		"\t}\n"
		"\tutf8_error:\n"
		"\tprintf(\"Invalid UTF-8 character on source file.\");\n"
		"\tabort();\n"
		"}\n"
		"\n"
		"unsigned int strlen_utf8(char * text) {\n"
		"\tint pos = 0;\n"
		"\twhile (next_utf8(text, &pos) != 0);\n"
		"\treturn (pos-1);\n"
		"}\n"
		"\n"
		"typedef struct _stack_int {\n"
		"\tint* data;\n"
		"\tunsigned int size;\n"
		"\tint top;\n"
		"} stack_int;\n"
		"\n"
		"void stack_int_init(stack_int* s) {\n"
		"\ts->size = 32;\n"
		"\ts->data = (int*) malloc(sizeof(int) * s->size);\n"
		"\ts->top = 0;\n"
		"}\n"
		"\n"
		"void stack_int_free(stack_int* s) {\n"
		"\tif (s != NULL && s->data != NULL) free(s->data);\n"
		"}\n"
		"\n"
		"int stack_int_grow(stack_int* s) {\n"
		"\ts->size *= 2;\n"
		"\tvoid* ndata = realloc(s->data, sizeof(int) * s->size);\n"
		"\tif (ndata == NULL) s->size /= 2;\n"
		"\telse s->data = (int*) ndata;\n"
		"\treturn (s->data == ndata);\n"
		"}\n"
		"\n"
		"void stack_int_push(stack_int* s, int val) {\n"
		"\tif (s->top >= s->size && !stack_int_grow(s)) {\n"
		"\t\tprintf(\"Could not allocate enough memory for a regex stack.\");\n"
		"\t\tabort();\n"
		"\t}\n"
		"\ts->data[s->top++] = val;\n"
		"}\n"
		"\n"
		"int stack_int_pop(stack_int *s) {\n"
		"\tif (s->top > 0) return s->data[--s->top];\n"
		"\treturn 0;\n"
		"}\n"
		"\n"
		/** The following union makes sure that the space used by the function pointer array
		 * also allows saving data pointers on them.
		 */
		"typedef union _chainptr {\n"
		"\tint (*func)();\n"
		"\tunion _chainptr * subchain;\n"
		"} chainptr;\n\n"
		/**
		 * The following function is a dummy function to be used as an indicator that the
		 * next pointer in the chain function array is a pointer to the next chain array
		 * instead of the next chain function.
		 */
		"int CHAIN_JUMP() { return -1; }\n\n"
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
		"#define chain_next(text, chain) \\\n"
		"( \\\n"
		"	chain[0].func == CHAIN_JUMP ? \\\n"
		"	( \\\n"
		"		chain[1].subchain[0].func != NULL ? \\\n"
		"		( \\\n"
		"			chain[1].subchain[1].func != NULL ? \\\n"
		"				((int (*)(char*, chainptr*))chain[1].subchain[0].func)(text, &chain[1].subchain[1]) : \\\n"
		"				((int (*)(char*))chain[1].subchain[0].func)(text) \\\n"
		"		) : 0 \\\n"
		"	) : ( \\\n"
		"		chain[0].func != NULL ? \\\n"
		"		( \\\n"
		"			chain[1].func != NULL ? \\\n"
		"				((int (*)(char*, chainptr*))chain[0].func)(text, &chain[1]) : \\\n"
		"				((int (*)(char*))chain[0].func)(text) \\\n"
		"		) : 0 \\\n"
		"	) \\\n"
		")\n"
		"\n"
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
