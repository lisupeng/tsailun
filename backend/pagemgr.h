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

#ifndef _PAGEMGR_H
#define _PAGEMGR_H

#include <QString>
#include <QDomDocument>


class Page
{
public:
	Page();
	virtual ~Page();

	bool readFromFile(QString filename);
	bool savePage(QString filename);
	void clear();

	void setUid(QString uid);
	QString getUid();

	void setTitle(QString title);
	QString getTitle();

	bool setContent(const QString &content);
	QString getContent();

protected:
	void initNewPage();

protected:
	QDomDocument m_Dom;

	QDomNode _getDomByNodePath(QString nodePath);

};


#endif // _PAGEMGR_H