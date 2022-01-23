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

#ifndef _PAGELISTMGR_H
#define _PAGELISTMGR_H

#include <QJsonArray>
#include <QString>
#include <QJsonDocument>

typedef struct _PageInfo
{
	QString title;
	QString dir;

	// QString date;
} PageInfo;

class PagelistMgr
{
public:
	PagelistMgr();
	virtual ~PagelistMgr();

public:
	bool read(QString filename);
	bool save(QString filename);

	bool pageTitleExists(QString title);

	int count();
	bool getPageInfo(int i, PageInfo &info);

	int getIndex(const QString &dirname);
	int getIndexByTitle(const QString &title);

	void insert(int i, PageInfo &info);
	void append(const PageInfo &info);

	bool remove(const QString &dirname);

	bool moveup(const QString &dirname);
	bool movedown(const QString &dirname);

	bool rename(const QString &oldtitle, const QString &newtitle);

	QString getTitleByDirname(const QString &dirname);

protected:
	void buildEmptyPagelist();
	bool parseJsonDoc(QJsonDocument &jsonDoc);
protected:
	QString          m_version;
	QJsonArray       m_pagelist;
};

#endif // _PAGELISTMGR_H