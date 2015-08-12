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
 * This file contains the NamedClassManager class definition. This class manages all named regex classes defined on the 
 * YAML language file.
 */
 
namespace pgen
{
	class NamedClassManager;
}
 
#ifndef NAMEDCLASSMANAGER_H_
#define NAMEDCLASSMANAGER_H_

// STL
#include <string>
#include <unordered_map>
// pgen
#include "../expr/CharClass.h"

using namespace std;
namespace pgen
{
	/**
	 * Handles all named regex classes. It provides a method for adding a new class directly from regex.
	 */
	class NamedClassManager
	{
	private:
		unordered_map<string, CharClass*> namedClasses;
	public:
		/**
		 * Constructor
		 */
		NamedClassManager();
		
		/**
		 * Destructor
		 */
		~NamedClassManager();
		
		/**
		 * Adds a new CharClass to the Manager.
		 * @param name the name of the class
		 * @param regex the regex of the class.
		 */
		void add(string name, string regex);
		
		/**
		 * Returns a const CharClass reference to a previously added charclass.
		 * @param name the name of the class to read
		 * @return a const reference to the CharClass object.
		 */
		CharClass& get(string name);
	};
}
 
#endif