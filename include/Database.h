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
#include "../include/Mutex.h"


class IError;
class Query;


/** Connection information and pooling. */
class Database 
{
private:
	/** Mutex helper class. */
	class Lock {
	public:
		Lock(Mutex& mutex,bool use) : m_mutex(mutex),m_b_use(use) {
			if (m_b_use) {
				m_mutex.Lock();
			}
		}
		~Lock() {
			if (m_b_use) {
				m_mutex.Unlock();
			}
		}
	private:
		Mutex& m_mutex;
		bool m_b_use;
	};
public:
	/** Connection pool struct. */
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

	/** Use embedded libmysqld + thread safe */
	Database(Mutex& ,const std::string& database,
		IError * = NULL);

	/** Connect to a MySQL server */
	Database(const std::string& host,
		const std::string& user,
		const std::string& password = "",
		const std::string& database = "",
		IError * = NULL);

	/** Connect to a MySQL server + thread safe */
	Database(Mutex& ,const std::string& host,
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
	Database(const Database& ) : m_mutex(m_mutex) {}
	Database& operator=(const Database& ) { return *this; }
	void error(const char *format, ...);
	//
	std::string host;
	std::string user;
	std::string password;
	std::string database;
	opendb_v m_opendbs;
	IError *m_errhandler;
	bool m_embedded;
	Mutex& m_mutex;
	bool m_b_use_mutex;
};


#endif // _DATABASE_H
