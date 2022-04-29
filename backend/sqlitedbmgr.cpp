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

#include "sqlitedbmgr.h"

#include "configmgr.h"

#include <QThread>
#include <QUuid>

SqliteDbMgr::SqliteDbMgr(const QString &dbname, QSqlDatabase **database)
{
	ConfigMgr *cfgMgr = ConfigMgr::GetInstance();
	QString appRootPath = cfgMgr->getAppRootDir();
	QString dbfile;

	if (dbname == "user")
		dbfile = appRootPath + "/data/db/users.db";
	else if (dbname == "space")
		dbfile = appRootPath + "/data/db/spaces.db";
	else if (dbname == "group")
		dbfile = appRootPath + "/data/db/groups.db";
	else
		return;

	m_connName = dbname+"_conn_" + QUuid::createUuid().toString();

	m_database = new QSqlDatabase(QSqlDatabase::addDatabase("QSQLITE", m_connName));

	m_database->setDatabaseName(dbfile);
	m_database->open();

	*database = m_database;
}

SqliteDbMgr::~SqliteDbMgr()
{
	m_database->close();
	delete m_database;
	m_database = nullptr;

	QSqlDatabase::removeDatabase(m_connName);
}