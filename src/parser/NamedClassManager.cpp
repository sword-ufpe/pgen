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
 * \date 2015-02-07
 * \file
 * This file contains the NamedClassManager class implementation. This class manages all named regex classes defined on 
 * the YAML language file.
 */
 
// STL
#include <string>
#include <iostream>
// pgen
#include "../expr/CharClass.h"
#include "../misc/LanguageException.h"
#include "NamedClassManager.h"

using namespace std;
namespace pgen
{
	NamedClassManager::NamedClassManager()
	{
		add("alnum","[a-zA-Z0-9]");
		add("alpha","[a-zA-Z]");
		add("ascii","[\\x00-\\x7F]");
		add("blank","[ \\t]");
		add("cntrl","[\\x00-\\x1F\\x7F]");
		add("digit","[0-9]");
		add("graph","[\\x21-\\x7E]");
		add("lower","[a-z]");
		add("print","[\\x20-\\x7E]");
		add("punct","[!\"#$%&'()*+,\\-./\\:;<=>?@\\[\\]\\\\^_`{|}~]");
		add("space","[ \t\r\n\v\f]");
		add("upper","[A-Z]");
		add("word","[A-Za-z0-9_]");
		add("xdigit","[A-Fa-f0-9]");
	}
	
	NamedClassManager::~NamedClassManager()
	{		
		for (auto it = namedClasses.begin(); it != namedClasses.end();)
		{
			if (it->second != nullptr)
			{
				delete it->second;
			}
			it = namedClasses.erase(it);
		}
	}
	
	void NamedClassManager::add(string name, string regex)
	{
		unsigned int pos = 0;
		auto it = namedClasses.find(name);
		if (it != namedClasses.end())
		{
			delete it->second;
			it->second = CharClass::parse(regex, pos, *this);
		}
		else 
		{
			namedClasses[name] = CharClass::parse(regex, pos, *this);
		}
		
	}
	
	CharClass& NamedClassManager::get(string name)
	{
		auto it = namedClasses.find(name);
		if (it == namedClasses.end())
		{
			throw LanguageException("Error while trying to retrieve the named class '" + name + "'.");
		}
		return *((CharClass*)it->second);
	}
	
}