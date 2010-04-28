/**
 **	Database.cpp
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
#include <stdio.h>
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
#include <stdarg.h>
#endif

#include "../include/IError.h"
#include "../include/Database.h"


Database::Database(const std::string& d,IError *e)
:database(d)
,m_errhandler(e)
,m_embedded(true)
,m_mutex(m_mutex)
,m_b_use_mutex(false)
{
}


Database::Database(Mutex& m,const std::string& d,IError *e)
:database(d)
,m_errhandler(e)
,m_embedded(true)
,m_mutex(m)
,m_b_use_mutex(true)
{
}


Database::Database(const std::string& h,const std::string& u,const std::string& p,const std::string& d,IError *e)
:host(h)
,user(u)
,password(p)
,database(d)
,m_errhandler(e)
,m_embedded(false)
,m_mutex(m_mutex)
,m_b_use_mutex(false)
{
}


Database::Database(Mutex& m,const std::string& h,const std::string& u,const std::string& p,const std::string& d,IError *e)
:host(h)
,user(u)
,password(p)
,database(d)
,m_errhandler(e)
,m_embedded(false)
,m_mutex(m)
,m_b_use_mutex(true)
{
}


Database::~Database()
{
	for (opendb_v::iterator it = m_opendbs.begin(); it != m_opendbs.end(); it++)
	{
		OPENDB *p = *it;
		mysql_close(&p -> mysql);
	}
	while (m_opendbs.size())
	{
		opendb_v::iterator it = m_opendbs.begin();
		OPENDB *p = *it;
		if (p -> busy)
		{
			error("destroying Database object before Query object");
		}
		delete p;
		m_opendbs.erase(it);
	}
}


void Database::OnMyInit(OPENDB *odb)
{
	// using embedded server (libmysqld)
	if (m_embedded)
	{
		mysql_options(&odb -> mysql, MYSQL_READ_DEFAULT_GROUP, "test_libmysqld_CLIENT");
	}
}


void Database::RegErrHandler(IError *p)
{
	m_errhandler = p;
}


Database::OPENDB *Database::grabdb()
{
	Lock lck(m_mutex, m_b_use_mutex);
	OPENDB *odb = NULL;

	for (opendb_v::iterator it = m_opendbs.begin(); it != m_opendbs.end(); it++)
	{
		odb = *it;
		if (!odb -> busy)
		{
			break;
		}
		else
		{
			odb = NULL;
		}
	}
	if (!odb)
	{
		odb = new OPENDB;
		if (!odb)
		{
			error("grabdb: OPENDB struct couldn't be created");
			return NULL;
		}
		if (!mysql_init(&odb -> mysql))
		{
			error("mysql_init() failed - list size %d",m_opendbs.size());
			delete odb;
			return NULL;
		}
		// use callback to set mysql_options() before connect, etc
		this -> OnMyInit(odb);
		if (m_embedded)
		{
			if (!mysql_real_connect(&odb -> mysql,NULL,NULL,NULL,database.c_str(),0,NULL,0) )
			{
				error("mysql_real_connect(NULL,NULL,NULL,%s,0,NULL,0) failed - list size %d",database.c_str(),m_opendbs.size());
				delete odb;
				return NULL;
			}
		}
		else
		{
			if (!mysql_real_connect(&odb -> mysql,host.c_str(),user.c_str(),password.c_str(),database.c_str(),0,NULL,0) )
			{
				error("mysql_real_connect(%s,%s,***,%s,0,NULL,0) failed - list size %d",host.c_str(),user.c_str(),database.c_str(),m_opendbs.size());
				delete odb;
				return NULL;
			}
		}
		odb -> busy = true;
		m_opendbs.push_back(odb);
	}
	else
	{
		if (mysql_ping(&odb -> mysql))
		{
			error("mysql_ping() failed when reusing an old connection from the connection pool");
		}
		odb -> busy = true;
	}
	return odb;
}


void Database::freedb(Database::OPENDB *odb)
{
	Lock lck(m_mutex, m_b_use_mutex);
	if (odb)
	{
		odb -> busy = false;
	}
}


void Database::error(const char *format, ...)
{
	if (m_errhandler)
	{
		va_list ap;
		char errstr[5000];
		va_start(ap, format);
#ifdef WIN32
		vsprintf(errstr, format, ap);
#else
		vsnprintf(errstr, 5000, format, ap);
#endif
		va_end(ap);
		m_errhandler -> error(*this, errstr);
	}
}


void Database::error(Query& q,const char *format, ...)
{
	if (m_errhandler)
	{
		va_list ap;
		char errstr[5000];
		va_start(ap, format);
#ifdef WIN32
		vsprintf(errstr, format, ap);
#else
		vsnprintf(errstr, 5000, format, ap);
#endif
		va_end(ap);
		m_errhandler -> error(*this, q, errstr);
	}
}


bool Database::Connected()
{
	OPENDB *odb = grabdb();
	if (!odb)
	{
		return false;
	}
	int ping_result = mysql_ping(&odb -> mysql);
	if (ping_result)
	{
		error("mysql_ping() failed");
	}
	freedb(odb);
	return ping_result ? false : true;
}
