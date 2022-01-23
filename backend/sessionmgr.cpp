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

#include <QMutexLocker>
#include "sessionmgr.h"

SessionMgr::SessionMgr()
{
	m_mutex = new QMutex(QMutex::Recursive);
}

SessionMgr::~SessionMgr()
{
	if (m_mutex)
		delete m_mutex;
}

void SessionMgr::addSession(const QString &sid, const QJsonObject &sessionObj)
{
	QMutexLocker autolock(m_mutex);

	m_sessionMap[sid] = sessionObj;
}

void SessionMgr::removeSession(const QString &sid)
{
	QMutexLocker autolock(m_mutex);

	std::map<QString, QJsonObject>::iterator it = m_sessionMap.find(sid);
	if (it == m_sessionMap.end())
		return;

	m_sessionMap.erase(it);
}

bool SessionMgr::getSession(const QString &sid, QJsonObject &sessionObj)
{
	QMutexLocker autolock(m_mutex);

	std::map<QString, QJsonObject>::iterator it = m_sessionMap.find(sid);
	if (it == m_sessionMap.end())
		return false;

	sessionObj = m_sessionMap[sid];

	return true;
}

bool SessionMgr::getAccountBySid(const QString &sid, QString &account)
{
	QMutexLocker autolock(m_mutex);

	if (sid.isNull() || sid == "")
	{
		account = "guest";
		return true;
	}

	QJsonObject sessionobj;
	if (getSession(sid, sessionobj))
	{
		account = sessionobj.value("account").toString();
		return true;
	}
	else
	{
		return false;
	}
}