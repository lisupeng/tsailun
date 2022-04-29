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

#ifndef _AUTHCACHE_H
#define _AUTHCACHE_H

#include <QMutex>
#include <QString>
#include <QMap>
#include <QList>

struct AuthCacheData
{
	int permission;
};

class AuthCache
{
public:
	AuthCache();
	virtual ~AuthCache();

	// key:  "user@space_path@read/write"
	void add(const QString &key, AuthCacheData &cachedata);
	bool get(const QString &key, AuthCacheData &cachedata);
	void removeItemsFromCacheByMatchedKeys(const QString &substr);
	void clearAll();

protected:
	QMutex        *m_mutex; // init to recursive mode

	unsigned int   m_capacity;

	QMap<QString, AuthCacheData>    m_authCache;
	QList<QString>                  m_queue;
};

#endif // _AUTHCACHE_H