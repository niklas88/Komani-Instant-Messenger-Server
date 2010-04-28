/**
 **	Query.cpp
 **
 **	Published / author: 2001-02-15 / grymse@alhem.net
 **/

/*
Copyright (C) 2001  Anders Hedstrom

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
#ifdef _WIN32
#pragma warning(disable:4786)
#endif

#include <string>
#include <map>
#ifdef WIN32
#include <config-win.h>
#include <mysql.h>
#else
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mysql/mysql.h>
#endif

#include "../include/Database.h"
#include "../include/Query.h"


Query::Query(Database *dbin)
:m_db(*dbin)
,odb(dbin ? dbin -> grabdb() : NULL)
,res(NULL)
,row(NULL)
{
}


Query::Query(Database& dbin) : m_db(dbin),odb(dbin.grabdb()),res(NULL),row(NULL)
{
}


Query::Query(Database *dbin,const std::string& sql) : m_db(*dbin)
,odb(dbin ? dbin -> grabdb() : NULL),res(NULL),row(NULL)
{
	execute(sql);
}


Query::Query(Database& dbin,const std::string& sql) : m_db(dbin),odb(dbin.grabdb()),res(NULL),row(NULL)
{
	execute(sql); // returns 0 if fail
}


Query::~Query()
{
	if (res)
	{
		GetDatabase().error(*this, "mysql_free_result in destructor");
		mysql_free_result(res);
	}
	if (odb)
	{
		m_db.freedb(odb);
	}
}


Database& Query::GetDatabase() const
{
	return m_db;
}


bool Query::execute(const std::string& sql)
{		// query, no result
	m_last_query = sql;
	if (odb && res)
	{
		GetDatabase().error(*this, "execute: query busy");
	}
	if (odb && !res)
	{
		if (mysql_query(&odb -> mysql,sql.c_str()))
		{
			GetDatabase().error(*this,"query failed");
		}
		else
		{
			return true;
		}
	}
	return false;
}



// methods using db specific api calls

MYSQL_RES *Query::get_result(const std::string& sql)
{	// query, result
	if (odb && res)
	{
		GetDatabase().error(*this, "get_result: query busy");
	}
	if (odb && !res)
	{
		if (execute(sql))
		{
			res = mysql_store_result(&odb -> mysql);
		}
	}
	return res;
}


void Query::free_result()
{
	if (odb && res)
	{
		mysql_free_result(res);
		res = NULL;
		row = NULL;
	}
}


MYSQL_ROW Query::fetch_row()
{
	rowcount = 0;
	return odb && res ? row = mysql_fetch_row(res) : NULL;
}


my_ulonglong Query::insert_id()
{
	if (odb)
	{
		return mysql_insert_id(&odb -> mysql);
	}
	else
	{
		return 0;
	}
}


long Query::num_rows()
{
	return odb && res ? mysql_num_rows(res) : 0;
}


// data retreival methods

bool Query::is_null(int x)
{
	if (odb && res && row)
	{
		return row[x] ? false : true;
	}
	return false; // ...
}


const char *Query::getstr(int x)
{
	if (odb && res && row)
	{
		return row[x] ? row[x] : "";
	}
	else
	{
		return NULL;
	}
}


const char *Query::getstr()
{
	return getstr(rowcount++);
}


double Query::getnum(int x)
{
	return odb && res && row && row[x] ? atof(row[x]) : 0;
}


long Query::getval(int x)
{
	return odb && res && row && row[x] ? atol(row[x]) : 0;
}


double Query::getnum()
{
	return getnum(rowcount++);
}


long Query::getval()
{
	return getval(rowcount++);
}


unsigned long Query::getuval(int x)
{
	unsigned long l = 0;
	if (odb && res && row && row[x])
	{
		l = a2ubigint(row[x]);
	}
	return l;
}


unsigned long Query::getuval()
{
	return getuval(rowcount++);
}


int64_t Query::getbigint(int x)
{
	return odb && res && row && row[x] ? a2bigint(row[x]) : 0;
}


int64_t Query::getbigint()
{
	return getbigint(rowcount++);
}


uint64_t Query::getubigint(int x)
{
	return odb && res && row && row[x] ? a2ubigint(row[x]) : 0;
}


uint64_t Query::getubigint()
{
	return getubigint(rowcount++);
}


double Query::get_num(const std::string& sql)
{
	double l = 0;
	if (get_result(sql))
	{
		if (fetch_row())
		{
			l = getnum();
		}
		free_result();
	}
	return l;
}


long Query::get_count(const std::string& sql)
{
	long l = 0;
	if (get_result(sql))
	{
		if (fetch_row())
			l = getval();
		free_result();
	}
	return l;
}


const char *Query::get_string(const std::string& sql)
{
	bool found = false;
	m_tmpstr = "";
	if (get_result(sql))
	{
		if (fetch_row())
		{
			m_tmpstr = getstr();
			found = true;
		}
		free_result();
	}
	return m_tmpstr.c_str(); // %! changed from 1.0 which didn't return NULL on failed query
}


//MYSQL_FIELD *	STDCALL mysql_fetch_field(MYSQL_RES *result);
/*
MYSQL_FIELD *Query::fetch_field()
{
	return odb && res ? mysql_fetch_field(res) : NULL;
}


const char *Query::fetch_fieldname()
{
	MYSQL_FIELD *field = odb && res ? mysql_fetch_field(res) : NULL;
	return field ? field -> name : "";
}
*/


std::string Query::safestr(const std::string& str)
{
	std::string str2;
	for (size_t i = 0; i < str.size(); i++)
	{
		switch (str[i])
		{
		case 0:
			str2+="";
			break;
		case '\'':
		case '\\':
		case 34:
			str2 += '\\';
		default:
			str2 += str[i];
		}
	}
	return str2;
}


std::string Query::unsafestr(const std::string& str)
{
	std::string str2;
	for (size_t i = 0; i < str.size(); i++)
	{
		if (str[i] == '\\')
		{
			i++;
		}
		if (i < str.size())
		{
			str2 += str[i];
		}
	}
	return str2;
}


std::string Query::xmlsafestr(const std::string& str)
{
	std::string str2;
	for (size_t i = 0; i < str.size(); i++)
	{
		switch (str[i])
		{
		case '&':
			str2 += "&amp;";
			break;
		case '<':
			str2 += "&lt;";
			break;
		case '>':
			str2 += "&gt;";
			break;
		case '"':
			str2 += "&quot;";
			break;
		case '\'':
			str2 += "&apos;";
			break;
		default:
			str2 += str[i];
		}
	}
	return str2;
}


const std::string& Query::GetLastQuery()
{
	return m_last_query;
}


std::string Query::GetError()
{
	return odb ? mysql_error(&odb -> mysql) : "";
}


int Query::GetErrno()
{
	return odb ? mysql_errno(&odb -> mysql) : 0;
}


int64_t Query::a2bigint(const std::string& str)
{
	int64_t val = 0;
	bool sign = false;
	size_t i = 0;
	if (str[i] == '-')
	{
		sign = true;
		i++;
	}
	for (; i < str.size(); i++)
	{
		val = val * 10 + (str[i] - 48);
	}
	return sign ? -val : val;
}


uint64_t Query::a2ubigint(const std::string& str)
{
	uint64_t val = 0;
	for (size_t i = 0; i < str.size(); i++)
	{
		val = val * 10 + (str[i] - 48);
	}
	return val;
}


bool Query::Connected()
{
	if (odb)
	{
		if (mysql_ping(&odb -> mysql))
		{
			GetDatabase().error(*this, "mysql_ping() failed");
			return false;
		}
	}
	return odb ? true : false;
}
