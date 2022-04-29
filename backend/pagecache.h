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

#ifndef _PAGECACHE_H
#define _PAGECACHE_H

#include <stdint.h>
#include <QString>
#include <QMap>
#include <QList>
#include <QMutex>

#include "clist.h"

struct PageCacheData
{

	QString content;       // TODO use ptr for further optimization
	// author
	// modified

	node_t *lruNode;

};

class PageCache
{
public:
	PageCache();
	virtual ~PageCache();

	void SetCapacity(uint32_t size);

	bool pageCacheExists(const QString &url);
	bool getPageFromCache(const QString &url, PageCacheData &cachedata);
	bool addPageToCache(const QString &url, PageCacheData &cachedata);
	void removePageFromCache(const QString &url);

protected:
	QMutex        *m_mutex; // init to recursive mode

	uint32_t       m_capacity;


	QMap<QString, PageCacheData>    m_pageCache;

	list_t        *m_LRUlist;
};


#endif // _PAGECACHE_H