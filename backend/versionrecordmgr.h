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

#ifndef _VERSIONRECORDMGR_H
#define _VERSIONRECORDMGR_H

#include <QString>
#include <QSqlDatabase>
#include <QJsonObject>
#include <QJsonArray>

class VersionRecordMgr
{
public:
	VersionRecordMgr(const QString &path);
	virtual ~VersionRecordMgr();

public:
	bool append(const QString &version, const QString &author, const QString &date,
		        const QString &sha, const QString &type);

	int count();

	bool getRecordByVersion(const QString &version, QJsonObject &jsonobj);
	bool getRecordBySHA(const QString &sha, QJsonObject &jsonobj);

	bool getVersionList(QJsonArray &list);

	QString getVersionHash(const QString &version);

protected:
	bool _initTable();
	void recordToJson(QSqlRecord &rec, QSqlQuery &sql_query, QJsonObject &jsonobj);

protected:
	QString       m_dbFile;
};

#endif // _VERSIONRECORDMGR_H