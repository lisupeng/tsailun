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

#include <QUuid>
#include <QRegExp>
#include <QFile>
#include <QDir>
#include "utils.h"
#include "configmgr.h"

QString PathNameConcat(const QString &path, const QString &filename)
{
	if (path.endsWith("/"))
		return path + filename;
	else
		return path + "/" + filename;
}

QString CreateUid()
{
	// create uid
	QUuid id = QUuid::createUuid();
	QString sUid = id.toString();
	sUid.remove(QRegExp("\\{|\\}|\\-"));

	return sUid;
}

bool ForceCopy(const QString &src, const QString &target)
{
	QFile fs(src);

	if (!fs.open(QFile::ReadOnly))
		return false;

	QFile fd(target);

	if (!fd.open(QFile::WriteOnly))
		return false;

	fd.write((char*)fs.map(0, fs.size()), fs.size()); //Copies all data

	fd.close();
	fs.close();

	return true;
}

QString getSpacePathByUrl(QString url)
{
	// if not start with /spaces
	if (url.indexOf("/spaces") != 0)
		return "";

	// doesn't contain "/pages"
	int idx_pages = url.indexOf("/pages");
	if (idx_pages == -1)
		return "";

	return url.left(idx_pages);
}

bool checkSpacePathInput(const QString &spacePath, QString &spacePathOut)
{
	if (spacePath.size() <= 0)
		return false;

	spacePathOut = spacePath;

	QString tmp = spacePath;
	tmp.replace("\\", "/");

	int len = tmp.size();
	if (tmp[len - 1] == QChar('/'))
	{
		tmp = tmp.left(len - 1);
	}

	spacePathOut = tmp;

	return true;
}

int serverDirCheck(const QString &path)
{
	// check if '../server/' dir exists
	QDir dir(path);
	if (!dir.exists())
		return UTILS_RET_DIR_MISSING_ERR;

	QFileInfoList fileInfoList = dir.entryInfoList();
	for (int i = 0; i < fileInfoList.size(); i++)
	{
		QFileInfo info = fileInfoList[i];

		QString name = info.fileName();

		if (name == "config")
		{
			if (!info.permission(QFile::ExeUser))
				return UTILS_RET_PERMISSION_ERR; // can't enter 'server/config' dir
		}

		if (name == "data")
		{
			if (!info.permission(QFile::ExeUser | QFile::WriteUser | QFile::ReadUser))
				return UTILS_RET_PERMISSION_ERR; // can't enter/write/read 'server/data' dir
		}
	}

	return UTILS_RET_OK;
}

void copyPath(QString src, QString dst)
{
	QDir dir(src);
	if (!dir.exists())
		return;

	foreach(QString d, dir.entryList(QDir::Dirs | QDir::NoDotAndDotDot)) {
		QString dst_path = dst + QDir::separator() + d;
		dir.mkpath(dst_path);
		copyPath(src + QDir::separator() + d, dst_path);
	}

	foreach(QString f, dir.entryList(QDir::Files)) {
		QFile::copy(src + QDir::separator() + f, dst + QDir::separator() + f);
	}
}

QString getSpaceAndPagePathByUrl(QString url)
{
	static QString appRootUrl = "";
	static QString appRootPath = "";

	if (appRootUrl == "")
	{
		ConfigMgr *cfgMgr = ConfigMgr::GetInstance();
		appRootUrl = cfgMgr->getAppRootUrl();
		appRootPath = cfgMgr->getAppRootDir();
	}

	if (!(url.startsWith(appRootUrl)))
		return"";

	QString pagePath = url.right(url.length() - appRootUrl.length());
	QString pageFullPath = appRootPath + "/data/" + pagePath;

	return pageFullPath;
}