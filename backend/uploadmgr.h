/*
* This file is part of the Tsailun project
*
* Copyright (c) 2022 Li Supeng
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

#ifndef _UPLOADMGR_H
#define _UPLOADMGR_H

#include <QString>
#include <QMutex>
#include <QJsonObject>
#include <map>

struct UploadData
{
	QString         tid;
	QString         path;
	QString         filename;
	qint64          size;

	// add timestamp and timeout
};

class UploadMgr
{
public:
	UploadMgr();
	virtual ~UploadMgr();

	bool    addRecord(const QString &tid, const UploadData &data);
	bool    getRecord(const QString &tid, UploadData &data);

protected:
	std::map<QString, UploadData>      m_uploadMap;
	QMutex                            *m_mutex; // init to recursive mode
};


#endif // _UPLOADMGR_H