/*
 * LanguageException.h
 *
 *  Created on: Jan 07, 2015
 *      Author: Dimas Melo Filho
 */

#ifndef LANGUAGEEXCEPTION_H_
#define LANGUAGEEXCEPTION_H_

#include <exception>
#include <string>

namespace pgen {

class LanguageException : public std::exception {
private:
	std::string error;
public:
	LanguageException(std::string& error);
	LanguageException(std::string error);
	LanguageException(const char* error);
	virtual ~LanguageException() throw ();
	virtual const char * what() const throw ();
};

} /* namespace pagen */

#endif /* LANGUAGEEXCEPTION_H_ */
