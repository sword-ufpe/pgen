/*
 * LanguageException.cpp
 *
 *  Created on: Jan 07, 2015
 *      Author: Dimas Melo Filho
 */

// pgen
#include "LanguageException.h"

namespace pgen 
{

	LanguageException::LanguageException(std::string& error)
	 : error(error) 
	{
	}

	LanguageException::LanguageException(std::string error)
	 : error(error) 
	{
	}

	LanguageException::LanguageException(const char* error)
	 : error(error)
	{
	}

	LanguageException::~LanguageException() throw () 
	{
	}

	const char * LanguageException::what() const throw () 
	{
		return error.c_str();
	}
	
} /* namespace pgen */
