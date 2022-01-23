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

//#include <QUuid>
//#include <QStringList>
//#include <QDir>
//#include <QRegExp>
//#include <QTextStream>
//#include "uidmgr.h"
//#include "utils.h"
//
//static QString GetUidFilename(const QString &path)
//{
//	QDir dir(path);
//	if (!dir.exists())
//		return "";
//
//	QStringList filters;
//	filters << "*.uid";
//
//	QStringList list = dir.entryList(filters);
//
//	if (list.size() == 0)
//		return "";
//	else if (list.size() == 1)
//		return list[0];
//	else
//	{
//		// TODO shouldn't be here, log error. notify admin
//		return list[0];
//	}
//}

//QString UidMgr::ReadUidFromPath(const QString &path)
//{
//	QString uidFilename = GetUidFilename(path);
//
//	if (uidFilename == "")
//		return "";
//
//	// remove ".uid" and return
//	uidFilename.remove(QRegExp("\\.uid"));
//
//	return uidFilename;
//}

//QString UidMgr::CreateUid()
//{
//	// create uid
//	QUuid id = QUuid::createUuid();
//	QString sUid = id.toString();
//	sUid.remove(QRegExp("\\{|\\}"));
//
//	return sUid;
//}