/**
 **	IError.h
 **
 **	Published / author: 2004-06-11 / grymse@alhem.net
 **/

/*
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

#ifndef _IERROR_H
#define _IERROR_H

#include <string>

class Database;
class Query;

class IError
{
public:
	virtual void error(Database&,const std::string&) = 0;
	virtual void error(Database&,Query&,const std::string&) = 0;
   virtual ~IError(){}
};


#endif // _IERROR_H
/**
 **	StderrLog.h
 **
 **	Published / author: 2004-08-18 / grymse@alhem.net
 **/

/*
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
#ifndef _STDERRLOG_H
#define _STDERRLOG_H


class StderrLog : public IError
{
public:
	void error(Database&,const std::string&);
	void error(Database&,Query&,const std::string&);
    virtual ~StderrLog(){} 

};




#endif // _STDERRLOG_H
/**
 **	SysLog.h
 **
 **	Published / author: 2004-08-18 / grymse@alhem.net
 **/

/*
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
#ifndef _SYSLOG_H
#define _SYSLOG_H
#ifndef WIN32

#include <syslog.h>


class SysLog : public IError
{
public:
	SysLog(const std::string& = "database", int = LOG_PID, int = LOG_USER);
	virtual ~SysLog();

	void error(Database&,const std::string&);
	void error(Database&,Query&,const std::string&);

};



#endif // WIN32
#endif // _SYSLOG_H
/*
enum_t.h

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
#ifndef _ENUM_T_H
#define _ENUM_T_H

#include <string>
#include <map>
#ifdef WIN32
#include <config-win.h>
#include <mysql.h>
typedef unsigned __int64 uint64_t;
#define strcasecmp stricmp
#else
#include <stdint.h>
#endif

class enum_t
{
public:
  enum_t(std::map<std::string, uint64_t>& );

  const std::string& String();
  unsigned short Value();
  const char *c_str();

  void operator=(const std::string& );
  void operator=(unsigned short);
  bool operator==(const std::string& );
  bool operator==(unsigned short);
  bool operator!=(const std::string& );

private:
  std::map<std::string, uint64_t>& m_mmap;
  std::map<unsigned short, std::string> m_vmap;
  unsigned short m_value;

};


#endif // _ENUM_T_H
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
/**
 **	Database.h
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

#ifndef _DATABASE_H
#define _DATABASE_H

#include <string>
#include <list>

class IError;
class Query;


class Database 
{
public:
	struct OPENDB {
		OPENDB() : busy(false) {}
		MYSQL mysql;
		bool busy;
	};
	typedef std::list<OPENDB *> opendb_v;

public:
  /** Use embedded libmysqld */
  Database(const std::string& database,
    IError * = NULL);

  /** Connect to a MySQL server*/
	Database(const std::string& host,
		const std::string& user,
		const std::string& password = "",
		const std::string& database = "",
		IError * = NULL);

  virtual ~Database();

  /** Callback after mysql_init */
  virtual void OnMyInit(OPENDB *);

  /** try to establish connection with given host */
	bool Connected();

	void RegErrHandler(IError *);
	void error(Query&,const char *format, ...);

	/** Request a database connection.
The "grabdb" method is used by the Query class, so that each object instance of Query gets a unique
database connection. I will reimplement your connection check logic in the Query class, as that's where
the database connection is really used.
It should be used something like this.
{
    Query q(db);
    if (!q.Connected())
       return false;
    q.execute("delete * from user"); // well maybe not
}

When the Query object is deleted, then "freedb" is called - the database connection stays open in the
m_opendbs vector. New Query objects can then reuse old connections.
	*/
	OPENDB *grabdb();
	void freedb(OPENDB *odb);

private:
	void error(const char *format, ...);
	//
	std::string host;
	std::string user;
	std::string password;
	std::string database;
	opendb_v m_opendbs;
	IError *m_errhandler;
  bool m_embedded;
};


#endif // _DATABASE_H
#ifdef _WIN32
#pragma warning(disable:4786)
#endif
/**
 **	Query.h
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

#ifndef _QUERY_H
#define _QUERY_H

#include <string>
#include <map>
#ifdef WIN32
typedef unsigned __int64 uint64_t;
typedef __int64 int64_t;
#else
#include <stdint.h>
#endif


class Query 
{
public:
	Query(Database& dbin);
	Query(Database& dbin,const std::string& sql);
	~Query();

	bool Connected();
	Database& GetDatabase();
	const std::string& GetLastQuery();

	// execute() returns true if query is successful
	bool execute(const std::string& sql);

// methods using db specific api calls

	MYSQL_RES *get_result(const std::string& sql);
	void free_result();
	MYSQL_ROW fetch_row();
	my_ulonglong insert_id();
	long num_rows();
	std::string GetError();
	int GetErrno();

// data retreival methods
	bool is_null(int x);

	// string
	const char *get_string(const std::string& sql);
	const char *getstr(int x);
	const char *getstr();
	// integer
	long get_count(const std::string& sql);
	long getval(int x);
	long getval();
	unsigned long getuval(int x);
	unsigned long getuval();
	int64_t getbigint(int x);
	int64_t getbigint();
	uint64_t getubigint(int x);
	uint64_t getubigint();
	// float
	double get_num(const std::string& sql);
	double getnum(int x);
	double getnum();

//MYSQL_FIELD *	STDCALL mysql_fetch_field(MYSQL_RES *result);
/*
	MYSQL_FIELD *fetch_field();
	const char *fetch_fieldname();
*/
	std::string safestr(const std::string& );
	std::string unsafestr(const std::string& );
	std::string xmlsafestr(const std::string& );

	int64_t a2bigint(const std::string& );
	uint64_t a2ubigint(const std::string& );
	//

protected:
	Query(Database *dbin);
	Query(Database *dbin,const std::string& sql);
private:
	Database& m_db;
	Database::OPENDB *odb;
	MYSQL_RES *res;
	MYSQL_ROW row;
	short rowcount;
	std::string m_tmpstr;
	std::string m_last_query;
};


#endif // _QUERY_H
