/*
set_t.h

Copyright (C) 2004  Anders Hedstrom

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/
#ifndef _SET_T_H
#define _SET_T_H

#include <string>
#include <map>
#ifdef WIN32
#include <config-win.h>
#include <mysql.h>
typedef unsigned __int64 uint64_t;
#else
#include <stdint.h>
#endif

/** Implements MySQL SET datatype. */
class set_t
{
public:
	set_t(std::map<std::string, uint64_t>& );

	const std::string& String();
	uint64_t Value();
	const char *c_str();

	bool in_set(const std::string& );

	void operator=(const std::string& );
	void operator=(uint64_t);
	void operator|=(uint64_t);
	void operator&=(uint64_t);
	void operator+=(const std::string& );
	void operator-=(const std::string& );

private:
	std::map<std::string, uint64_t>& m_mmap;
	uint64_t m_value;
	std::string m_strvalue;

};


#endif // _SET_T_H
