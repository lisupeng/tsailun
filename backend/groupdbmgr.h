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

#ifndef _GROUPDBMGR_H
#define _GROUPDBMGR_H

#include <QSqlDatabase>
#include <QMutex>
#include <QJsonArray>

#include <QMap>

class GroupDbMgr
{
public:
	GroupDbMgr();
	virtual ~GroupDbMgr();
	bool init();

	bool addGroup(const QString &groupName, const QString &comment);
	bool delGroup(const QString &groupName);
	bool addUserToGroup(const QString &groupName, const QString &useraccount);
	bool getGroupList(QJsonArray &list);

	bool getGroupInfo(const QString &groupName, QJsonObject &groupinfo);

	bool updateGroup(const QString &groupName, const QString &comment, const QJsonArray &userlist);

	bool getUserlist(const QString &groupName, QJsonArray &list);

	bool isUserInGroup(const QString &groupName, const QString &account);

protected:
	bool init_group_table();

	void recordToJson(QSqlRecord &rec, QSqlQuery &sql_query, QJsonObject &jsonobj);

	bool __getDbConnections(QSqlDatabase &database);

protected:
	QMutex        *m_mutex; // inited to recursive mode

protected:
	QMap<Qt::HANDLE, QSqlDatabase> m_dbConnections;
};

#endif // _GROUPDBMGR_H