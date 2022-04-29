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

#include "pagecache.h"

static void free_clist_node(void *data)
{
	QString *pstr = (QString *)data;
	delete pstr;
}

PageCache::PageCache()
{
	m_capacity = 1024; // TODO make this value can be configured

	m_mutex = new QMutex(QMutex::Recursive);

	m_LRUlist = new_list();

}

PageCache::~PageCache()
{
	if (m_mutex)
		delete m_mutex;

	empty_list(m_LRUlist, free_clist_node);
}

void PageCache::SetCapacity(uint32_t size)
{
	QMutexLocker autolock(m_mutex);

	m_capacity = size;
}

bool PageCache::pageCacheExists(const QString &url)
{
	QMutexLocker autolock(m_mutex);

	if (m_pageCache.find(url) != m_pageCache.end())
		return true;
	else
		return false;
}

bool PageCache::getPageFromCache(const QString &url, PageCacheData &cachedata)
{
	QMutexLocker autolock(m_mutex);

	QMap<QString, PageCacheData>::const_iterator i = m_pageCache.find(url);

	if (i == m_pageCache.end())
		return false;

	cachedata = i.value();

	remove_node_do_not_free(m_LRUlist, cachedata.lruNode);

	//push_back_node
	push_back_node(m_LRUlist, cachedata.lruNode);

	return true;
}

bool PageCache::addPageToCache(const QString &url, PageCacheData &cachedata)
{
	QMutexLocker autolock(m_mutex);

	if (m_capacity == 0)
		return false;

	// Already exists, do update
	if (pageCacheExists(url))
	{
		return true;
	}

	// check if cache full, remove Least Recently used
	if ((m_pageCache.size() + 1) > m_capacity)
	{
		QString _delUrl = *((QString *)get_front(m_LRUlist));
		m_pageCache.remove(_delUrl);
		remove_front(m_LRUlist, free_clist_node);
	}

	// add to cache
	node_t *node = new node_t;
	QString *str = new QString(url);
	node->data = str;

	cachedata.lruNode = node;

	m_pageCache[url] = cachedata;

	push_back_node(m_LRUlist, node);

	return true;
}

void PageCache::removePageFromCache(const QString &url)
{
	QMutexLocker autolock(m_mutex);

	QMap<QString, PageCacheData>::const_iterator i = m_pageCache.find(url);

	if (i == m_pageCache.end())
		return;

	PageCacheData cachedata = i.value();

	remove_node_do_not_free(m_LRUlist, cachedata.lruNode);

	free_clist_node(cachedata.lruNode->data);

	delete cachedata.lruNode;

	m_pageCache.remove(url);
}


