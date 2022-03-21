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

#include "spacedbmgr.h"
#include "configmgr.h"

#include <QSqlQuery>
#include <QVariant>
#include <QJsonObject>
#include <QSqlRecord>
#include <QThread>
#include <QDir>

static QString _getRTableName(const QString &spaceName)
{
	QChar c = spaceName[0];

	if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z'))
		return spaceName + "_rtable";
	else
		return "u" + spaceName + "_rtable";
}

static QString _getWTableName(const QString &spaceName)
{
	QChar c = spaceName[0];

	if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z'))
		return spaceName + "_wtable";
	else
		return "u" + spaceName + "_wtable";
}

SpaceDbMgr::SpaceDbMgr()
{
	m_mutex = new QMutex(QMutex::Recursive);
}

SpaceDbMgr::~SpaceDbMgr()
{
	if (m_mutex)
		delete m_mutex;
}


bool SpaceDbMgr::init()
{
	QMutexLocker autolock(m_mutex);

	if (!init_space_table())
	{
		return false;
	}

	return true;
}

bool SpaceDbMgr::init_space_table()
{

	// create table if not exists
	QSqlDatabase  database;
	if (!__getDbConnections(database))
	{
		return false;
	}
	QSqlQuery sql_query(database);

	QString sql = "CREATE TABLE IF NOT EXISTS space_table (spacename varchar(64) primary key, "
		"path varchar(2048), rcount varchar(8), wcount varchar(8), extrainfo varchar(2048))";

	sql_query.prepare(sql);

	if (!sql_query.exec())
	{
		database.close();
		return false;
	}

	int count;
	if (!_getSpaceCount(count, database))
	{
		database.close();
		return false;
	}

	 
	if (count == 0)
	{
		if (!insertDefaultSpace())
		{
			database.close();
			return false;
		}
	}

	database.close();

	return true;
}

bool SpaceDbMgr::insertDefaultSpace()
{
	if (!addSpace("space_default", "/spaces/default"))
		return false;

	if (!addToSpaceWlist("space_default", "user", "*"))
		return false;

	if (!addToSpaceRlist("space_default", "user", "*"))
		return false;

	if (!addToSpaceRlist("space_default", "user", "admin"))
		return false;

	if (!addToSpaceWlist("space_default", "user", "admin"))
		return false;

	if (!addToSpaceRlist("space_default", "user", "guest"))
		return false;

	if (!addToSpaceWlist("space_default", "user", "guest"))
		return false;

	return true;
}

bool SpaceDbMgr::_getSpaceCount(int &count, QSqlDatabase &database)
{
	// do query
	QSqlQuery sql_query(database);
	QString sql = "SELECT count(*) FROM space_table";
	sql_query.prepare(sql);
	if (!sql_query.exec())
	{
		return false;
	}

	sql_query.next();

	count = sql_query.value(0).toInt();

	return true;
}

bool SpaceDbMgr::addSpace(const QString &spaceName, const QString &path)
{
	QMutexLocker autolock(m_mutex);

	QString rtableName = _getRTableName(spaceName);
	QString wtableName = _getWTableName(spaceName);
	QSqlDatabase  database;
	if (!__getDbConnections(database))
	{
		return false;
	}

	QSqlQuery sql_query(database);

	// create rtable
	QString sql = "CREATE TABLE IF NOT EXISTS " + rtableName + " (itemtype varchar(16), item varchar(128) primary key)";
	sql_query.prepare(sql);
	if (!sql_query.exec())
	{
		database.close();
		return false;
	}

	// create wtable
	sql = "CREATE TABLE IF NOT EXISTS " + wtableName + " (itemtype varchar(16), item varchar(128) primary key)";
	sql_query.prepare(sql);
	if (!sql_query.exec())
	{
		database.close();
		return false;
	}

	// add record to space_table
	sql = "INSERT INTO space_table (spacename, path, rcount, wcount) VALUES('" + spaceName + "', '" + path + "', '0', '0')";
	sql_query.prepare(sql);
	if (!sql_query.exec())
	{
		database.close();
		return false;
	}

	// create space path
	ConfigMgr *cfgMgr = ConfigMgr::GetInstance();
	QString appRootPath = cfgMgr->getAppRootDir();
	QString spacePath = appRootPath + "/data" + path + "/pages";
	QDir dir(spacePath);
	if (!dir.mkpath(spacePath))
	{
		database.close();
		return false;
	}

	database.close();

	return true;
}

bool SpaceDbMgr::deleteSpace(const QString &spaceName)
{
	QMutexLocker autolock(m_mutex);

	QString rtableName = _getRTableName(spaceName);
	QString wtableName = _getWTableName(spaceName);
	QSqlDatabase  database;
	if (!__getDbConnections(database))
	{
		return false;
	}

	QSqlQuery sql_query(database);

	// del rtable/wtable
	QString sql = "DROP TABLE if exists " + rtableName;
	sql_query.prepare(sql);

	if (!sql_query.exec())
	{
		database.close();
		return false;
	}

	sql = "DROP TABLE if exists " + wtableName;
	sql_query.prepare(sql);

	if (!sql_query.exec())
	{
		database.close();
		return false;
	}

	// delete record
	sql = "DELETE FROM space_table WHERE spacename='" + spaceName + "'";

	sql_query.prepare(sql);

	if (!sql_query.exec())
	{
		database.close();
		return false;
	}

	database.close();

	return true;
}

bool SpaceDbMgr::addToSpaceRlist(const QString &spaceName, const QString &type, const QString &item)
{
	QMutexLocker autolock(m_mutex);

	QString rtableName = _getRTableName(spaceName);
	QSqlDatabase  database;
	if (!__getDbConnections(database))
	{
		return false;
	}

	QSqlQuery sql_query(database);
	QString sql = "INSERT INTO "+ rtableName +" (itemtype, item) VALUES('" + type + "', '" + item + "')";
	sql_query.prepare(sql);
	if (!sql_query.exec())
	{
		database.close();
		return false;
	}

	// update count
	int count = 0;
	if (!_getSizeOfSpaceRlist(spaceName, count, database))
	{
		database.close();
		return false;
	}
	QString rcount = QString("%1").arg(count);
	sql = "UPDATE space_table SET rcount='" + rcount + "' WHERE spacename='" + spaceName + "'";
	sql_query.prepare(sql);
	if (!sql_query.exec())
	{
		database.close();
		return false;
	}

	database.close();

	return true;
}

bool SpaceDbMgr::addToSpaceWlist(const QString &spaceName, const QString &type, const QString &item)
{
	QMutexLocker autolock(m_mutex);

	QString wtableName = _getWTableName(spaceName);
	QSqlDatabase  database;
	if (!__getDbConnections(database))
	{
		return false;
	}

	QSqlQuery sql_query(database);
	QString sql = "INSERT INTO " + wtableName + " (itemtype, item) VALUES('" + type + "', '" + item + "')";
	sql_query.prepare(sql);
	if (!sql_query.exec())
	{
		database.close();
		return false;
	}

	// update count
	int count = 0;
	if (!_getSizeOfSpaceWlist(spaceName, count, database))
	{
		database.close();
		return false;
	}
	QString wcount = QString("%1").arg(count);
	sql = "UPDATE space_table SET wcount='" + wcount + "' WHERE spacename='" + spaceName + "'";
	sql_query.prepare(sql);
	if (!sql_query.exec())
	{
		database.close();
		return false;
	}

	database.close();

	return true;
}

bool SpaceDbMgr::_getSizeOfSpaceRlist(const QString &spaceName, int &size, QSqlDatabase  &database)
{
	QString rtableName = _getRTableName(spaceName);

	QSqlQuery sql_query(database);
	QString sql = "SELECT count(*) FROM " + rtableName;
	sql_query.prepare(sql);
	if (!sql_query.exec())
	{
		return false;
	}

	sql_query.next();

	size = sql_query.value(0).toInt();

	return true;
}

bool SpaceDbMgr::_getSizeOfSpaceWlist(const QString &spaceName, int &size, QSqlDatabase  &database)
{
	QMutexLocker autolock(m_mutex);

	QString wtableName = _getWTableName(spaceName);

	QSqlQuery sql_query(database);
	QString sql = "SELECT count(*) FROM " + wtableName;
	sql_query.prepare(sql);
	if (!sql_query.exec())
	{
		return false;
	}

	sql_query.next();

	size = sql_query.value(0).toInt();

	return true;
}

bool SpaceDbMgr::getSpaceList(QJsonArray &list)
{
	QJsonObject jsonobj;
	QMutexLocker autolock(m_mutex);
	QSqlDatabase  database;
	if (!__getDbConnections(database))
	{
		database.close();
		return false;
	}

	// create table if not exists
	QSqlQuery sql_query(database);
	QString sql;

	sql = "SELECT spacename,path,rcount,wcount FROM space_table";

	sql_query.prepare(sql);

	if (!sql_query.exec())
	{
		database.close();
		return false;
	}

	 
	QSqlRecord rec = sql_query.record();

	 
	while (sql_query.next())
	{
		QJsonObject jsonobj;
		recordToJson(rec, sql_query, jsonobj);
		list.append(jsonobj);
	}

	database.close();

	return true;
}

void SpaceDbMgr::recordToJson(QSqlRecord &rec, QSqlQuery &sql_query, QJsonObject &jsonobj)
{
	int count = rec.count();

	for (int col = 0; col < count; col++)
	{
		jsonobj.insert(rec.fieldName(col), sql_query.value(col).toString());
	}
}

bool SpaceDbMgr::__getDbConnections(QSqlDatabase &database)
{
	Qt::HANDLE threadId = QThread::currentThreadId();

	// create one
	ConfigMgr *cfgMgr = ConfigMgr::GetInstance();
	QString appRootPath = cfgMgr->getAppRootDir();
	QString spaceDbFile = appRootPath + "/data/db/spaces.db";

	char szName[64];
	sprintf(szName, "spacedb_conn_%p", threadId);
	QString connName = szName;
	database = QSqlDatabase::addDatabase("QSQLITE", connName);

	database.setDatabaseName(spaceDbFile);

	if (!database.open() || !database.isValid())
	{
		return false;
	}

	return true;
}

bool SpaceDbMgr::getSpaceDetail(const QString &spaceName, QJsonObject &spaceDetail)
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

	sql = "SELECT spacename,path,rcount,wcount FROM space_table WHERE spacename='" + spaceName + "'";

	sql_query.prepare(sql);

	if (!sql_query.exec())
	{
		database.close();
		return false;
	}

	 
	QSqlRecord rec = sql_query.record();

	 
	if (!sql_query.next())
	{
		database.close();
		return false;
	}

	recordToJson(rec, sql_query, spaceDetail);

	// rtable list
	QJsonArray rlist;
	if (_getSpaceRlist(spaceName, rlist, database))
	{
		spaceDetail.insert("rlist", rlist);
	}

	// get wtable list
	QJsonArray wlist;
	if (_getSpaceWlist(spaceName, wlist, database))
	{
		spaceDetail.insert("wlist", wlist);
	}

	database.close();

	return true;
}

bool SpaceDbMgr::_getSpaceRlist(const QString &spaceName, QJsonArray &rlist, QSqlDatabase  &database)
{
	QJsonObject jsonobj;
	QMutexLocker autolock(m_mutex);

	// create table if not exists
	QSqlQuery sql_query(database);
	QString sql;

	QString rtableName = _getRTableName(spaceName);

	sql = "SELECT item,itemtype FROM " + rtableName;

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
		rlist.append(jsonobj);
	}

	return true;
}

bool SpaceDbMgr::_getSpaceWlist(const QString &spaceName, QJsonArray &wlist, QSqlDatabase  &database)
{
	QJsonObject jsonobj;
	QMutexLocker autolock(m_mutex);

	// create table if not exists
	QSqlQuery sql_query(database);
	QString sql;

	QString wtableName = _getWTableName(spaceName);

	sql = "SELECT item,itemtype FROM " + wtableName;

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
		wlist.append(jsonobj);
	}

	return true;
}

bool SpaceDbMgr::getSpaceRlist(const QString &spaceName, QJsonArray &rlist)
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

	QString rtableName = _getRTableName(spaceName);

	sql = "SELECT item,itemtype FROM " + rtableName;

	sql_query.prepare(sql);

	if (!sql_query.exec())
	{
		database.close();
		return false;
	}

	 
	QSqlRecord rec = sql_query.record();

	 
	while (sql_query.next())
	{
		QJsonObject jsonobj;
		recordToJson(rec, sql_query, jsonobj);
		rlist.append(jsonobj);
	}

	database.close();

	return true;
}

bool SpaceDbMgr::getSpaceWlist(const QString &spaceName, QJsonArray &wlist)
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

	QString wtableName = _getWTableName(spaceName);

	sql = "SELECT item,itemtype FROM " + wtableName;

	sql_query.prepare(sql);

	if (!sql_query.exec())
	{
		database.close();
		return false;
	}

	 
	QSqlRecord rec = sql_query.record();

	 
	while (sql_query.next())
	{
		QJsonObject jsonobj;
		recordToJson(rec, sql_query, jsonobj);
		wlist.append(jsonobj);
	}

	database.close();

	return true;
}

bool SpaceDbMgr::updateSpace(const QString &spaceName, const QJsonArray &rlist, const QJsonArray &wlist)
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

	QString rtableName = _getRTableName(spaceName);
	QString wtableName = _getWTableName(spaceName);

	// clear rtable
	sql = "DELETE FROM " + rtableName;
	sql_query.prepare(sql);
	if (!sql_query.exec())
	{
		database.close();
		return false;
	}

	// clear wtable
	sql = "DELETE FROM " + wtableName;
	sql_query.prepare(sql);
	if (!sql_query.exec())
	{
		database.close();
		return false;
	}

	// add to rtable
	for (int i = 0; i < rlist.size(); i++)
	{
		QString item = rlist.at(i).toObject().value("item").toString();
		QString itemtype = rlist.at(i).toObject().value("itemtype").toString();

		// insert to DB
		QString sql = "INSERT INTO " + rtableName + " (itemtype, item) VALUES('" + itemtype + "', '" + item + "')";
		sql_query.prepare(sql);
		if (!sql_query.exec())
		{
			database.close();
			return false;
		}
	}

	// add to wtable
	for (int i = 0; i < wlist.size(); i++)
	{
		QString item = wlist.at(i).toObject().value("item").toString();
		QString itemtype = wlist.at(i).toObject().value("itemtype").toString();

		// insert to DB
		QString sql = "INSERT INTO " + wtableName + " (itemtype, item) VALUES('" + itemtype + "', '" + item + "')";
		sql_query.prepare(sql);
		if (!sql_query.exec())
		{
			database.close();
			return false;
		}
	}

	database.close();

	return true;
}

bool SpaceDbMgr::getSpaceNameByPath(const QString &spacePath, QString &spaceName)
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

	sql = "SELECT spacename FROM space_table WHERE path='" + spacePath + "'";

	sql_query.prepare(sql);

	if (!sql_query.exec())
	{
		database.close();
		return false;
	}

	 
	QSqlRecord rec = sql_query.record();

	 
	if (!sql_query.next())
	{
		database.close();
		return false;
	}

	spaceName = sql_query.value(0).toString();

	database.close();

	return true;
}

bool SpaceDbMgr::isSpaceExists(const QString &spaceName)
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

	sql = "SELECT spacename,path,rcount,wcount FROM space_table WHERE spacename='" + spaceName + "'";

	sql_query.prepare(sql);

	if (!sql_query.exec())
	{
		database.close();
		return false;
	}

	 
	QSqlRecord rec = sql_query.record();

	 
	if (!sql_query.next())
	{
		database.close();
		return false;
	}

	database.close();

	return true;
}
