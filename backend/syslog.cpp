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

#include "syslog.h"
#include "configmgr.h"
#include <QThread>
#include <QDateTime>
#include <QVariant>
#include <QFile>
#include <QTextStream>
#include <stdio.h>
#include <QTimeZone>
#include <QMutexLocker>

Syslog::Syslog()
{
	m_mutex = new QMutex(QMutex::Recursive);

	m_curLogLevel = SYSLOG_LEVEL_INFO;
}

Syslog::~Syslog()
{
	if (m_mutex)
		delete m_mutex;
}

bool Syslog::init()
{
	return true;
}


bool Syslog::logMessage(int level, const QString &component, const QString &msg)
{

	if (level < m_curLogLevel)
		return false;

	QString slevel;

	if (level == SYSLOG_LEVEL_DEBUG)
		slevel = "Debug";
	else if (level == SYSLOG_LEVEL_INFO)
		slevel = "Info";
	else if (level == SYSLOG_LEVEL_WARN)
		slevel = "Warning";
	else if (level == SYSLOG_LEVEL_ERROR)
		slevel = "Error";
	else
		return false;

	ConfigMgr *cfgMgr = ConfigMgr::GetInstance();
	QString appRootPath = cfgMgr->getAppRootDir();
	QString logFile = appRootPath + "/data/log/syslog.txt";

	// get current timestamp
	QDateTime local(QDateTime::currentDateTime());
	QDateTime UTC(local);
	QString utcDataTime = UTC.toString("yyyy-MM-dd HH:mm:ss.zzz");

	QString utcOffset = local.timeZone().displayName(local, QTimeZone::OffsetName);

	utcDataTime = utcDataTime + " " + utcOffset;
	QString timestamp = utcDataTime;

	//QString sql = "INSERT INTO log_table (level, timestamp, component, message) VALUES('" + slevel + "', '" + timestamp + "', '" + component + "', '" + msg + "')";
	QString msgline = timestamp + " [" + slevel + "]: " + msg + "\n";

	QString localDataTime = local.toString("yyyy-MM-dd HH:mm:ss.zzz");
	QString consoleMsg = localDataTime + " [" + slevel + "]: " + msg + "\n";

	QMutexLocker autolock(m_mutex);

#ifdef _WINDOWS
	_cwprintf(L"%s", consoleMsg.toStdWString().c_str());
#else
	printf("%s", consoleMsg.toStdString().c_str()); // for linux
#endif

	// open file and seek to file end
	QFile file(logFile);

	if (file.exists())
	{
		if (!file.open(QIODevice::Append))
			return false;
	}
	else
	{
		if (!file.open(QIODevice::WriteOnly))
			return false;
	}

	file.write(msgline.toUtf8());

	return true;
}


bool Syslog::getAllMessages(QString &log)
{
	ConfigMgr *cfgMgr = ConfigMgr::GetInstance();
	QString appRootPath = cfgMgr->getAppRootDir();
	QString logFile = appRootPath + "/data/log/syslog.txt";

	QFile file(logFile);

	if (!file.open(QIODevice::ReadOnly))
		return false;

	QByteArray bytes = file.readAll();
	log = QString::fromUtf8(bytes);

	return true;
}

bool Syslog::getRecentLog(QString &log)
{
	ConfigMgr *cfgMgr = ConfigMgr::GetInstance();
	QString appRootPath = cfgMgr->getAppRootDir();
	QString logFile = appRootPath + "/data/log/syslog.txt";

	QFile file(logFile);

	if (file.size() <= 500 * 1024)
		return this->getAllMessages(log);

	if (!file.open(QIODevice::ReadOnly))
		return false;

	file.seek(file.size() - 500 * 1024);

	//QByteArray bytes = file.readAll();
	QByteArray bytes = file.read(500*1024);

	int idx = bytes.indexOf("\n");
	if (idx != -1)
	{
		bytes = bytes.right(bytes.size()-idx);
	}

	log = QString::fromUtf8(bytes);

	return true;
}

void Syslog::setLogLevel(int level)
{
	m_curLogLevel = level;
}