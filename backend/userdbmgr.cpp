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

#include <QSqlError>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QVariant>
#include <QMutexLocker>

#include <QThread>

#include "userdbmgr.h"
#include "configmgr.h"
#include "crypto.h"

UserDbMgr::UserDbMgr()
{
	m_mutex = new QMutex(QMutex::Recursive);
}

UserDbMgr::~UserDbMgr()
{
	if (m_mutex)
		delete m_mutex;
}



bool UserDbMgr::init()
{
	if (!init_user_table())
	{
		return false;
	}

	if (!init_user_autocreate_table())
	{
		return false;
	}

	return true;
}

bool UserDbMgr::init_user_autocreate_table()
{
	// create table if not exists
	QSqlDatabase  database;
	if (!__getDbConnections(database))
	{
		return false;
	}
	QSqlQuery sql_query(database);

	QString sql = "CREATE TABLE IF NOT EXISTS userautocreate_table (nextaccount varchar(20))";

	sql_query.prepare(sql);

	if (!sql_query.exec())
	{
		__releaseDbConnections(database);
		return false;
	}

	sql = "SELECT nextaccount FROM userautocreate_table where nextaccount='11000'";
	sql_query.prepare(sql);
	if (!sql_query.exec())
	{
		__releaseDbConnections(database);
		return false;
	}

	 
	QSqlRecord rec = sql_query.record();

	 
	if (!sql_query.next())
	{
		// insert admin/admin
		QString credential = Crypto::genCredentialByPassword("admin");
		sql = "INSERT INTO userautocreate_table (nextaccount) VALUES('11000')";

		if (!sql_query.exec(sql))
		{
			__releaseDbConnections(database);
			return false;
		}
	}

	__releaseDbConnections(database);
	return true;
}

bool UserDbMgr::init_user_table()
{
	// create table if not exists
	QSqlDatabase  database;
	if (!__getDbConnections(database))
	{
		return false;
	}
	QSqlQuery sql_query(database);

	QString sql = "CREATE TABLE IF NOT EXISTS user_table (account varchar(64) primary key, fullname varchar(64), "
		"email varchar(255), role varchar(16), space varchar(64), uid varchar(64), createdate varchar(64), "
		"signindate varchar(64), credential varchar(64), extrainfo varchar(2048))";

	sql_query.prepare(sql);

	if (!sql_query.exec())
	{
		__releaseDbConnections(database);
		return false;
	}

	sql = "SELECT email,account,credential FROM user_table where account='admin'";
	sql_query.prepare(sql);
	if (!sql_query.exec())
	{
		__releaseDbConnections(database);
		return false;
	}

	 
	QSqlRecord rec = sql_query.record();

	 
	if (!sql_query.next())
	{
		// insert admin/admin
		QString credential = Crypto::genCredentialByPassword("admin");
		sql = "INSERT INTO user_table (email, account, role, space, credential) VALUES(NULL, 'admin', 'admin', 'space_default', '" + credential + "')";

		if (!sql_query.exec(sql))
		{
			__releaseDbConnections(database);
			return false;
		}

		// insert guest/guest
		credential = Crypto::genCredentialByPassword("guest");
		sql = "INSERT INTO user_table (email, account, role, space, credential) VALUES(NULL, 'guest', 'guest', 'space_default', '" + credential + "')";

		if (!sql_query.exec(sql))
		{
			__releaseDbConnections(database);
			return false;
		}
	}

	__releaseDbConnections(database);
	return true;
}

QJsonObject UserDbMgr::getUserInfo(QString keytype, QString value)
{
	QJsonObject jsonobj;
	QMutexLocker autolock(m_mutex);

	QSqlDatabase  database;
	if (!__getDbConnections(database))
	{
		return jsonobj;
	}

	// create table if not exists
	QSqlQuery sql_query(database);
	QString sql;

	if (keytype == "account")
		sql = "SELECT * FROM user_table where account='" + value + "'";
	else if (keytype == "email")
		sql = "SELECT * FROM user_table where email='" + value + "'";
	else
	{
		__releaseDbConnections(database);
		return jsonobj;
	}

	sql_query.prepare(sql);

	if (!sql_query.exec())
	{
		__releaseDbConnections(database);
		return jsonobj;
	}

	 
	QSqlRecord rec = sql_query.record();

	 
	if (!sql_query.next())
	{
		__releaseDbConnections(database);
		return jsonobj;
	}

	recordToJson(rec, sql_query, jsonobj);

	__releaseDbConnections(database);

	return jsonobj;
}

QJsonObject UserDbMgr::getUserInfoByEmail(QString email)
{
	return getUserInfo("email", email);
}

QJsonObject UserDbMgr::getUserInfoByName(QString account)
{
	return getUserInfo("account", account);
}

void UserDbMgr::recordToJson(QSqlRecord &rec, QSqlQuery &sql_query, QJsonObject &jsonobj)
{
	int count = rec.count();

	for (int col = 0; col < count; col++)
	{
		jsonobj.insert(rec.fieldName(col), sql_query.value(col).toString());
	}
}

bool UserDbMgr::getUserList(QJsonArray &list)
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

	sql = "SELECT account,fullname,email,role,space FROM user_table";

	sql_query.prepare(sql);

	if (!sql_query.exec())
	{
		__releaseDbConnections(database);
		return false;
	}

	 
	QSqlRecord rec = sql_query.record();

	 
	while (sql_query.next())
	{
		QJsonObject jsonobj;
		recordToJson(rec, sql_query, jsonobj);
		list.append(jsonobj);
	}

	__releaseDbConnections(database);

	return true;
}

bool UserDbMgr::createNewUser(const QString &email, const QString &fullname, const QString &credential, QString &err)
{
	QMutexLocker autolock(m_mutex);

	// make sure use doesn't exist already
	QJsonObject existingObj = getUserInfoByEmail(email);
	if (!existingObj.isEmpty())
	{
		err = "email_already_exists";
		return false;
	}

	QSqlDatabase  database;
	if (!__getDbConnections(database))
	{
		return false;
	}

	QSqlQuery sql_query(database);
	QString sql;

	sql = "INSERT INTO user_table (account, email, fullname, role, space, credential) VALUES('"+email+ "', '" +email+"', '"+fullname+"', 'regular', 'space_default', '" + credential + "')";

	sql_query.prepare(sql);

	if (!sql_query.exec())
	{
		__releaseDbConnections(database);
		return false;
	}

	__releaseDbConnections(database);

	return true;
}

bool UserDbMgr::createNewUserEx(const QString &account, const QString &email, const QString &fullname,
	                            const QString &credential, const QString &role, const QString &space)
{
	QMutexLocker autolock(m_mutex);

	// make sure use doesn't exist already
	if (isUserAccountExists(account))
	{
		return false;
	}

	QSqlDatabase  database;
	if (!__getDbConnections(database))
	{
		return false;
	}

	QSqlQuery sql_query(database);
	QString sql;

	sql = "INSERT INTO user_table (account, email, fullname, role, space, credential) VALUES('" + account + "', '" + email + "', '" + fullname + "', '" + role + "', '" + space + "', '" + credential + "')";

	sql_query.prepare(sql);

	if (!sql_query.exec())
	{
		__releaseDbConnections(database);
		return false;
	}

	__releaseDbConnections(database);

	return true;
}

bool UserDbMgr::createNewUserPwd(const QString &account, const QString &email, const QString &fullname, const QString &pwd,
	const QString &role, const QString &space)
{
	QString credential = Crypto::genCredentialByPassword(pwd);
	return createNewUserEx(account, email, fullname, credential, role, space);
}

bool UserDbMgr::deleteUser(const QString &account)
{
	QMutexLocker autolock(m_mutex);

	QSqlDatabase  database;
	if (!__getDbConnections(database))
	{
		return false;
	}

	QSqlQuery sql_query(database);
	QString sql;

	sql = "DELETE FROM user_table WHERE account='" + account + "'";

	sql_query.prepare(sql);

	if (!sql_query.exec())
	{
		__releaseDbConnections(database);
		return false;
	}

	__releaseDbConnections(database);

	return true;
}

bool UserDbMgr::__getDbConnections(QSqlDatabase &database)
{
	// create one
	ConfigMgr *cfgMgr = ConfigMgr::GetInstance();
	QString appRootPath = cfgMgr->getAppRootDir();
	QString userDbFile = appRootPath + "/data/db/users.db";

	char szName[64];
	sprintf(szName, "spacedb_conn_%p", QThread::currentThreadId());
	QString connName = szName;
	database = QSqlDatabase::addDatabase("QSQLITE", connName);

	database.setDatabaseName(userDbFile);

	if (!database.open() || !database.isValid())
	{
		return false;
	}

	return true;
}

void UserDbMgr::__releaseDbConnections(QSqlDatabase &database)
{
	database.close();
}

bool UserDbMgr::getUserInfoByAccount(const QString &account, QJsonObject &userInfo)
{
	userInfo = getUserInfo("account", account);
	userInfo.remove("credential");
	return true;
}

bool UserDbMgr::updateUser(const QString &account, const QString &fullname, const QString &email,
	                       const QString &role, const QString &space)
{
	QMutexLocker autolock(m_mutex);

	QSqlDatabase  database;
	if (!__getDbConnections(database))
	{
		return false;
	}

	// create table if not exists
	QSqlQuery sql_query(database);
	QString sql;

	sql = "UPDATE user_table SET fullname='" + fullname +"',email='"+email+"', role='"+role+"',space='"+space+ "' WHERE account='" + account + "'";
	sql_query.prepare(sql);

	if (!sql_query.exec())
	{
		__releaseDbConnections(database);
		return false;
	}

	__releaseDbConnections(database);

	return true;
}

bool UserDbMgr::updateCredential(const QString &account, const QString &oldcredential, const QString &credential)
{
	if (account.size() == 0 || oldcredential.size() == 0 || credential.size() == 0)
		return false;

	if (account == "guest")
		return false;

	QJsonObject userInfo = getUserInfo("account", account);

	if (userInfo.value("credential").toString() != oldcredential)
		return false;

	QMutexLocker autolock(m_mutex);

	QSqlDatabase  database;
	if (!__getDbConnections(database))
	{
		return false;
	}

	// create table if not exists
	QSqlQuery sql_query(database);
	QString sql;

	sql = "UPDATE user_table SET credential='" + credential + "' WHERE account='" + account + "'";
	sql_query.prepare(sql);

	if (!sql_query.exec())
	{
		__releaseDbConnections(database);
		return false;
	}

	__releaseDbConnections(database);

	return true;
}

bool UserDbMgr::isUserAccountExists(const QString &account)
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

	sql = "SELECT * FROM user_table where account='" + account + "'";


	sql_query.prepare(sql);

	if (!sql_query.exec())
	{
		__releaseDbConnections(database);
		return false;
	}

	 
	QSqlRecord rec = sql_query.record();

	 
	if (!sql_query.next())
	{
		__releaseDbConnections(database);
		return false;
	}

	__releaseDbConnections(database);

	return true;
}

bool UserDbMgr::getUserCount(int &count)
{
	QMutexLocker autolock(m_mutex);

	QSqlDatabase  database;
	if (!__getDbConnections(database))
	{
		return false;
	}

	QSqlQuery sql_query(database);
	QString sql = "SELECT count(*) FROM user_table";
	sql_query.prepare(sql);
	if (!sql_query.exec())
	{
		__releaseDbConnections(database);
		return false;
	}

	sql_query.next();

	count = sql_query.value(0).toInt();

	__releaseDbConnections(database);

	return true;
}

QString UserDbMgr::autoGenUserAccount()
{
	// create table if not exists
	QSqlDatabase  database;
	if (!__getDbConnections(database))
	{
		return "";
	}
	QSqlQuery sql_query(database);

	QString sql = "SELECT * FROM userautocreate_table";
	sql_query.prepare(sql);
	if (!sql_query.exec())
	{
		__releaseDbConnections(database);
		return "";
	}

	 
	QSqlRecord rec = sql_query.record();

	 
	if (!sql_query.next())
	{
		__releaseDbConnections(database);
		return "";
	}

	//
	QString account = sql_query.value(0).toString();

	int _nextaccount = account.toInt() + 1;
	QString nextaccount = QString("%1").arg(_nextaccount);

	sql = "UPDATE userautocreate_table SET nextaccount='" + nextaccount + "' WHERE nextaccount='" + account + "'";
	sql_query.prepare(sql);

	if (!sql_query.exec())
	{
		__releaseDbConnections(database);
		return "";
	}

	__releaseDbConnections(database);

	return account;
}