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

#include <QSqlQuery>
#include <QVariant>
#include <QSqlRecord>
#include <QLockFile>
#include "versionrecordmgr.h"

#define GET_DATABASE(dbfile) \
           QSqlDatabase database; \
           do { \
               	QLockFile filelock(dbfile + ".lock"); \
                if (filelock.lock()) { \
	                database = QSqlDatabase::addDatabase("QSQLITE"); \
                    database.setDatabaseName(dbfile); \
                    database.open(); \
                } \
		   } while (0);

VersionRecordMgr::VersionRecordMgr(const QString &path)
{
	m_dbFile = path + "/versions.db";

	if (!_initTable())
	{
		return;
	}

	return;
}

VersionRecordMgr::~VersionRecordMgr()
{

}

bool VersionRecordMgr::_initTable()
{
	GET_DATABASE(m_dbFile);

	// create table if not exists
	QSqlQuery sql_query(database);
	QString sql = "CREATE TABLE IF NOT EXISTS version_table (version varchar(8) primary key, "
		"author varchar(64), date varchar(64), sha varchar(70), type varchar(10))";
	sql_query.prepare(sql);

	if (!sql_query.exec())
	{
		return false;
	}

	return true;
}

int VersionRecordMgr::count()
{
	GET_DATABASE(m_dbFile);

	QSqlQuery sql_query(database);
	QString sql = "SELECT count(*) FROM version_table";
	sql_query.prepare(sql);
	if (!sql_query.exec())
	{
		return 0;
	}

	sql_query.next();
	return sql_query.value(0).toInt();
}

bool VersionRecordMgr::append(const QString &version, const QString &author, const QString &date,
	const QString &sha, const QString &type)
{
	GET_DATABASE(m_dbFile);

	QSqlQuery sql_query(database);
	QString sql = "INSERT INTO version_table (version, author, date, sha, type) VALUES('"+version+"','"+author+"','"+date+"','"+sha+"','"+type + "')";

	sql_query.prepare(sql);

	if (!sql_query.exec())
	{
		return false;
	}

	return true;
}

bool VersionRecordMgr::getRecordByVersion(const QString &version, QJsonObject &jsonobj)
{
	GET_DATABASE(m_dbFile);

	QSqlQuery sql_query(database);
	QString sql = "SELECT * FROM version_table where version='" + version + "'";

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

	recordToJson(rec, sql_query, jsonobj);

	return true;
}

bool VersionRecordMgr::getRecordBySHA(const QString &sha, QJsonObject &jsonobj)
{
	GET_DATABASE(m_dbFile);

	QSqlQuery sql_query(database);
	QString sql = "SELECT * FROM version_table where sha='" + sha + "'";

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

	recordToJson(rec, sql_query, jsonobj);

	return true;
}

void VersionRecordMgr::recordToJson(QSqlRecord &rec, QSqlQuery &sql_query, QJsonObject &jsonobj)
{
	int count = rec.count();

	for (int col = 0; col < count; col++)
	{
		jsonobj.insert(rec.fieldName(col), sql_query.value(col).toString());
	}
}

QString VersionRecordMgr::getVersionHash(const QString &version)
{
	QJsonObject jsonobj;
	if (!getRecordByVersion(version, jsonobj))
	{
		return QString();
	}

	return jsonobj.value("sha").toString();
}

bool VersionRecordMgr::getVersionList(QJsonArray &list)
{
	GET_DATABASE(m_dbFile);

	QSqlQuery sql_query(database);
	QString sql = "SELECT version,author,date FROM version_table";

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
