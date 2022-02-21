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

#ifndef _USERDBMGR_H
#define _USERDBMGR_H

#include <QSqlDatabase>
#include <QMutex>
#include <QJsonObject>
#include <QJsonArray>

#include <QMap>

class UserDbMgr
{
public:
	UserDbMgr();
	virtual ~UserDbMgr();
	bool init();

	// return userinfo in json
	QJsonObject getUserInfoByEmail(QString email);
	QJsonObject getUserInfoByName(QString account);

	bool getUserList(QJsonArray &list);

	bool createNewUser(const QString &email, const QString &fullname, const QString &credential, QString &err);

	bool createNewUserEx(const QString &account, const QString &email, const QString &fullname, const QString &credential,
		                 const QString &role, const QString &space);

	bool createNewUserPwd(const QString &account, const QString &email, const QString &fullname, const QString &pwd,
		const QString &role, const QString &space);

	bool deleteUser(const QString &account);

	bool getUserInfoByAccount(const QString &account, QJsonObject &userInfo);

	bool updateUser(const QString &account, const QString &fullname, const QString &email, 
		            const QString &role, const QString &space);

	bool updateCredential(const QString &account, const QString &oldcredential, const QString &credential);

	bool isUserAccountExists(const QString &account);

	bool getUserCount(int &count);

	QString autoGenUserAccount();

protected:

	bool init_user_table();
	bool init_user_autocreate_table();

	QJsonObject getUserInfo(QString keytype, QString value);

	void recordToJson(QSqlRecord &rec, QSqlQuery &sql_query, QJsonObject &jsonobj);

	bool __getDbConnections(QSqlDatabase &database);
	void __releaseDbConnections(QSqlDatabase &database);

protected:
	QMutex        *m_mutex; // inited to recursive mode

protected:

};

#endif // _USERDBMGR_H