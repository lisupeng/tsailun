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

#ifndef _SYSLOG_H
#define _SYSLOG_H

#include <QString>

#include <QSqlDatabase>
#include <QMutex>
#include <QMap>
#include <QJsonArray>

#include <QSqlRecord>
#include <QSqlQuery>
#include <QJsonObject>

enum SYSLOG_LEVEL
{
	SYSLOG_LEVEL_DEBUG       = 1,
	SYSLOG_LEVEL_INFO        = 5,
	SYSLOG_LEVEL_WARN        = 10,
	SYSLOG_LEVEL_ERROR       = 15
};

class Syslog
{
public:
	Syslog();
	virtual ~Syslog();
	bool init();

public:
	bool logMessage(int level, const QString &component, const QString &msg);

	bool getAllMessages(QString &log);

	void setLogLevel(int level);

protected:

protected:
	QMutex        *m_mutex; // inited to recursive mode

protected:
	QMap<Qt::HANDLE, QSqlDatabase> m_dbConnections;

	int            m_curLogLevel;
};


#endif // _SYSLOG_H