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

#ifndef _SPACEDBMGR_H
#define _SPACEDBMGR_H

#include <QSqlDatabase>
#include <QMutex>
#include <QJsonArray>

#include <QMap>

class SpaceDbMgr
{
public:
	SpaceDbMgr();
	virtual ~SpaceDbMgr();
	bool init();

	bool addSpace(const QString &spaceName, const QString &path);
	bool deleteSpace(const QString &spaceName);
	bool addToSpaceRlist(const QString &spaceName, const QString &type, const QString &item); // type should be "user" or "group"
	bool addToSpaceWlist(const QString &spaceName, const QString &type, const QString &item);

	bool getSpaceList(QJsonArray &list);

	bool getSpaceDetail(const QString &spaceName, QJsonObject &spaceDetail);
	bool getSpaceRlist(const QString &spaceName, QJsonArray &rlist);
	bool getSpaceWlist(const QString &spaceName, QJsonArray &wlist);

	bool updateSpace(const QString &spaceName, const QJsonArray &rlist, const QJsonArray &wlist);

	bool getSpaceNameByPath(const QString &spacePath, QString &spaceName);

	bool isSpaceExists(const QString &spaceName);

protected:
	bool init_space_table();

	bool insertDefaultSpace();

	bool _getSpaceCount(int &count, QSqlDatabase  &database);
	bool _getSizeOfSpaceRlist(const QString &spaceName, int &size, QSqlDatabase  &database);
	bool _getSizeOfSpaceWlist(const QString &spaceName, int &size, QSqlDatabase  &database);

	bool _getSpaceRlist(const QString &spaceName, QJsonArray &rlist, QSqlDatabase  &database);
	bool _getSpaceWlist(const QString &spaceName, QJsonArray &wlist, QSqlDatabase  &database);

//	bool __opendb();
//	void __closedb();

	void recordToJson(QSqlRecord &rec, QSqlQuery &sql_query, QJsonObject &jsonobj);

	//bool __getDbConnections(QSqlDatabase &database);

protected:
	QMutex        *m_mutex; // inited to recursive mode

protected:
	QMap<Qt::HANDLE, QSqlDatabase> m_dbConnections;

};

#endif // _SPACEDBMGR_H