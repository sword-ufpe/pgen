/**
 * \author Dimas Melo Filho
 * \date 2015-01-07
 * \file
 * This file contains the declaration of the LLStar class. This class is responsible for generating a LL(*) parser in a
 * recursive descent way, for parsing a specified language. The class requires access to the Language and its tokens in
 * order to generate code for parsing it.
 */
 
#ifndef LLSTAR_H_
#define LLSTAR_H_

namespace pagen { class LLStar; }

// STL
#include <string>
#include <set>
// Other
#include "Language.h"

using namespace std;
 
namespace pagen {
	
	class LLStar {
	public:
		Language* language;
		
		LLStar(Language* language);
		virtual ~LLStar();
	};
};
 
#endif /* LLSTAR_H_ */