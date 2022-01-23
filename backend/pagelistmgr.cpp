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

#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>
#include <QJsonValue>
#include "pagelistmgr.h"

PagelistMgr::PagelistMgr()
{
	buildEmptyPagelist();
}

PagelistMgr::~PagelistMgr()
{
}

void PagelistMgr::buildEmptyPagelist()
{
	m_version = "1.0";

	m_pagelist = QJsonArray();
}

bool PagelistMgr::parseJsonDoc(QJsonDocument &jsonDoc)
{
	if (jsonDoc.isNull())
		return false;

	if (!jsonDoc.isObject())
		return false;

	QJsonObject jsonObj = jsonDoc.object();

	if (!jsonObj.value("version").isString())
		return false;

	m_version = jsonObj.value("version").toString();

	if (!jsonObj.value("list").isArray())
		return false;

	m_pagelist = jsonObj.value("list").toArray();

	return true;
}

bool PagelistMgr::read(QString filename)
{
	QFile file(filename);
	if (!file.exists())
		return false;

	if (!file.open(QIODevice::ReadOnly))
		return false;

	QJsonParseError error;
	QJsonDocument jsonDoc = QJsonDocument::fromJson(file.readAll(), &error);

	file.close();

	return parseJsonDoc(jsonDoc);
}

bool PagelistMgr::save(QString filename)
{
	QFile file(filename);
	if (!file.open(QIODevice::ReadWrite)) {
		return false;
	}

	// clear file content
	file.resize(0);

	QJsonDocument jsonDoc;
	QJsonObject jsonObj;
	jsonObj.insert("version", m_version);
	jsonObj.insert("list", m_pagelist);

	jsonDoc.setObject(jsonObj);

	file.write(jsonDoc.toJson());
	file.close();

	return true;
}

bool PagelistMgr::pageTitleExists(QString title)
{
	for (int i = 0; i < m_pagelist.size(); i++)
	{
		QJsonObject obj = m_pagelist.at(i).toObject();

		if (title == obj.value("title").toString())
			return true;
	}

	return false;
}

int PagelistMgr::count()
{
	return m_pagelist.size();
}

bool PagelistMgr::getPageInfo(int i, PageInfo &info)
{
	if (i >= m_pagelist.size())
		return false;

	QJsonObject obj = m_pagelist.at(i).toObject();

	info.title = obj.value("title").toString();
	info.dir = obj.value("dir").toString();

	return true;
}

int PagelistMgr::getIndex(const QString &dirname)
{
	for (int i = 0; i < m_pagelist.size(); i++)
	{
		QJsonObject obj = m_pagelist.at(i).toObject();

		if (dirname == obj.value("dir").toString())
		{
			return i;
		}
	}

	return -1;
}

int PagelistMgr::getIndexByTitle(const QString &title)
{
	for (int i = 0; i < m_pagelist.size(); i++)
	{
		QJsonObject obj = m_pagelist.at(i).toObject();

		if (title == obj.value("title").toString())
		{
			return i;
		}
	}

	return -1;
}

void PagelistMgr::insert(int i, PageInfo &info)
{
	QJsonObject obj;
	obj.insert("title", info.title);
	obj.insert("dir", info.dir);

	m_pagelist.insert(i, obj);
}

void PagelistMgr::append(const PageInfo &info)
{
	QJsonObject obj;
	obj.insert("title", info.title);
	obj.insert("dir", info.dir);

	m_pagelist.append(obj);
}

bool PagelistMgr::remove(const QString &dirname)
{
	for (int i = 0; i < m_pagelist.size(); i++)
	{
		QJsonObject obj = m_pagelist.at(i).toObject();

		if (dirname == obj.value("dir").toString())
		{
			m_pagelist.removeAt(i);
			return true;
		}
	}

	return false;
}

bool PagelistMgr::moveup(const QString &dirname)
{
	for (int i = 0; i < m_pagelist.size(); i++)
	{
		QJsonObject obj = m_pagelist.at(i).toObject();

		if (dirname == obj.value("dir").toString())
		{
			if (i == 0)
				return true;

			m_pagelist.removeAt(i);
			m_pagelist.insert(i - 1, obj);
			return true;
		}
	}

	return false;
}

bool PagelistMgr::movedown(const QString &dirname)
{
	for (int i = 0; i < m_pagelist.size(); i++)
	{
		QJsonObject obj = m_pagelist.at(i).toObject();

		if (dirname == obj.value("dir").toString())
		{
			if (i == m_pagelist.size()-1)
				return true;

			m_pagelist.removeAt(i);
			m_pagelist.insert(i + 1, obj);
			return true;
		}
	}

	return false;
}

bool PagelistMgr::rename(const QString &oldtitle, const QString &newtitle)
{

	int idx = getIndexByTitle(oldtitle);
	if (idx == -1)
		return false;

	QJsonObject obj = m_pagelist.at(idx).toObject();

	obj.remove("title");
	obj.insert("title", newtitle);

	m_pagelist.replace(idx, obj);
	
	return true;
}

QString PagelistMgr::getTitleByDirname(const QString &dirname)
{
	int idx = getIndex(dirname);

	if (idx == -1)
		return "";

	return m_pagelist.at(idx).toObject().value("title").toString();
}