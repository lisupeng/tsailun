/*
 * This file is part of the Tsailun project 
 * 
 * Copyright (c) 2021-2022 Li Supeng
 * 
 * Tsailun is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License.
 * 
 * Tsailun is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with Tsailun.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "groupdbmgr.h"

#include "configmgr.h"

#include <QSqlQuery>
#include <QVariant>
#include <QJsonObject>
#include <QSqlRecord>

#include <QThread>

GroupDbMgr::GroupDbMgr()
{
	m_mutex = new QMutex(QMutex::Recursive);
}

GroupDbMgr::~GroupDbMgr()
{
	if (m_mutex)
		delete m_mutex;
}


bool GroupDbMgr::init()
{
	QMutexLocker autolock(m_mutex);
	QSqlDatabase  database;
	if (!__getDbConnections(database))
	{
		return false;
	}

	if (!init_group_table())
	{
		return false;
	}

	return true;
}

bool GroupDbMgr::init_group_table()
{

	// create table if not exists
	QSqlDatabase  database;
	if (!__getDbConnections(database))
	{
		return false;
	}
	QSqlQuery sql_query(database);

	QString sql = "CREATE TABLE IF NOT EXISTS group_table (groupname varchar(64) primary key, "
		"usercount varchar(8), comment varchar(1024))";

	sql_query.prepare(sql);

	if (!sql_query.exec())
	{
		return false;
	}

	return true;
}

bool GroupDbMgr::addGroup(const QString &groupName, const QString &comment)
{
	QMutexLocker autolock(m_mutex);

	QString userListTableName = groupName + "_userlisttable";
	QSqlDatabase  database;
	if (!__getDbConnections(database))
	{
		return false;
	}

	QSqlQuery sql_query(database);

	// create userlisttable
	QString sql = "CREATE TABLE IF NOT EXISTS " + userListTableName + " (useraccount varchar(64) primary key)";
	sql_query.prepare(sql);
	if (!sql_query.exec())
	{
		return false;
	}

	// add record to group_table
	sql = "INSERT INTO group_table (groupname, comment) VALUES('" + groupName + "', '" + comment + "')";
	sql_query.prepare(sql);
	if (!sql_query.exec())
	{
		return false;
	}

	return true;
}

bool GroupDbMgr::delGroup(const QString &groupName)
{
	QMutexLocker autolock(m_mutex);

	QString userListTableName = groupName + "_userlisttable";
	QSqlDatabase  database;
	if (!__getDbConnections(database))
	{
		return false;
	}

	QSqlQuery sql_query(database);

	// delete user list table
	QString sql = "DROP TABLE if exists " + userListTableName;
	sql_query.prepare(sql);

	if (!sql_query.exec())
	{
		return false;
	}

	// delete record
	sql = "DELETE FROM group_table WHERE groupname='" + groupName + "'";

	sql_query.prepare(sql);

	if (!sql_query.exec())
	{
		return false;
	}

	return true;
}

bool GroupDbMgr::addUserToGroup(const QString &groupName, const QString &useraccount)
{
	QMutexLocker autolock(m_mutex);

	QString userListTableName = groupName + "_userlisttable";
	QSqlDatabase  database;
	if (!__getDbConnections(database))
	{
		return false;
	}

	QSqlQuery sql_query(database);

	// add record to group_table
	QString sql = "INSERT INTO "+ userListTableName +" (useraccount) VALUES('" + useraccount + "')";
	sql_query.prepare(sql);
	if (!sql_query.exec())
	{
		return false;
	}

	return true;
}

bool GroupDbMgr::getGroupList(QJsonArray &list)
{
	QJsonObject jsonobj;
	QMutexLocker autolock(m_mutex);
	QSqlDatabase  database;
	if (!__getDbConnections(database))
	{
		return false;
	}

	// create table if not exists
	QSqlQuery sql_query(database);
	QString sql;

	sql = "SELECT groupname, usercount, comment FROM group_table";

	sql_query.prepare(sql);

	if (!sql_query.exec())
	{
		return false;
	}

	 
	QSqlRecord rec = sql_query.record();

	 
	while (sql_query.next())
	{
		QJsonObject jsonobj;
		recordToJson(rec, sql_query, jsonobj);
		list.append(jsonobj);
	}

	return true;
}

void GroupDbMgr::recordToJson(QSqlRecord &rec, QSqlQuery &sql_query, QJsonObject &jsonobj)
{
	int count = rec.count();

	for (int col = 0; col < count; col++)
	{
		jsonobj.insert(rec.fieldName(col), sql_query.value(col).toString());
	}
}

bool GroupDbMgr::__getDbConnections(QSqlDatabase &database)
{
	Qt::HANDLE threadId = QThread::currentThreadId();

	QSqlDatabase db;

	if (m_dbConnections.contains(threadId))
	{
		db = m_dbConnections[threadId];
	}

	if (db.isValid() && db.isOpen())
	{
		database = db;
		return true;
	}

	// create one
	ConfigMgr *cfgMgr = ConfigMgr::GetInstance();
	QString appRootPath = cfgMgr->getAppRootDir();
	QString userDbFile = appRootPath + "/data/db/groups.db";

	char szName[64];
	sprintf(szName, "conn_%p", threadId);
	QString connName = szName;
	db = QSqlDatabase::addDatabase("QSQLITE", connName);

	db.setDatabaseName(userDbFile);

	if (!db.open())
	{
		return false;
	}

	m_dbConnections[threadId] = db;
	database = db;

	return true;
}

bool GroupDbMgr::getGroupInfo(const QString &groupName, QJsonObject &groupinfo)
{
	QJsonObject jsonobj;
	QMutexLocker autolock(m_mutex);
	QSqlDatabase  database;
	if (!__getDbConnections(database))
	{
		return false;
	}

	// create table if not exists
	QSqlQuery sql_query(database);
	QString sql;

	sql = "SELECT groupname, usercount, comment FROM group_table WHERE groupname='"+groupName+"'";

	sql_query.prepare(sql);

	if (!sql_query.exec())
	{
		return false;
	}

	 
	QSqlRecord rec = sql_query.record();

	 
	if (!sql_query.next())
	{
		return false;
	}

	recordToJson(rec, sql_query, groupinfo);

	// user list
	QJsonArray userlist;
	if (getUserlist(groupName, userlist))
	{
		groupinfo.insert("userlist", userlist);
	}

	return true;
}


bool GroupDbMgr::updateGroup(const QString &groupName, const QString &comment, const QJsonArray &userlist)
{
	QMutexLocker autolock(m_mutex);

	QString userListTableName = groupName + "_userlisttable";
	QSqlDatabase  database;
	if (!__getDbConnections(database))
	{
		return false;
	}

	QSqlQuery sql_query(database);


	// update comment
	QString sql = "UPDATE group_table SET comment='" + comment + "' WHERE groupname='" + groupName + "'";
	sql_query.prepare(sql);
	if (!sql_query.exec())
	{
		return false;
	}

	// clear user list table
	sql = "DELETE FROM " + userListTableName;
	sql_query.prepare(sql);
	if (!sql_query.exec())
	{
		return false;
	}

	// update user list
	for (int i = 0; i < userlist.size(); i++)
	{
		QString account = userlist.at(i).toString();

		// insert to DB
		QString sql = "INSERT INTO " + userListTableName + " (useraccount) VALUES('" + account + "')";
		sql_query.prepare(sql);
		if (!sql_query.exec())
		{
			return false;
		}
	}

	return true;
}

bool GroupDbMgr::getUserlist(const QString &groupName, QJsonArray &list)
{
	QJsonObject jsonobj;
	QMutexLocker autolock(m_mutex);
	QSqlDatabase  database;
	if (!__getDbConnections(database))
	{
		return false;
	}

	// create table if not exists
	QSqlQuery sql_query(database);
	QString sql;

	QString userListTableName = groupName + "_userlisttable";

	sql = "SELECT useraccount FROM " + userListTableName;

	sql_query.prepare(sql);

	if (!sql_query.exec())
	{
		return false;
	}

	 
	QSqlRecord rec = sql_query.record();

	 
	while (sql_query.next())
	{
		int useraccount_col = 0;
		list.append(sql_query.value(useraccount_col).toString());
	}

	return true;
}

bool GroupDbMgr::isUserInGroup(const QString &groupName, const QString &account)
{
	QJsonObject jsonobj;
	QMutexLocker autolock(m_mutex);
	QSqlDatabase  database;
	if (!__getDbConnections(database))
	{
		return false;
	}

	// create table if not exists
	QSqlQuery sql_query(database);
	QString sql;

	QString userListTableName = groupName + "_userlisttable";

	sql = "SELECT useraccount FROM " + userListTableName + "WHERE useraccount = '" + account + "'";

	sql_query.prepare(sql);

	if (!sql_query.exec())
	{
		return false;
	}

	 
	QSqlRecord rec = sql_query.record();

	 
	if (sql_query.next())
		return true;
	else
		return false;
}