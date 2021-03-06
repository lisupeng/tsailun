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

#ifndef _SQLITEDBMGR_H
#define _SQLITEDBMGR_H

#include <QSqlDatabase>
#include <QString>

class SqliteDbMgr
{
public:
	SqliteDbMgr(const QString &dbname, QSqlDatabase **database);
	virtual ~SqliteDbMgr();

protected:
	QString       m_connName;
	QSqlDatabase  *m_database;
};


#endif // _SQLITEDBMGR_H