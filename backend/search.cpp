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

#include "search.h"
#include "spacedbmgr.h"
#include "auth.h"
#include "configmgr.h"
#include "pagelistmgr.h"
#include "htmlparser/src/htmlparser_interface.h"
#include "syslog.h"
#include <QJsonObject>
#include <QDirIterator>
#include <QRegExp>

extern SpaceDbMgr    g_spaceDbMgr;
extern Syslog        g_syslog;

static QString _getPageRelativePath(const QString &pagedir, const QString &listFilePath)
{
	QString tmp = listFilePath.left(listFilePath.size()-QString("/pagelist.json").size());

	int idx = tmp.indexOf("/spaces");
	if (idx == -1)
		return "";

	tmp = tmp.right(tmp.size()-idx);

	return tmp+"/"+pagedir;
}

static QString _getPageRelativePathByHtmlFile(const QString &htmlFilePath)
{
	QString tmp = htmlFilePath.left(htmlFilePath.size() - QString("/index.html").size());

	int idx = tmp.indexOf("/spaces");
	if (idx == -1)
		return "";

	tmp = tmp.right(tmp.size() - idx);

	return tmp;
}

static bool __titleContails(const QString &title, const QStringList &searchInput)
{
	if (searchInput.size() == 0)
		return false;

	for (int i = 0; i < searchInput.size(); i++)
	{
		if (!title.toUpper().contains(searchInput[i]))
			return false;
	}

	return true;
}

static QStringList preprocessSearchInput(const QString &searchInput)
{
	// TODO-2 handle multiple spaces case(e.g 'hello   world 123')
	QString upper = searchInput.toUpper();
	return upper.split(" ");
}

bool SearchMgr::searchTitle(const QString &sid, const QString &searchInput, QJsonArray &searchResult)
{
	// 1.get space list
	QJsonArray list;
	if (!g_spaceDbMgr.getSpaceList(list))
	{
		return false;
	}

	// 2.filter out space that can't be read by user
	QJsonArray listAfterFiltering;
	for (int i = 0; i < list.size(); i++)
	{
		QJsonObject obj = list.at(i).toObject();

		QString spaceName = obj.value("spacename").toString();

		int authres = AuthMgr::SpaceReadPermissionCheckBySpaceName(sid, spaceName);

		if (authres == AUTH_RET_SUCCESS)
		{
			listAfterFiltering.append(obj);
		}
	}

	// 3. preprocess searchInput
	QStringList processedSearchInput = preprocessSearchInput(searchInput);

	// 4.enumerate page title in space and check if contains searchInput
	for (int i = 0; i < listAfterFiltering.size(); i++)
	{
		QJsonObject spaceObj = listAfterFiltering.at(i).toObject();

		QString spaceName = spaceObj.value("spacename").toString();
		QString spacePath = spaceObj.value("path").toString();

		searchTitleInSpace(spacePath, processedSearchInput, searchResult);
	}

	return true;
}

bool SearchMgr::searchTitleInSpace(const QString &spacePath, const QStringList &processedSearchInput, QJsonArray &searchResult)
{
	ConfigMgr *cfgMgr = ConfigMgr::GetInstance();
	QString appRootPath = cfgMgr->getAppRootDir();
	QString spaceFullPath = appRootPath + "/data" + spacePath + "/pages";

	// search spaceFullPath and its subdirs to find 'pagelist.json'
	QStringList filter;
	filter << "pagelist.json";

	QDirIterator it(spaceFullPath, filter, QDir::AllEntries | QDir::NoSymLinks | QDir::NoDotAndDotDot, QDirIterator::Subdirectories);
	QStringList pageListFiles;
	while (it.hasNext())
	{
		QString filename = it.next();
		pageListFiles << filename;
	}

	// get titles & paths from pagelist files and search
	for (int i = 0; i < pageListFiles.size(); i++)
	{
		QString listFilePath = pageListFiles[i];

		if (!QFile(listFilePath).exists())
		{
			g_syslog.logMessage(SYSLOG_LEVEL_ERROR, "", "Can't find list path.");
			continue;
		}

		PagelistMgr listMgr;
		if (!listMgr.read(listFilePath))
		{
			g_syslog.logMessage(SYSLOG_LEVEL_ERROR, "", "Can't read list file.");
			continue;
		}

		int count = listMgr.count();
		for (int i = 0; i < count; i++)
		{
			PageInfo info;
			listMgr.getPageInfo(i, info);
			QString pagedir = info.dir;
			QString title = info.title;

			if (__titleContails(title, processedSearchInput))
			{
				// add to result
				QJsonObject searchResEntry;
				searchResEntry.insert("title", title);
				QString pageRelativePath = _getPageRelativePath(pagedir, listFilePath);
				searchResEntry.insert("path", pageRelativePath);

				searchResult.append(searchResEntry);
			}
		}
	}

	return true;
}

bool SearchMgr::searchText(const QString &sid, const QString &searchInput, QJsonArray &searchResult)
{
	// 1.get space list
	QJsonArray list;
	if (!g_spaceDbMgr.getSpaceList(list))
	{
		return false;
	}

	// 2.filter out space that can't be read by user
	QJsonArray listAfterFiltering;
	for (int i = 0; i < list.size(); i++)
	{
		QJsonObject obj = list.at(i).toObject();

		QString spaceName = obj.value("spacename").toString();

		int authres = AuthMgr::SpaceReadPermissionCheckBySpaceName(sid, spaceName);

		if (authres == AUTH_RET_SUCCESS)
		{
			listAfterFiltering.append(obj);
		}
	}

	// 3. preprocess searchInput
	QStringList processedSearchInput = preprocessSearchInput(searchInput);

	// 4.enumerate page title in space and check if contains searchInput
	for (int i = 0; i < listAfterFiltering.size(); i++)
	{
		QJsonObject spaceObj = listAfterFiltering.at(i).toObject();

		QString spaceName = spaceObj.value("spacename").toString();
		QString spacePath = spaceObj.value("path").toString();

		searchTextInSpace(spacePath, processedSearchInput, searchResult);
	}

	return true;
}

bool SearchMgr::searchTextInSpace(const QString &spacePath, const QStringList &processedSearchInput, QJsonArray &searchResult)
{
	ConfigMgr *cfgMgr = ConfigMgr::GetInstance();
	QString appRootPath = cfgMgr->getAppRootDir();
	QString spaceFullPath = appRootPath + "/data" + spacePath + "/pages";

	QStringList filter;
	filter << "index.html";

	QDirIterator it(spaceFullPath, filter, QDir::AllEntries | QDir::NoSymLinks | QDir::NoDotAndDotDot, QDirIterator::Subdirectories);

	QStringList pageFiles;
	while (it.hasNext())
	{
		QString filename = it.next();
		pageFiles << filename;
	}

	for (int i = 0; i < pageFiles.size(); i++)
	{
		QString pagefilename = pageFiles[i];
		QFile file(pagefilename);

		if (file.open(QIODevice::ReadOnly))
		{
			std::string cleantext = retrieve_text_from_html(&(file.readAll()));
			// TODO-0 perf optimize
			QString text = QString::fromUtf8(cleantext.c_str());

			// TODO-0 make sure processedSearchInput doesn't contain regular expression special characters
			// e.g. "c++"-->"c\+\+"

			QRegExp re;
			QString pattern = "";

			for (int j = 0; j < processedSearchInput.size(); j++)
			{
				if (pattern.size() == 0)
				{
					pattern = processedSearchInput[j];
				}
				else
				{
					pattern = pattern + "|" + processedSearchInput[j];
				}
			}

			re.setPattern("(" + pattern + ")");
			re.setCaseSensitivity(Qt::CaseInsensitive);

			if (text.contains(re))
			{
				QJsonObject searchResEntry;
				//searchResEntry.insert("title", "unknown");
				QString pageRelativePath = _getPageRelativePathByHtmlFile(pagefilename);
				searchResEntry.insert("path", pageRelativePath);

				QStringList dirnamelist = pageRelativePath.split("/");
				if (dirnamelist.size() > 0)
				{
					searchResEntry.insert("title", dirnamelist[dirnamelist.size()-1]);
				}
				else
				{
					searchResEntry.insert("title", "___");
				}

				searchResult.append(searchResEntry);
			}
		}
	}

	return true;
}