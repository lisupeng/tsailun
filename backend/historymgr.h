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

#ifndef _HISTORYMGR_H
#define _HISTORYMGR_H

#include <QString>
#include <QMutex>
#include <QJsonArray>
#include "versionrecordmgr.h"

class HistoryMgr
{
public:
	HistoryMgr();

public:
	// getPageVersionCount
	bool getVersionList(const QString &uid, QJsonArray &list);
	bool addVersion(const QString &uid, const QString &oldfile, const QString &newfile, const QString &author);
	bool getVersion(const QString &uid, int ver, const QString &outputfile);

protected:
	bool diff(QString oldfile, QString newfile, QString patchfile);
	bool patch(QString oldfile, QString patchfile, QString newfile, QString rejfile);

	bool validatePatchFile(QString oldfile, QString patchfile, QString sha);

	QString getDirForUid(const QString &uid);
	QString createDirForUid(const QString &uid);

	bool _getVersion(VersionRecordMgr &versionRecord, const QString &uid, int ver, const QString &outputfile);

protected:
	QMutex        m_mutex;

	static bool   m_init;
};

#endif // _HISTORYMGR_H