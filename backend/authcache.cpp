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

#include "authcache.h"

#include <QPair>

AuthCache::AuthCache()
{
	m_capacity = 20000; // TODO this value is big enought at present. Make it configurable

}

AuthCache::~AuthCache()
{

}

void AuthCache::add(const QString &key, AuthCacheData &cachedata)
{
	QMutexLocker autolock(m_mutex);

	if (m_authCache.size() + 1 > m_capacity)
	{
		QString toDel = m_queue.front();
		m_queue.removeAt(0);

		m_authCache.remove(toDel);
	}

	m_queue.push_back(key);
	m_authCache[key] = cachedata;
}

bool AuthCache::get(const QString &key, AuthCacheData &cachedata)
{
	QMutexLocker autolock(m_mutex);

	QMap<QString, AuthCacheData>::const_iterator i = m_authCache.find(key);

	if (i == m_authCache.end())
		return false;

	cachedata = i.value();

	return true;
}

void AuthCache::removeItemsFromCacheByMatchedKeys(const QString &substr)
{
	QMutexLocker autolock(m_mutex);

	QList<QPair<QString, unsigned int>>   delList;

	for (unsigned int i = 0; i < m_queue.size(); i++)
	{
		QString key = m_queue.at(i);

		if (key.indexOf(substr) != -1)
		{
			QPair<QString, unsigned int> pair;
			pair.first = key;
			pair.second = i;

			// order REVERSED, because need to delete in reversed order from m_queue later
			delList.push_front(pair);
		}

	}

	for (unsigned int i = 0; i < delList.size(); i++)
	{
		QString key = delList.at(i).first;
		unsigned int pos = delList.at(i).second;

		m_authCache.remove(key);
		m_queue.removeAt(pos);

	}
}

void AuthCache::clearAll()
{
	QMutexLocker autolock(m_mutex);

	m_authCache.clear();
	m_queue.clear();
}