/**
 * \author Dimas Melo Filho
 * \date 2015-01-07
 * \file
 * This file contains the implementation of the LLStar class. This class is responsible for generating a LL(*) parser 
 * in a recursive descent way, for parsing a specified language. The class requires access to the Language and its 
 * tokens in order to generate code for parsing it.
 */
 
#include "LLStar.h"
 
using namespace std;
namespace pagen {

//----------------------------------------------------------------------------------------------------------------------
LLStar::LLStar(Language* language)
 : language(language) 
{
}

//----------------------------------------------------------------------------------------------------------------------
LLStar::~LLStar() {
}

};