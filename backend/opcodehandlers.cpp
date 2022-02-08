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

#include "opcodehandlers.h"
#include "auth.h"
#include "utils.h"
#include "pagelistmgr.h"
#include "pagemgr.h"
#include "configmgr.h"
#include "syslog.h"
#include "sessionmgr.h"
#include "historymgr.h"
#include "userdbmgr.h"
#include "spacedbmgr.h"
#include "groupdbmgr.h"
#include "search.h"
#include "demomgr.h"
#include "stats.h"
#include "sysdefs.h"
#include <QDir>
#include <QLockFile>
#include <QTemporaryFile>

UserDbMgr        g_userDbMgr;
SpaceDbMgr       g_spaceDbMgr;
GroupDbMgr       g_groupDbMgr;
Syslog           g_syslog;
SessionMgr       g_sessionMgr;
HistoryMgr       g_historyMgr;
SearchMgr        g_searchMgr;
DemoMgr          g_demoMgr;
StatsMgr         g_statsMgr;


OpcodeHandler::OpcodeHandler()
{
	m_handlerMap["listpage"] = &OpcodeHandler::handle_listpage;
	m_handlerMap["del"] = &OpcodeHandler::handle_delpage;
	m_handlerMap["save"] = &OpcodeHandler::handle_savepage;
	m_handlerMap["read"] = &OpcodeHandler::handle_readpage;
	m_handlerMap["create"] = &OpcodeHandler::handle_createpage;
	m_handlerMap["writelog"] = &OpcodeHandler::handle_writelog;
	m_handlerMap["export"] = &OpcodeHandler::handle_exportpage;
	m_handlerMap["search"] = &OpcodeHandler::handle_search;
	m_handlerMap["listuser"] = &OpcodeHandler::handle_listuser;
	m_handlerMap["updateuser"] = &OpcodeHandler::handle_updateuser;
	m_handlerMap["updatecredential"] = &OpcodeHandler::handle_updatecredential;
	m_handlerMap["deluser"] = &OpcodeHandler::handle_deluser;
	m_handlerMap["getuserinfo"] = &OpcodeHandler::handle_getuserinfo;
	m_handlerMap["getspaceinfo"] = &OpcodeHandler::handle_getspaceinfo;
	m_handlerMap["getgroupinfo"] = &OpcodeHandler::handle_getgroupinfo;
	m_handlerMap["delusergroup"] = &OpcodeHandler::handle_delusergroup;
	m_handlerMap["delspace"] = &OpcodeHandler::handle_delspace;
	m_handlerMap["listspace"] = &OpcodeHandler::handle_listspace;
	m_handlerMap["listuserspace"] = &OpcodeHandler::handle_listuserspace;
	m_handlerMap["updatespace"] = &OpcodeHandler::handle_updatespace;
	m_handlerMap["listgroup"] = &OpcodeHandler::handle_listgroup;
	m_handlerMap["updategroup"] = &OpcodeHandler::handle_updategroup;
	m_handlerMap["creategroup"] = &OpcodeHandler::handle_creategroup;
	m_handlerMap["createspace"] = &OpcodeHandler::handle_createspace;
	m_handlerMap["createdemo"] = &OpcodeHandler::handle_createdemo;
	m_handlerMap["dirinfo"] = &OpcodeHandler::handle_dirinfo;
	m_handlerMap["viewfile"] = &OpcodeHandler::handle_viewfile;
	m_handlerMap["listfile"] = &OpcodeHandler::handle_listfile;
	m_handlerMap["upload"] = &OpcodeHandler::handle_upload;
	m_handlerMap["register"] = &OpcodeHandler::handle_register;
	m_handlerMap["listversion"] = &OpcodeHandler::handle_listversion;
	m_handlerMap["restore"] = &OpcodeHandler::handle_restore;
	m_handlerMap["checkpermission"] = &OpcodeHandler::handle_checkpermission;
	m_handlerMap["sysver"] = &OpcodeHandler::handle_sysver;
	m_handlerMap["watchdog"] = &OpcodeHandler::handle_watchdog;
	m_handlerMap["getstats"] = &OpcodeHandler::handle_getstats;
	m_handlerMap["getlog"] = &OpcodeHandler::handle_getlog;
	m_handlerMap["moveup"] = &OpcodeHandler::handle_moveup;
	m_handlerMap["movedown"] = &OpcodeHandler::handle_movedown;
	m_handlerMap["rename"] = &OpcodeHandler::handle_renamepage;
	m_handlerMap["test"] = &OpcodeHandler::handle_test;
}

static QString getSpaceFullPathByUrl(QString url)
{
	static QString appRootPath = "";

	if (appRootPath == "")
	{
		ConfigMgr *cfgMgr = ConfigMgr::GetInstance();
		appRootPath = cfgMgr->getAppRootDir();
	}

	QString spaceFullPath = appRootPath + "/data" + getSpacePathByUrl(url) + "/pages";

	return spaceFullPath;
}

bool generateDirnameForPage(QString path, QString title, QString &pageDirname)
{
	QString rawTitle = title;
	// 1. max length 64
	rawTitle.truncate(64);

	// reserve "upload"
	if (rawTitle == "upload")
		rawTitle = "upload_";

	// 2. replace special characters to "_"
	pageDirname = rawTitle.replace(QRegExp("[\\\\\\/\\:\\*\\?\\\"\\<\\>\\.\\ ]"), "_");

	// 3. Check if page already exists
	bool hasPageListData = false;
	PagelistMgr listMgr;
	QString listFilePath = PathNameConcat(path, QString("pagelist.json"));

	// pagelist.json doesn't exist if no page created in the dir yet
	if (QFile(listFilePath).exists())
	{
		if (!listMgr.read(listFilePath))
		{
			g_syslog.logMessage(SYSLOG_LEVEL_ERROR, "", "Unable to read listFile.");

			return false;
		}
		else
		{
			hasPageListData = true;
		}
	}

	// title already exists, shouldn't create page with the same title, return false
	if (listMgr.pageTitleExists(title))
	{
		return false;
	}

	// 4. Due to special characters filtering, different page titles may have the same page dir. Append _<NUM> to 
	//    make a unique dir
	QString fullPageDir;
	fullPageDir = PathNameConcat(path, pageDirname);

	if (QDir(fullPageDir).exists())
	{
		for (int i = 1; i < 100000; i++)
		{
			QString sNum = QString("_%1").arg(i);
			if (!QDir(fullPageDir + sNum).exists())
			{
				pageDirname = pageDirname + sNum;
				return true;
			}

		}

		return false;
	}

	return true;
}

static bool move_dir_to_trash(QDir &dir)
{
	// make sure trash dir exists
	ConfigMgr *cfgMgr = ConfigMgr::GetInstance();
	QString _dataDir = cfgMgr->getAppRootDir() + "/data";
	QString trashDir = _dataDir + "/trash";
	if (!QDir(trashDir).exists())
	{
		QDir dataDir(_dataDir);
		if (!(dataDir.mkdir("trash")))
		{
			return false;
		}
	}

	// find a dir name that doesn't exists already
	QString dirname = dir.dirName();
	QString targetDirName = "";

	if (!QDir(trashDir + "/" + dirname).exists())
	{
		targetDirName = dirname;
	}
	else
	{
		for (int i = 0; i < 10000; i++)
		{
			QString _tmpTarget = dirname + QString("%1").arg(i);

			if (!QDir(trashDir + "/" + _tmpTarget).exists())
			{
				targetDirName = _tmpTarget;
				break;
			}
		}
	}

	// copy to trash dir
	if (targetDirName.size() == 0)
		return false;

	QDir(trashDir).mkdir(targetDirName);

	copyPath(dir.absolutePath(), trashDir + "/" + targetDirName);

	// remove
	if (!dir.removeRecursively())
	{
		return false;
	}

	return true;
}

void _handle_page_create(QString path, QString title, QString loc, QString curPageDir, QString &output)
{
	// Lock list file dir
	QString dirLockfileName = PathNameConcat(path, QString(".lock"));
	QLockFile listDirLock(dirLockfileName);
	if (!listDirLock.lock())
	{
		g_syslog.logMessage(SYSLOG_LEVEL_ERROR, "", "Unable to lock file.");
		return;
	}

	// generate page dir name by page title
	QString pageDir = title;
	if (!generateDirnameForPage(path, title, pageDir))
	{
		g_syslog.logMessage(SYSLOG_LEVEL_ERROR, "", "Page already exists.");
		output = "<html></html>";
		return;
	}

	QDir dir(path);
	if (!dir.exists())
	{
		g_syslog.logMessage(SYSLOG_LEVEL_ERROR, "", "File doesn't exist.");
		return;
	}

	// If pageId dir doesn't exist, create it, in case already created by another thread
	QString pagePath = PathNameConcat(path, pageDir);

	// if already exists, should fail.
	if (QDir(pagePath).exists())
	{
		g_syslog.logMessage(SYSLOG_LEVEL_ERROR, "", "Page already exists.");
		output = "<html></html>";
		return;
	}

	// create dir for page
	if (!(dir.mkdir(pageDir)))
	{
		g_syslog.logMessage(SYSLOG_LEVEL_ERROR, "", "Unable create dir.");
		return;
	}

	QString indexFilepath = PathNameConcat(pagePath, QString("index.html"));

	// lock index.html.lock
	QLockFile indexFileLock(indexFilepath + ".lock");
	if (!indexFileLock.tryLock(20000))
	{
		g_syslog.logMessage(SYSLOG_LEVEL_ERROR, "", "Unable lock index file.");
		return;
	}

	// create index.html file and set title( should not exist already)
	if (QFile(indexFilepath).exists())
	{
		g_syslog.logMessage(SYSLOG_LEVEL_ERROR, "", "Index file doesn't exist.");
		return;
	}
	Page page;
	page.setTitle(title);
	// create uid file
	QString uid = CreateUid();
	if (uid == "")
	{
		g_syslog.logMessage(SYSLOG_LEVEL_ERROR, "", "Unable create uid.");
		return;
	}
	page.setUid(uid);
	page.savePage(indexFilepath);

	QString listFilepath = PathNameConcat(path, QString("pagelist.json"));

	// lock list.lock file
	QLockFile listFileLock(listFilepath + ".lock");
	if (!listFileLock.tryLock(20000))
	{
		g_syslog.logMessage(SYSLOG_LEVEL_ERROR, "", "Unable lock list file.");
		return;
	}

	// create list file if doesn't exist already
	PagelistMgr listMgr;
	if (!QFile(listFilepath).exists())
	{
		// create list file
		listMgr.save(listFilepath);
	}
	else
	{
		if (!listMgr.read(listFilepath))
		{
			g_syslog.logMessage(SYSLOG_LEVEL_ERROR, "", "Unable to read list file.");
			return;
		}
	}

	// write pagedir(pageId) to list file
	PageInfo pageinfo;
	pageinfo.title = title;
	pageinfo.dir = pageDir;

	if (curPageDir.size() > 0)
	{
		int idx = listMgr.getIndex(curPageDir);
		if (idx == -1)
		{
			listMgr.append(pageinfo);
		}
		else
		{
			if (loc == "before")
			{
				if (idx > 0)
					listMgr.insert(idx, pageinfo);
				else
					listMgr.insert(0, pageinfo);
			}
			else if (loc == "after")
			{
				if (idx < listMgr.count() - 1)
					listMgr.insert(idx + 1, pageinfo);
				else
					listMgr.append(pageinfo);
			}
			else
			{
				listMgr.append(pageinfo);
			}
		}
	}
	else
	{
		listMgr.append(pageinfo);
	}

	listMgr.save(listFilepath);

	QJsonObject jsonobj;
	jsonobj.insert("status", "ok");
	jsonobj.insert("pagedir", pageDir);
	jsonobj.insert("uid", uid);

	output = QString(QJsonDocument(jsonobj).toJson(QJsonDocument::Compact));

	//output = QString("{\"status\":\"ok\", \"pagedir\":\"") + pageDir + "\"}";
	//output = "<html>ok</html>";

	// unlock index file
	indexFileLock.unlock();

	// unlock list file
	listFileLock.unlock();

	// unlock dir
	listDirLock.unlock();
}

bool _ListChildPage(QString path, QJsonArray &jsonarray)
{
	QString listFilePath = PathNameConcat(path, QString("pagelist.json"));

	// Should not fail here, doesn't exist if no sub page
	if (!QFile(listFilePath).exists())
		return true;

	PagelistMgr listMgr;
	if (!listMgr.read(listFilePath))
		return false; // data corrupt ?

	int count = listMgr.count();
	for (int i = 0; i < count; i++)
	{
		PageInfo info;
		listMgr.getPageInfo(i, info);
		QString pagedir = info.dir;
		QString title = info.title;

		// check if has child by checking if has 'pagedir/pagelist.json' file
		QString childLevelPagelistFile = PathNameConcat(path, pagedir) + "/pagelist.json";
		bool isLeaf;
		if (!QFile(childLevelPagelistFile).exists())
		{
			isLeaf = true;
		}
		else
		{
			PagelistMgr childListMgr;
			childListMgr.read(childLevelPagelistFile);
			if (childListMgr.count() > 0)
				isLeaf = false;
			else
				isLeaf = true;
		}

		Page childPage;
		if (childPage.readFromFile(PathNameConcat(path, pagedir) + "/index.html") == false)
			continue;

		//QString uid = UidMgr::ReadUidFromPath(PathNameConcat(path, pagedir));
		QString uid = childPage.getUid();
		if (uid == "")
			continue;

		// ztree format
		// QString nodeinfo = QString("{\"id\":\"") + pagedir + "\",\"name\":\"" + title + "\"}";

		// antd tree format
		QString nodeinfo = QString("{\"key\":\"") + uid + "\",\"pagedir\":\"" + pagedir + "\",\"title\":\"" + title + "\",\"isLeaf\":" + isLeaf + "}";

		QJsonObject jsonobj;
		jsonobj.insert("key", uid);
		jsonobj.insert("pagedir", pagedir);
		jsonobj.insert("title", title);
		jsonobj.insert("isLeaf", isLeaf);

		jsonarray.append(jsonobj);
	}

	return true;
}

// node and it's children
void handleGetDirInfo(QString path, QJsonObject &result)
{
	Page page;
	if (page.readFromFile(PathNameConcat(path, QString("index.html"))) == false)
	{
		// error
		result.insert("status", "error");
		return;
	}

	QString uid = page.getUid();
	if (uid == "")
	{
		result.insert("status", "error");
		return;
	}

	QJsonObject nodeobj;
	nodeobj.insert("key", uid);
	nodeobj.insert("title", page.getTitle());

	// dir
	QStringList pathlist = path.split("/");
	if (pathlist.size() == 0)
	{
		result.insert("status", "error");
		return;
	}
	QString pagedir = pathlist.at(pathlist.size() - 1);

	nodeobj.insert("pagedir", pagedir);

	// children
	QJsonArray jsonarray;
	if (_ListChildPage(path, jsonarray) == false)
	{
		result.insert("status", "error");
		return;
	}

	if (!jsonarray.isEmpty())
	{
		nodeobj.insert("children", jsonarray);
		nodeobj.insert("isLeaf", false);
	}
	else
		nodeobj.insert("isLeaf", true);

	result.insert("status", "ok");
	result.insert("node", nodeobj);
}

void handle_page_save(const QString &path, QString content, const QString &sid, QJsonObject &result)
{
	// Lock file
	QString indexFilepath = PathNameConcat(path, QString("index.html"));
	QLockFile indexFileLock(indexFilepath + ".lock");

	if (!indexFileLock.lock())
	{
		g_syslog.logMessage(SYSLOG_LEVEL_ERROR, "", "Unable lock index file.");
		return;
	}

	Page page;
	if (!page.readFromFile(indexFilepath))
	{
		g_syslog.logMessage(SYSLOG_LEVEL_ERROR, "", "Unable read index file.");
		return;
	}

	// set content
	if (!page.setContent(content))
	{
		g_syslog.logMessage(SYSLOG_LEVEL_ERROR, "", "Unable setContent.");
		return;
	}

	// save old version file
	QTemporaryFile old;
	old.open();
	old.close();
	if (!ForceCopy(indexFilepath, old.fileName()))
	{
		g_syslog.logMessage(SYSLOG_LEVEL_ERROR, "", "ForceCopy failed.");
		return;
	}

	if (!page.savePage(indexFilepath))
	{
		g_syslog.logMessage(SYSLOG_LEVEL_ERROR, "", "Unable restore old version.");
		return;
	}

	QString author = "";
	if (sid == "")
		author = "guest";
	else
	{
		QJsonObject sessionobj;
		if (g_sessionMgr.getSession(sid, sessionobj))
		{
			if (sessionobj.contains("account"))
				author = sessionobj.value("account").toString();
			/*
			else if (sessionobj.contains("email"))
			{
			// Actually sessionobj should always contain 'account'. Not likely hit here
			author = sessionobj.value("email").toString();
			}
			*/
		}
	}

	// File crash on English Win10 OS, due to Chinese characters in path
	// TODO need improvement here to avoid copy workaround
	QTemporaryFile newTmpFile;
	newTmpFile.open();
	newTmpFile.close();
	if (!ForceCopy(indexFilepath, newTmpFile.fileName()))
	{
		g_syslog.logMessage(SYSLOG_LEVEL_ERROR, "", "ForceCopy failed.");
		return;
	}

	// save version to history
	if (!g_historyMgr.addVersion(page.getUid(), old.fileName(), newTmpFile.fileName(), author))
	{
		g_syslog.logMessage(SYSLOG_LEVEL_ERROR, "", "addVersion failed.");
	}

	// set status:ok
	result.insert("status", "ok");
}

void handle_page_read(const QString &path, const QString &ver, bool keepBodyTag, QJsonObject &result)
{
	QString indexFilepath = PathNameConcat(path, QString("index.html"));

	Page currentPage;
	if (!currentPage.readFromFile(indexFilepath))
	{
		result.insert("status", "error");
		return;
	}

	Page *page;

	Page verPage;

	// TODO-5 this value inputed from web, should check it
	if (ver.size() > 0)
	{
		QTemporaryFile tmp;
		tmp.open();
		if (!g_historyMgr.getVersion(currentPage.getUid(), ver.toInt(), tmp.fileName()))
		{
			result.insert("status", "error");
			return;
		}

		if (!verPage.readFromFile(tmp.fileName()))
		{
			result.insert("status", "error");
			return;
		}
		page = &verPage;
	}
	else
		page = &currentPage;

	// get creator, last modified date
	QString sUid = page->getUid();
	QJsonArray list;
	if (g_historyMgr.getVersionList(sUid, list))
	{
		QString author;
		QString modifiedBy;
		QString modifiedDate;
		QString createdDate;

		if (list.size() > 0)
		{
			author = list[0].toObject().value("author").toString();
			createdDate = list[0].toObject().value("date").toString();
		}

		int curVerIndex = 0;

		// TODO show modified info of latest version instead of current displayed version ?
		if (list.size() > 0)
			curVerIndex = list.size() - 1;

		if (curVerIndex <= list.size() - 1)
		{
			modifiedBy = list[curVerIndex].toObject().value("author").toString();
			modifiedDate = list[curVerIndex].toObject().value("date").toString();
		}

		if (author.size() > 0)
		{
			result.insert("author", author);
			QJsonObject userInfo;
			if (g_userDbMgr.getUserInfoByAccount(author, userInfo))
			{
				QString author_email = userInfo.value("email").toString();
				QString author_fullname = userInfo.value("fullname").toString();

				if (author_email.size() > 0)
					result.insert("author_email", author_email);

				if (author_fullname.size() > 0)
					result.insert("author_fullname", author_fullname);
			}
		}

		if (modifiedBy.size() > 0)
		{
			result.insert("last_by", modifiedBy);
			QJsonObject userInfo;
			if (g_userDbMgr.getUserInfoByAccount(modifiedBy, userInfo))
			{
				QString lastby_email = userInfo.value("email").toString();
				QString lasyby_fullname = userInfo.value("fullname").toString();

				if (lastby_email.size() > 0)
					result.insert("lastby_email", lastby_email);

				if (lasyby_fullname.size() > 0)
					result.insert("lasyby_fullname", lasyby_fullname);
			}
		}

		if (createdDate.size() > 0)
		{
			result.insert("created_date", createdDate);
		}

		if (modifiedDate.size() > 0)
		{
			result.insert("last_modified_date", modifiedDate);
		}
	}

	// read page content
	QString body = page->getContent();
	if (body.size() == 0)
	{
		result.insert("status", "error");
		return;
	}

	if (keepBodyTag == false)
	{
		// remove <body> and </body>, TODO optimize performance of entire handle_page_read()
		body.remove(QRegExp("<\\/body>|<body>"));
	}

	result.insert("status", "ok");
	result.insert("title", page->getTitle());
	result.insert("body", body);
}

void handle_page_del(const QString &path, QJsonObject &result)
{
	QDir dir(path);
	QString dirname = dir.dirName();

	if (!move_dir_to_trash(dir))
	{
		result.insert("status", "fail");
		return;
	}

	// remove entry from pagelist.json
	dir.cdUp(); // To parent level

	QString listFilePath = PathNameConcat(dir.absolutePath(), QString("pagelist.json"));

	// lock list.lock file
	QLockFile listFileLock(listFilePath + ".lock");
	if (!listFileLock.tryLock(20000))
	{
		return;
	}

	PagelistMgr listMgr;
	if (!listMgr.read(listFilePath))
	{
		// should return ok even can't operate pagelist.json
		// because dir remove successfully
		result.insert("status", "ok");
		return;
	}
	listMgr.remove(dirname);
	listMgr.save(listFilePath);

	result.insert("status", "ok");
}

void handle_page_export(const QString &path, QJsonObject &result)
{
	QString fullFilePath = path + "/index.html";

	if (!QFile(fullFilePath).exists())
	{
		result.insert("status", "fail");
		return;
	}

	QFile file(fullFilePath);
	if (file.open(QIODevice::ReadOnly))
	{
		//result.insert("data", QString::fromUtf8(file.readAll().toBase64()));

		QString content = QString::fromUtf8(file.readAll());
		result.insert("data", content);
		result.insert("status", "ok");
	}
	else
	{
		result.insert("status", "fail");
	}
}



void handleListPages(QString path, QJsonObject &result)
{
	QJsonArray jsonarray;
	if (_ListChildPage(path, jsonarray) == false)
	{
		result.insert("status", "error");
		return;
	}

	result.insert("status", "ok");
	result.insert("list", jsonarray);
}

static bool _verify_username(const QString &email, const QString &name)
{
	if (email == "*")
		return false;

	if (name == "*")
		return false;

	return true;
}

void handleSignUp(QJsonObject &postobj, QJsonObject &result)
{
	QString email = postobj.value("email").toString();
	QString fullname = postobj.value("fullname").toString();

	QString credential = postobj.value("credential").toString();

	QString err;

	if (!_verify_username(email, fullname))
	{
		result.insert("status", "fail");
		return;
	}

	if (g_userDbMgr.createNewUser(email, fullname, credential, err) == false)
	{
		// insert err
		if (err.size() > 0)
		{
			result.insert("errcode", err);
		}
		result.insert("status", "fail");

		return;
	}

	result.insert("status", "ok");
}

void _handle_listversion(const QString &path, QJsonObject &result)
{
	QString indexFilepath = PathNameConcat(path, QString("index.html"));

	Page currentPage;
	if (!currentPage.readFromFile(indexFilepath))
	{
		result.insert("status", "error");
		return;
	}

	QJsonArray list;
	if (!g_historyMgr.getVersionList(currentPage.getUid(), list))
	{
		result.insert("status", "error");
		return;
	}

	result.insert("status", "ok");
	result.insert("list", list);
}

void _handle_restore(const QString &path, const QString &ver, const QString &sid, QJsonObject &result)
{
	// get content of specified page
	QJsonObject readRes;
	handle_page_read(path, ver, true, readRes);

	if (readRes.value("status").toString() != "ok")
	{
		result.insert("status", "error");
		return;
	}

	QString content = readRes.value("body").toString();

	// call handle_page_save
	handle_page_save(path, content, sid, result);
}

void _handle_page_moveup(const QString &path, QJsonObject &result)
{
	QDir dir(path);
	QString dirname = dir.dirName();
	dir.cdUp(); // To parent level

	QString listFilePath = PathNameConcat(dir.absolutePath(), QString("pagelist.json"));

	// pagelist.json doesn't exist if no page created in the dir yet
	if (!QFile(listFilePath).exists())
	{
		result.insert("status", "error");
		return;

	}

	// lock list.lock file
	QLockFile listFileLock(listFilePath + ".lock");
	if (!listFileLock.tryLock(20000))
	{
		result.insert("status", "error");
		return;
	}

	PagelistMgr listMgr;
	if (!listMgr.read(listFilePath))
	{
		result.insert("status", "error");
		return;
	}

	listMgr.moveup(dirname);
	listMgr.save(listFilePath);

	result.insert("status", "ok");
}

void _handle_page_movedown(const QString &path, QJsonObject &result)
{
	QDir dir(path);
	QString dirname = dir.dirName();
	dir.cdUp(); // To parent level

	QString listFilePath = PathNameConcat(dir.absolutePath(), QString("pagelist.json"));

	// pagelist.json doesn't exist if no page created in the dir yet
	if (!QFile(listFilePath).exists())
	{
		result.insert("status", "error");
		return;

	}

	// lock list.lock file
	QLockFile listFileLock(listFilePath + ".lock");
	if (!listFileLock.tryLock(20000))
	{
		result.insert("status", "error");
		return;
	}

	PagelistMgr listMgr;
	if (!listMgr.read(listFilePath))
	{
		result.insert("status", "error");
		return;
	}

	listMgr.movedown(dirname);
	listMgr.save(listFilePath);

	result.insert("status", "ok");
}

void _handle_page_rename(const QString &path, const QString &title, QJsonObject &result)
{
	// TODO DON'T rename directory. because could be in using by other people

	QDir dir(path);
	QString dirname = dir.dirName();

	QDir parentdir(path);

	// rename title in pagelist.json
	parentdir.cdUp(); // To parent level

	QString listFilePath = PathNameConcat(parentdir.absolutePath(), QString("pagelist.json"));

	// lock list.lock file
	QLockFile listFileLock(listFilePath + ".lock");
	if (!listFileLock.tryLock(20000))
	{
		return;
	}

	PagelistMgr listMgr;
	if (!listMgr.read(listFilePath))
	{
		result.insert("status", "fail");
		return;
	}

	QString oldtitle = listMgr.getTitleByDirname(dirname);

	// rename title in pagelist.json
	if (!listMgr.rename(oldtitle, title))
	{
		result.insert("status", "fail");
		return;
	}
	if (!listMgr.save(listFilePath))
	{
		result.insert("status", "fail");
		return;
	}

	// modify title in index.html
	// Lock file
	QString indexFilepath = PathNameConcat(path, QString("index.html"));
	QLockFile indexFileLock(indexFilepath + ".lock");

	if (!indexFileLock.lock())
	{
		return;
	}

	Page page;
	if (!page.readFromFile(indexFilepath))
	{
		return;
	}
	page.setTitle(title);
	if (!page.savePage(indexFilepath))
	{
		return;
	}

	result.insert("status", "ok");
}

bool OpcodeHandler::handle_opcode(const QString &op, CWF::Request &req, CWF::Response &response, REQ_CONTEXT &ctx)
{
	std::map<std::string, PHANDLER>::iterator it;
	it = m_handlerMap.find(op.toStdString());

	if (it == m_handlerMap.end())
		return false;

	PHANDLER handler = it->second;

	(this->*handler)(req, response, ctx);

	return true;
}

void OpcodeHandler::handle_listpage(CWF::Request &req, CWF::Response &response, REQ_CONTEXT &ctx)
{
	QByteArray __url_ = req.getHttpParser().getUrl();
	QString url = QString::fromUtf8(QByteArray::fromPercentEncoding(__url_));

	QJsonObject result;
	int authres = AuthMgr::SpaceReadPermissionCheck(*(ctx.sid), url);

	if (authres == AUTH_RET_INVALID_SESSION)
	{
		result.insert("status", "fail");
		result.insert("errcode", "invalid_session");

		QString out = QString(QJsonDocument(result).toJson(QJsonDocument::Compact));

		response.write(out.toUtf8());
		return;
	}

	if (authres != AUTH_RET_SUCCESS)
	{
		result.insert("status", "fail");
		result.insert("errcode", "access_denied");

		QString out = QString(QJsonDocument(result).toJson(QJsonDocument::Compact));

		response.write(out.toUtf8());
	}

	QString path = getSpaceAndPagePathByUrl(url);

	handleListPages(path, result);

	QString out = QString(QJsonDocument(result).toJson(QJsonDocument::Compact));

	response.write(out.toUtf8());
}

void OpcodeHandler::handle_delpage(CWF::Request &req, CWF::Response &response, REQ_CONTEXT &ctx)
{
	QByteArray __url_ = req.getHttpParser().getUrl();
	QString url = QString::fromUtf8(QByteArray::fromPercentEncoding(__url_));

	QJsonObject result;
	QString path = getSpaceAndPagePathByUrl(url);

	// check permission
	int authres = AuthMgr::SpaceWritePermissionCheck(*(ctx.sid), url);

	if (authres == AUTH_RET_INVALID_SESSION)
	{
		result.insert("status", "fail");
		result.insert("errcode", "invalid_session");

		QString out = QString(QJsonDocument(result).toJson(QJsonDocument::Compact));

		response.write(out.toUtf8());
		return;
	}

	if (authres != AUTH_RET_SUCCESS)
	{
		result.insert("status", "fail");
		result.insert("errcode", "access_denied");

		QString out = QString(QJsonDocument(result).toJson(QJsonDocument::Compact));

		response.write(out.toUtf8());
	}

	handle_page_del(path, result);
	QString out = QString(QJsonDocument(result).toJson(QJsonDocument::Compact));

	response.write(out.toUtf8());
}

void OpcodeHandler::handle_savepage(CWF::Request &req, CWF::Response &response, REQ_CONTEXT &ctx)
{
	QByteArray __url_ = req.getHttpParser().getUrl();
	QString url = QString::fromUtf8(QByteArray::fromPercentEncoding(__url_));

	QJsonObject result;
	// check permission
	int authres = AuthMgr::SpaceWritePermissionCheck(*(ctx.sid), url);

	if (authres == AUTH_RET_INVALID_SESSION)
	{
		result.insert("status", "fail");
		result.insert("errcode", "invalid_session");

		QString out = QString(QJsonDocument(result).toJson(QJsonDocument::Compact));

		response.write(out.toUtf8());
		return;
	}

	if (authres != AUTH_RET_SUCCESS)
	{
		result.insert("status", "fail");
		result.insert("errcode", "access_denied");

		QString out = QString(QJsonDocument(result).toJson(QJsonDocument::Compact));

		response.write(out.toUtf8());
	}

	//QByteArray content = req.getHttpParser().getBody();
	QString scontent = (*(ctx.postobj)).value("content").toString();//QString::fromUtf8(content);
	QString path = getSpaceAndPagePathByUrl(url);
	handle_page_save(path, scontent, *(ctx.sid), result);

	QString out = QString(QJsonDocument(result).toJson(QJsonDocument::Compact));

	response.write(out.toUtf8());

	return;
}

void OpcodeHandler::handle_readpage(CWF::Request &req, CWF::Response &response, REQ_CONTEXT &ctx)
{
	QByteArray __url_ = req.getHttpParser().getUrl();
	QString url = QString::fromUtf8(QByteArray::fromPercentEncoding(__url_));

	QJsonObject result;

	int authres = AuthMgr::SpaceReadPermissionCheck(*(ctx.sid), url);

	if (authres == AUTH_RET_INVALID_SESSION)
	{
		result.insert("status", "fail");
		result.insert("errcode", "invalid_session");

		QString out = QString(QJsonDocument(result).toJson(QJsonDocument::Compact));

		response.write(out.toUtf8());
		return;
	}

	if (authres != AUTH_RET_SUCCESS)
	{
		result.insert("status", "fail");
		result.insert("errcode", "access_denied");

		QString out = QString(QJsonDocument(result).toJson(QJsonDocument::Compact));

		response.write(out.toUtf8());
	}

	QByteArray ver = req.getHttpParser().getParameter("ver");
	QString path = getSpaceAndPagePathByUrl(url);


	handle_page_read(path, ver, false, result);
	QString out = QString(QJsonDocument(result).toJson(QJsonDocument::Compact));

	response.write(out.toUtf8());
}

void OpcodeHandler::handle_createpage(CWF::Request &req, CWF::Response &response, REQ_CONTEXT &ctx)
{
	QByteArray __url_ = req.getHttpParser().getUrl();
	QString url = QString::fromUtf8(QByteArray::fromPercentEncoding(__url_));

	QJsonObject result;
	// check permission
	int authres = AuthMgr::SpaceWritePermissionCheck(*(ctx.sid), url);

	if (authres == AUTH_RET_INVALID_SESSION)
	{
		result.insert("status", "fail");
		result.insert("errcode", "invalid_session");

		QString out = QString(QJsonDocument(result).toJson(QJsonDocument::Compact));

		response.write(out.toUtf8());
		return;
	}

	if (authres != AUTH_RET_SUCCESS)
	{
		result.insert("status", "fail");
		result.insert("errcode", "access_denied");

		QString out = QString(QJsonDocument(result).toJson(QJsonDocument::Compact));

		response.write(out.toUtf8());
	}

	QByteArray title = req.getHttpParser().getParameter("title", true, false);
	QString stitle = QString::fromUtf8(QByteArray::fromPercentEncoding(title));
	if (stitle.length() == 0)
	{
		g_syslog.logMessage(SYSLOG_LEVEL_ERROR, "", "Page title shouldn't be 0.");
		return;
	}
	QString path = getSpaceAndPagePathByUrl(url);

	QByteArray loc = req.getHttpParser().getParameter("loc");
	QString sloc = QString::fromUtf8(QByteArray::fromPercentEncoding(loc));

	QDir parentPath;
	if (sloc.size())
	{
		if (sloc == "before" || sloc == "after")
		{
			parentPath = QDir(path);
			parentPath.cdUp();
		}
		else if (sloc == "root")
		{
			QString _spacePath = getSpaceFullPathByUrl(url);
			parentPath = QDir(_spacePath);
		}
		else if (sloc == "child")
		{
			parentPath = QDir(path);
		}
	}
	else
	{
		parentPath = QDir(path);
	}

	QString sParentPath = parentPath.absolutePath();

	QString currentPagedir = QDir(url).dirName();

	QString out = "";
	_handle_page_create(sParentPath, stitle, sloc, currentPagedir, out);

	response.write(out.toUtf8());

	return;
}

void OpcodeHandler::handle_writelog(CWF::Request &req, CWF::Response &response, REQ_CONTEXT &ctx)
{
	QJsonObject result;

	QString log = (*(ctx.postobj)).value("log").toString();

	g_syslog.logMessage(SYSLOG_LEVEL_INFO, "web", log);

	result.insert("status", "ok");

	QString out = QString(QJsonDocument(result).toJson(QJsonDocument::Compact));

	response.write(out.toUtf8());
	return;
}

void OpcodeHandler::handle_exportpage(CWF::Request &req, CWF::Response &response, REQ_CONTEXT &ctx)
{
	QByteArray __url_ = req.getHttpParser().getUrl();
	QString url = QString::fromUtf8(QByteArray::fromPercentEncoding(__url_));

	QJsonObject result;
	QString path = getSpaceAndPagePathByUrl(url);

	// check permission
	int authres = AuthMgr::SpaceReadPermissionCheck(*(ctx.sid), url);

	if (authres == AUTH_RET_INVALID_SESSION)
	{
		result.insert("status", "fail");
		result.insert("errcode", "invalid_session");

		QString out = QString(QJsonDocument(result).toJson(QJsonDocument::Compact));

		response.write(out.toUtf8());
		return;
	}

	if (authres != AUTH_RET_SUCCESS)
	{
		result.insert("status", "fail");
		result.insert("errcode", "access_denied");

		QString out = QString(QJsonDocument(result).toJson(QJsonDocument::Compact));

		response.write(out.toUtf8());
	}

	handle_page_export(path, result);

	QString out = QString(QJsonDocument(result).toJson(QJsonDocument::Compact));

	response.write(out.toUtf8());
}

void OpcodeHandler::handle_search(CWF::Request &req, CWF::Response &response, REQ_CONTEXT &ctx)
{
	QJsonObject result;

	QByteArray searchinput = req.getHttpParser().getParameter("s", true, false);
	QString sSearchInput = QString::fromUtf8(QByteArray::fromPercentEncoding(searchinput));

	QJsonArray searchResult;
	if (g_searchMgr.searchText(*(ctx.sid), sSearchInput, searchResult))
		//if (g_searchMgr.searchTitle(sid, sSearchInput, searchResult))
	{
		result.insert("status", "ok");
		result.insert("searchres", searchResult);
	}
	else
	{
		result.insert("status", "fail");
	}

	QString out = QString(QJsonDocument(result).toJson(QJsonDocument::Compact));

	response.write(out.toUtf8());
}

void OpcodeHandler::handle_listuser(CWF::Request &req, CWF::Response &response, REQ_CONTEXT &ctx)
{
	QJsonObject result;

	int authres = AuthMgr::AdminPermissionCheck(*(ctx.sid));

	if (authres == AUTH_RET_INVALID_SESSION)
	{
		result.insert("status", "fail");
		result.insert("errcode", "invalid_session");

		QString out = QString(QJsonDocument(result).toJson(QJsonDocument::Compact));

		response.write(out.toUtf8());
		return;
	}

	if (authres != AUTH_RET_SUCCESS)
	{
		result.insert("status", "fail");
		result.insert("errcode", "access_denied");

		QString out = QString(QJsonDocument(result).toJson(QJsonDocument::Compact));

		response.write(out.toUtf8());
	}

	//handleListPages(path, result);
	QJsonArray list;
	if (g_userDbMgr.getUserList(list))
	{
		result.insert("list", list);
		result.insert("status", "ok");
	}
	else
	{
		result.insert("status", "error");
	}

	QString out = QString(QJsonDocument(result).toJson(QJsonDocument::Compact));

	response.write(out.toUtf8());
}

void OpcodeHandler::handle_updateuser(CWF::Request &req, CWF::Response &response, REQ_CONTEXT &ctx)
{
	QJsonObject result;

	// permission check
	int authres = AuthMgr::AdminPermissionCheck(*(ctx.sid));

	if (authres == AUTH_RET_INVALID_SESSION)
	{
		result.insert("status", "fail");
		result.insert("errcode", "invalid_session");

		QString out = QString(QJsonDocument(result).toJson(QJsonDocument::Compact));

		response.write(out.toUtf8());
		return;
	}

	if (authres != AUTH_RET_SUCCESS)
	{
		result.insert("status", "fail");
		result.insert("errcode", "access_denied");

		QString out = QString(QJsonDocument(result).toJson(QJsonDocument::Compact));

		response.write(out.toUtf8());
	}

	if ((*(ctx.postobj)).contains("account"))
	{
		QString account = (*(ctx.postobj)).value("account").toString();

		QString fullname, email, role, space;

		if ((*(ctx.postobj)).contains("fullname"))
			fullname = (*(ctx.postobj)).value("fullname").toString();
		else
			fullname = "";

		if ((*(ctx.postobj)).contains("email"))
			email = (*(ctx.postobj)).value("email").toString();
		else
			email = "";

		if ((*(ctx.postobj)).contains("role"))
			role = (*(ctx.postobj)).value("role").toString();
		else
			role = "";

		if ((*(ctx.postobj)).contains("space"))
			space = (*(ctx.postobj)).value("space").toString();
		else
			space = "";

		if (g_userDbMgr.updateUser(account, fullname, email, role, space))
			result.insert("status", "ok");
		else
			result.insert("status", "error");
	}
	else
	{
		result.insert("status", "error");
	}

	QString out = QString(QJsonDocument(result).toJson(QJsonDocument::Compact));

	response.write(out.toUtf8());
}

void OpcodeHandler::handle_updatecredential(CWF::Request &req, CWF::Response &response, REQ_CONTEXT &ctx)
{
	QJsonObject result;

	QString account;
	g_sessionMgr.getAccountBySid(*(ctx.sid), account);
	QString oldcredential = (*(ctx.postobj)).value("oldcredential").toString();
	QString credential = (*(ctx.postobj)).value("credential").toString();

	// updateCredential
	if (g_userDbMgr.updateCredential(account, oldcredential, credential))
	{
		result.insert("status", "ok");
	}
	else
	{
		result.insert("status", "error");
	}

	QString out = QString(QJsonDocument(result).toJson(QJsonDocument::Compact));

	response.write(out.toUtf8());
}

void OpcodeHandler::handle_deluser(CWF::Request &req, CWF::Response &response, REQ_CONTEXT &ctx)
{
	QJsonObject result;
	QByteArray account = req.getHttpParser().getParameter("account", true, false);
	QString sAccount = QString::fromUtf8(QByteArray::fromPercentEncoding(account));

	if (sAccount != "admin" && sAccount != "guest" && g_userDbMgr.deleteUser(sAccount))
	{
		result.insert("status", "ok");
	}
	else
	{
		result.insert("status", "error");
	}

	QString out = QString(QJsonDocument(result).toJson(QJsonDocument::Compact));

	response.write(out.toUtf8());
}

void OpcodeHandler::handle_getuserinfo(CWF::Request &req, CWF::Response &response, REQ_CONTEXT &ctx)
{
	QJsonObject result;

	int authres = AuthMgr::AdminPermissionCheck(*(ctx.sid));

	if (authres == AUTH_RET_INVALID_SESSION)
	{
		result.insert("status", "fail");
		result.insert("errcode", "invalid_session");

		QString out = QString(QJsonDocument(result).toJson(QJsonDocument::Compact));

		response.write(out.toUtf8());
		return;
	}

	if (authres != AUTH_RET_SUCCESS)
	{
		result.insert("status", "fail");
		result.insert("errcode", "access_denied");

		QString out = QString(QJsonDocument(result).toJson(QJsonDocument::Compact));

		response.write(out.toUtf8());
	}

	// TODO-6 input param check

	QByteArray account = req.getHttpParser().getParameter("account", true, false);
	QString sAccount = QString::fromUtf8(QByteArray::fromPercentEncoding(account));

	QJsonObject userInfo;
	g_userDbMgr.getUserInfoByAccount(account, userInfo);

	if (!userInfo.isEmpty())
	{
		result.insert("status", "ok");
		result.insert("userinfo", userInfo);
	}
	else
	{
		result.insert("status", "error");
	}

	QString out = QString(QJsonDocument(result).toJson(QJsonDocument::Compact));

	response.write(out.toUtf8());

	return;
}

void OpcodeHandler::handle_getspaceinfo(CWF::Request &req, CWF::Response &response, REQ_CONTEXT &ctx)
{
	QJsonObject result;

	int authres = AuthMgr::AdminPermissionCheck(*(ctx.sid));

	if (authres == AUTH_RET_INVALID_SESSION)
	{
		result.insert("status", "fail");
		result.insert("errcode", "invalid_session");

		QString out = QString(QJsonDocument(result).toJson(QJsonDocument::Compact));

		response.write(out.toUtf8());
		return;
	}

	if (authres != AUTH_RET_SUCCESS)
	{
		result.insert("status", "fail");
		result.insert("errcode", "access_denied");

		QString out = QString(QJsonDocument(result).toJson(QJsonDocument::Compact));

		response.write(out.toUtf8());
	}

	// TODO-6 input param check

	QByteArray spacename = req.getHttpParser().getParameter("space", true, false);
	QString sSpaceName = QString::fromUtf8(QByteArray::fromPercentEncoding(spacename));

	QJsonObject spaceDetail;
	if (g_spaceDbMgr.getSpaceDetail(sSpaceName, spaceDetail))
	{
		result.insert("spaceinfo", spaceDetail);
		result.insert("status", "ok");
	}
	else
	{
		result.insert("status", "error");
	}

	QString out = QString(QJsonDocument(result).toJson(QJsonDocument::Compact));

	response.write(out.toUtf8());

	return;
}

void OpcodeHandler::handle_getgroupinfo(CWF::Request &req, CWF::Response &response, REQ_CONTEXT &ctx)
{
	QJsonObject result;

	int authres = AuthMgr::AdminPermissionCheck(*(ctx.sid));

	if (authres == AUTH_RET_INVALID_SESSION)
	{
		result.insert("status", "fail");
		result.insert("errcode", "invalid_session");

		QString out = QString(QJsonDocument(result).toJson(QJsonDocument::Compact));

		response.write(out.toUtf8());
		return;
	}

	if (authres != AUTH_RET_SUCCESS)
	{
		result.insert("status", "fail");
		result.insert("errcode", "access_denied");

		QString out = QString(QJsonDocument(result).toJson(QJsonDocument::Compact));

		response.write(out.toUtf8());
	}

	// TODO-6 input param check

	QByteArray groupname = req.getHttpParser().getParameter("group", true, false);
	QString sGroupName = QString::fromUtf8(QByteArray::fromPercentEncoding(groupname));

	QJsonObject groupInfo;

	if (g_groupDbMgr.getGroupInfo(sGroupName, groupInfo))
	{
		result.insert("groupinfo", groupInfo);
		result.insert("status", "ok");
	}
	else
	{
		result.insert("status", "error");
	}

	QString out = QString(QJsonDocument(result).toJson(QJsonDocument::Compact));

	response.write(out.toUtf8());

	return;
}

void OpcodeHandler::handle_delusergroup(CWF::Request &req, CWF::Response &response, REQ_CONTEXT &ctx)
{
	QJsonObject result;
	QByteArray name = req.getHttpParser().getParameter("name", true, false);
	QString groupname = QString::fromUtf8(QByteArray::fromPercentEncoding(name));

	if (g_groupDbMgr.delGroup(groupname))
	{
		result.insert("status", "ok");
	}
	else
	{
		result.insert("status", "error");
	}

	QString out = QString(QJsonDocument(result).toJson(QJsonDocument::Compact));

	response.write(out.toUtf8());

	return;

}

void OpcodeHandler::handle_delspace(CWF::Request &req, CWF::Response &response, REQ_CONTEXT &ctx)
{
	QJsonObject result;
	QByteArray name = req.getHttpParser().getParameter("name", true, false);
	QString spacename = QString::fromUtf8(QByteArray::fromPercentEncoding(name));

	if (g_spaceDbMgr.deleteSpace(spacename))
	{
		result.insert("status", "ok");
	}
	else
	{
		result.insert("status", "error");
	}

	QString out = QString(QJsonDocument(result).toJson(QJsonDocument::Compact));

	response.write(out.toUtf8());

	return;
}

void OpcodeHandler::handle_listspace(CWF::Request &req, CWF::Response &response, REQ_CONTEXT &ctx)
{
	QJsonObject result;

	int authres = AuthMgr::AdminPermissionCheck(*(ctx.sid));

	if (authres == AUTH_RET_INVALID_SESSION)
	{
		result.insert("status", "fail");
		result.insert("errcode", "invalid_session");

		QString out = QString(QJsonDocument(result).toJson(QJsonDocument::Compact));

		response.write(out.toUtf8());
		return;
	}

	if (authres != AUTH_RET_SUCCESS)
	{
		result.insert("status", "fail");
		result.insert("errcode", "access_denied");

		QString out = QString(QJsonDocument(result).toJson(QJsonDocument::Compact));

		response.write(out.toUtf8());
	}

	QJsonArray list;
	if (g_spaceDbMgr.getSpaceList(list))
	{
		result.insert("list", list);
		result.insert("status", "ok");
	}
	else
	{
		result.insert("status", "error");
	}

	QString out = QString(QJsonDocument(result).toJson(QJsonDocument::Compact));

	response.write(out.toUtf8());

	return;
}

void OpcodeHandler::handle_listuserspace(CWF::Request &req, CWF::Response &response, REQ_CONTEXT &ctx)
{
	QJsonObject result;

	// list space
	QJsonArray list;
	if (!g_spaceDbMgr.getSpaceList(list))
	{
		result.insert("status", "error");

		QString out = QString(QJsonDocument(result).toJson(QJsonDocument::Compact));
		response.write(out.toUtf8());
		return;
	}

	// get user readable space list
	QJsonArray readableList;

	for (int i = 0; i < list.size(); i++)
	{
		QJsonObject obj = list.at(i).toObject();

		QString spacename = obj.value("spacename").toString();

		int authres = AuthMgr::SpaceReadPermissionCheckBySpaceName(*(ctx.sid), spacename);

		if (authres == AUTH_RET_SUCCESS)
		{
			obj.remove("rcount");
			obj.remove("wcount");
			readableList.append(obj);
		}
	}

	result.insert("list", readableList);

	result.insert("status", "ok");

	QString out = QString(QJsonDocument(result).toJson(QJsonDocument::Compact));
	response.write(out.toUtf8());

	return;
}

void OpcodeHandler::handle_updatespace(CWF::Request &req, CWF::Response &response, REQ_CONTEXT &ctx)
{
	QJsonObject result;

	int authres = AuthMgr::AdminPermissionCheck(*(ctx.sid));

	if (authres == AUTH_RET_INVALID_SESSION)
	{
		result.insert("status", "fail");
		result.insert("errcode", "invalid_session");

		QString out = QString(QJsonDocument(result).toJson(QJsonDocument::Compact));

		response.write(out.toUtf8());
		return;
	}

	if (authres != AUTH_RET_SUCCESS)
	{
		result.insert("status", "fail");
		result.insert("errcode", "access_denied");

		QString out = QString(QJsonDocument(result).toJson(QJsonDocument::Compact));

		response.write(out.toUtf8());
	}

	QJsonArray rlist;
	QJsonArray wlist;
	QString spaceName;

	if ((*(ctx.postobj)).contains("spacename") && (*(ctx.postobj)).contains("rlist") && (*(ctx.postobj)).contains("wlist"))
	{
		spaceName = (*(ctx.postobj)).value("spacename").toString();
		rlist = (*(ctx.postobj)).value("rlist").toArray();
		wlist = (*(ctx.postobj)).value("wlist").toArray();

		if (g_spaceDbMgr.updateSpace(spaceName, rlist, wlist))
			result.insert("status", "ok");
		else
			result.insert("status", "error");
	}
	else
	{
		result.insert("status", "error");
	}

	QString out = QString(QJsonDocument(result).toJson(QJsonDocument::Compact));


	response.write(out.toUtf8());

	return;
}

void OpcodeHandler::handle_listgroup(CWF::Request &req, CWF::Response &response, REQ_CONTEXT &ctx)
{
	QJsonObject result;

	int authres = AuthMgr::AdminPermissionCheck(*(ctx.sid));

	if (authres == AUTH_RET_INVALID_SESSION)
	{
		result.insert("status", "fail");
		result.insert("errcode", "invalid_session");

		QString out = QString(QJsonDocument(result).toJson(QJsonDocument::Compact));

		response.write(out.toUtf8());
		return;
	}

	if (authres != AUTH_RET_SUCCESS)
	{
		result.insert("status", "fail");
		result.insert("errcode", "access_denied");

		QString out = QString(QJsonDocument(result).toJson(QJsonDocument::Compact));

		response.write(out.toUtf8());
	}

	QJsonArray list;

	if (g_groupDbMgr.getGroupList(list))
	{
		result.insert("list", list);
		result.insert("status", "ok");
	}
	else
	{
		result.insert("status", "error");
	}

	QString out = QString(QJsonDocument(result).toJson(QJsonDocument::Compact));

	response.write(out.toUtf8());

	return;
}

void OpcodeHandler::handle_updategroup(CWF::Request &req, CWF::Response &response, REQ_CONTEXT &ctx)
{
	QJsonObject result;

	int authres = AuthMgr::AdminPermissionCheck(*(ctx.sid));

	if (authres == AUTH_RET_INVALID_SESSION)
	{
		result.insert("status", "fail");
		result.insert("errcode", "invalid_session");

		QString out = QString(QJsonDocument(result).toJson(QJsonDocument::Compact));

		response.write(out.toUtf8());
		return;
	}

	if (authres != AUTH_RET_SUCCESS)
	{
		result.insert("status", "fail");
		result.insert("errcode", "access_denied");

		QString out = QString(QJsonDocument(result).toJson(QJsonDocument::Compact));

		response.write(out.toUtf8());
	}

	QJsonArray userList;
	QString groupname;
	QString comment;

	if ((*(ctx.postobj)).contains("group") && (*(ctx.postobj)).contains("userlist"))
	{
		groupname = (*(ctx.postobj)).value("group").toString();
		userList = (*(ctx.postobj)).value("userlist").toArray();
		comment = (*(ctx.postobj)).value("comment").toString();

		if (g_groupDbMgr.updateGroup(groupname, comment, userList))
			result.insert("status", "ok");
		else
			result.insert("status", "error");
	}
	else
	{
		result.insert("status", "error");
	}

	QString out = QString(QJsonDocument(result).toJson(QJsonDocument::Compact));

	response.write(out.toUtf8());

	return;
}

void OpcodeHandler::handle_creategroup(CWF::Request &req, CWF::Response &response, REQ_CONTEXT &ctx)
{
	QJsonObject result;

	int authres = AuthMgr::AdminPermissionCheck(*(ctx.sid));

	if (authres == AUTH_RET_INVALID_SESSION)
	{
		result.insert("status", "fail");
		result.insert("errcode", "invalid_session");

		QString out = QString(QJsonDocument(result).toJson(QJsonDocument::Compact));

		response.write(out.toUtf8());
		return;
	}

	if (authres != AUTH_RET_SUCCESS)
	{
		result.insert("status", "fail");
		result.insert("errcode", "access_denied");

		QString out = QString(QJsonDocument(result).toJson(QJsonDocument::Compact));

		response.write(out.toUtf8());
	}

	QByteArray name = req.getHttpParser().getParameter("name", true, false);
	QString groupName = QString::fromUtf8(QByteArray::fromPercentEncoding(name));

	QByteArray bcomment = req.getHttpParser().getParameter("comment", true, false);
	QString comment = QString::fromUtf8(QByteArray::fromPercentEncoding(bcomment));

	// TODO-5 input param check

	if (g_groupDbMgr.addGroup(groupName, comment))
	{
		result.insert("status", "ok");
	}
	else
	{
		result.insert("status", "error");
	}

	QString out = QString(QJsonDocument(result).toJson(QJsonDocument::Compact));

	response.write(out.toUtf8());

	return;
}

void OpcodeHandler::handle_createspace(CWF::Request &req, CWF::Response &response, REQ_CONTEXT &ctx)
{
	QJsonObject result;

	int authres = AuthMgr::AdminPermissionCheck(*(ctx.sid));

	if (authres == AUTH_RET_INVALID_SESSION)
	{
		result.insert("status", "fail");
		result.insert("errcode", "invalid_session");

		QString out = QString(QJsonDocument(result).toJson(QJsonDocument::Compact));
		response.write(out.toUtf8());
		return;
	}

	if (authres != AUTH_RET_SUCCESS)
	{
		result.insert("status", "fail");
		result.insert("errcode", "access_denied");

		QString out = QString(QJsonDocument(result).toJson(QJsonDocument::Compact));
		response.write(out.toUtf8());
	}

	QByteArray name = req.getHttpParser().getParameter("name", true, false);
	QString spaceName = QString::fromUtf8(QByteArray::fromPercentEncoding(name));
	QByteArray path = req.getHttpParser().getParameter("path", true, false);
	QString spacePath = QString::fromUtf8(QByteArray::fromPercentEncoding(path));

	QString spacePathNormalized;
	if (!checkSpacePathInput(spacePath, spacePathNormalized))
	{
		result.insert("status", "error");

		QString out = QString(QJsonDocument(result).toJson(QJsonDocument::Compact));

		response.write(out.toUtf8());
	}

	if (g_spaceDbMgr.addSpace(spaceName, spacePathNormalized))
	{
		result.insert("status", "ok");
	}
	else
	{
		result.insert("status", "error");
	}

	g_spaceDbMgr.addToSpaceRlist("spaceName", "user", "*");
	g_spaceDbMgr.addToSpaceWlist("spaceName", "user", "*");

	QString out = QString(QJsonDocument(result).toJson(QJsonDocument::Compact));

	response.write(out.toUtf8());

	return;
}

void OpcodeHandler::handle_createdemo(CWF::Request &req, CWF::Response &response, REQ_CONTEXT &ctx)
{
	QJsonObject result;

	g_demoMgr.handle_create_demo_instance(result);

	QString out = QString(QJsonDocument(result).toJson(QJsonDocument::Compact));

	response.write(out.toUtf8());

	return;
}

void OpcodeHandler::handle_dirinfo(CWF::Request &req, CWF::Response &response, REQ_CONTEXT &ctx)
{
	QByteArray __url_ = req.getHttpParser().getUrl();
	QString url = QString::fromUtf8(QByteArray::fromPercentEncoding(__url_));

	QString path = getSpaceAndPagePathByUrl(url);

	QJsonObject result;
	handleGetDirInfo(path, result);

	QString out = QString(QJsonDocument(result).toJson(QJsonDocument::Compact));

	response.write(out.toUtf8());

	return;
}

void OpcodeHandler::handle_viewfile(CWF::Request &req, CWF::Response &response, REQ_CONTEXT &ctx)
{
	QByteArray __url_ = req.getHttpParser().getUrl();
	QString url = QString::fromUtf8(QByteArray::fromPercentEncoding(__url_));

	// TODO-3 check permission

	QByteArray _name = req.getHttpParser().getParameter("name", true, false);
	QString name = QString::fromUtf8(QByteArray::fromPercentEncoding(_name));

	QString pagepath = getSpaceAndPagePathByUrl(url);

	QString fullFilePath = pagepath + "/upload/" + name;

	if (!QFile(fullFilePath).exists())
	{
		g_syslog.logMessage(SYSLOG_LEVEL_ERROR, "", "File already exists.");
		return;
	}

	QFile file(fullFilePath);
	if (file.open(QIODevice::ReadOnly))
	{
		response.addHeader("content-disposition", ("inline; filename=" + name.toUtf8()));
		response.write(file.readAll());
	}
	else
	{
		response.sendError(0, "Unable to read file");
	}

	return;
}

void OpcodeHandler::handle_listfile(CWF::Request &req, CWF::Response &response, REQ_CONTEXT &ctx)
{
	QByteArray __url_ = req.getHttpParser().getUrl();
	QString url = QString::fromUtf8(QByteArray::fromPercentEncoding(__url_));

	QJsonObject result;

	// TODO check permission

	QString pagepath = getSpaceAndPagePathByUrl(url);

	// if "upload" dir doesn't exist, create it
	QString uploadDir = pagepath + "/upload";

	QDir dir(uploadDir);
	if (!dir.exists())
	{
		return;
	}

	QJsonArray list;

	QFileInfoList fileInfoList = dir.entryInfoList(QDir::Files, QDir::Time | QDir::Reversed);
	for (int i = 0; i < fileInfoList.size(); i++)
	{
		QFileInfo info = fileInfoList[i];

		if (info.isFile())
		{
			QString name = info.fileName();
			qint64 size = info.size();
			QDateTime date = info.created();

			QDateTime UTC(date.toUTC());
			QString utcDataTime = UTC.toString("yyyy-MM-dd HH:mm:ss");

			utcDataTime += " UTC";

			QJsonObject obj;
			obj.insert("file", name);
			obj.insert("size", size);
			obj.insert("date", utcDataTime);

			list.append(obj);
		}
	}

	result.insert("list", list);
	result.insert("status", "ok");

	QString out = QString(QJsonDocument(result).toJson(QJsonDocument::Compact));
	response.write(out.toUtf8());
	return;
}

void OpcodeHandler::handle_upload(CWF::Request &req, CWF::Response &response, REQ_CONTEXT &ctx)
{
	QByteArray __url_ = req.getHttpParser().getUrl();
	QString url = QString::fromUtf8(QByteArray::fromPercentEncoding(__url_));

	QJsonObject result;

	// check permission
	int authres = AuthMgr::SpaceWritePermissionCheck(*(ctx.sid), url);

	if (authres == AUTH_RET_INVALID_SESSION)
	{
		result.insert("status", "fail");
		result.insert("errcode", "invalid_session");

		QString out = QString(QJsonDocument(result).toJson(QJsonDocument::Compact));
		response.write(out.toUtf8());
		return;
	}

	if (authres != AUTH_RET_SUCCESS)
	{
		result.insert("status", "fail");
		result.insert("errcode", "access_denied");

		QString out = QString(QJsonDocument(result).toJson(QJsonDocument::Compact));
		response.write(out.toUtf8());
	}

	if (!(*(ctx.postobj)).contains("filedata"))
	{
		result.insert("status", "fail");

		QString out = QString(QJsonDocument(result).toJson(QJsonDocument::Compact));
		response.write(out.toUtf8());
		return;
	}

	QByteArray _filename = req.getHttpParser().getParameter("filename", true, false);
	QString filename = QString::fromUtf8(QByteArray::fromPercentEncoding(_filename));

	QString filedata = (*(ctx.postobj)).value("filedata").toString();
	int size = (*(ctx.postobj)).value("size").toInt();

	QByteArray filebytes = QByteArray::fromBase64(filedata.toLatin1());

	QString pagepath = getSpaceAndPagePathByUrl(url);

	// if "upload" dir doesn't exist, create it
	QString uploadDir = pagepath + "/upload";
	if (!QDir(uploadDir).exists())
	{
		QDir pageDir(pagepath);
		if (!(pageDir.mkdir("upload")))
		{
			g_syslog.logMessage(SYSLOG_LEVEL_ERROR, "", "Unable create 'upload' dir.");
			result.insert("status", "fail");
			return;
		}
	}

	// TODO-7 if file already exists, append a <NUM> to filename
	QString fullfileName = uploadDir + "/" + filename;

	if (QFile(fullfileName).exists())
	{
		// TODO
		return;
	}

	// TODO write
	QFile fd(fullfileName);

	if (!fd.open(QFile::WriteOnly))
	{
		g_syslog.logMessage(SYSLOG_LEVEL_ERROR, "", "Failed to open file.");
		result.insert("status", "fail");
		return;
	}

	fd.write(filebytes);
	fd.close();

	result.insert("status", "ok");

	QString out = QString(QJsonDocument(result).toJson(QJsonDocument::Compact));
	response.write(out.toUtf8());
	return;
}

void OpcodeHandler::handle_register(CWF::Request &req, CWF::Response &response, REQ_CONTEXT &ctx)
{
	QJsonObject result;

	handleSignUp((*(ctx.postobj)), result);

	QString out = QString(QJsonDocument(result).toJson(QJsonDocument::Compact));

	response.write(out.toUtf8());
	return;
}

void OpcodeHandler::handle_listversion(CWF::Request &req, CWF::Response &response, REQ_CONTEXT &ctx)
{
	QByteArray __url_ = req.getHttpParser().getUrl();
	QString url = QString::fromUtf8(QByteArray::fromPercentEncoding(__url_));

	QJsonObject result;
	QString path = getSpaceAndPagePathByUrl(url);

	_handle_listversion(path, result);
	QString out = QString(QJsonDocument(result).toJson(QJsonDocument::Compact));

	response.write(out.toUtf8());
	return;
}

void OpcodeHandler::handle_restore(CWF::Request &req, CWF::Response &response, REQ_CONTEXT &ctx)
{
	QByteArray __url_ = req.getHttpParser().getUrl();
	QString url = QString::fromUtf8(QByteArray::fromPercentEncoding(__url_));

	QJsonObject result;
	QString path = getSpaceAndPagePathByUrl(url);
	QByteArray ver = req.getHttpParser().getParameter("ver");

	_handle_restore(path, ver, *(ctx.sid), result);
	QString out = QString(QJsonDocument(result).toJson(QJsonDocument::Compact));

	response.write(out.toUtf8());
	return;
}

void OpcodeHandler::handle_checkpermission(CWF::Request &req, CWF::Response &response, REQ_CONTEXT &ctx)
{
	QByteArray __url_ = req.getHttpParser().getUrl();
	QString url = QString::fromUtf8(QByteArray::fromPercentEncoding(__url_));

	QJsonObject result;

	QByteArray access = req.getHttpParser().getParameter("access");
	QString sAccess = QString::fromUtf8(QByteArray::fromPercentEncoding(access));

	int authres = AUTH_RET_ACCESS_DENIED;

	if (sAccess == "writespace")
	{
		authres = AuthMgr::SpaceWritePermissionCheck(*(ctx.sid), url);
	}
	else if (sAccess == "sysadmin")
	{
		authres = AuthMgr::AdminPermissionCheck(*(ctx.sid));
	}

	if (authres == AUTH_RET_INVALID_SESSION)
	{
		result.insert("status", "fail");
		result.insert("errcode", "invalid_session");

		QString out = QString(QJsonDocument(result).toJson(QJsonDocument::Compact));
		response.write(out.toUtf8());
		return;
	}
	else if (authres == AUTH_RET_SUCCESS)
	{
		result.insert("status", "ok");

		QString out = QString(QJsonDocument(result).toJson(QJsonDocument::Compact));
		response.write(out.toUtf8());
	}
	else
	{
		result.insert("status", "fail");
		result.insert("errcode", "access_denied");

		QString out = QString(QJsonDocument(result).toJson(QJsonDocument::Compact));
		response.write(out.toUtf8());
	}

	QString out = QString(QJsonDocument(result).toJson(QJsonDocument::Compact));

	response.write(out.toUtf8());
	return;
}

void OpcodeHandler::handle_sysver(CWF::Request &req, CWF::Response &response, REQ_CONTEXT &ctx)
{
	QJsonObject result;

	QString ver = QString("TSAILUN ") + QString(__SYS_VERSION_);

	result.insert("sysver", ver);

	result.insert("status", "ok");

	QString out = QString(QJsonDocument(result).toJson(QJsonDocument::Compact));
	response.write(out.toUtf8());
	return;
}

void OpcodeHandler::handle_watchdog(CWF::Request &req, CWF::Response &response, REQ_CONTEXT &ctx)
{
	QJsonObject result;

	result.insert("status", "ok");

	QString out = QString(QJsonDocument(result).toJson(QJsonDocument::Compact));
	response.write(out.toUtf8());
	return;
}

void OpcodeHandler::handle_getstats(CWF::Request &req, CWF::Response &response, REQ_CONTEXT &ctx)
{
	QJsonObject result;

	result.insert("ReqCounter", QString("%1").arg(g_statsMgr.getRequestStatsCounter()));
	result.insert("SiteReqCounter", QString("%1").arg(g_statsMgr.getSiteRequestStatsCounter()));

	result.insert("status", "ok");

	QString out = QString(QJsonDocument(result).toJson(QJsonDocument::Compact));
	response.write(out.toUtf8());
	return;
}

void OpcodeHandler::handle_getlog(CWF::Request &req, CWF::Response &response, REQ_CONTEXT &ctx)
{
	QJsonObject result;
	QString log;

	if (g_syslog.getAllMessages(log))
	{
		result.insert("status", "ok");
		result.insert("log", log);
	}
	else
	{
		result.insert("status", "fail");
	}

	QString out = QString(QJsonDocument(result).toJson(QJsonDocument::Compact));

	response.write(out.toUtf8());
	return;
}

void OpcodeHandler::handle_moveup(CWF::Request &req, CWF::Response &response, REQ_CONTEXT &ctx)
{
	QByteArray __url_ = req.getHttpParser().getUrl();
	QString url = QString::fromUtf8(QByteArray::fromPercentEncoding(__url_));

	QJsonObject result;
	QString path = getSpaceAndPagePathByUrl(url);

	_handle_page_moveup(path, result);
	QString out = QString(QJsonDocument(result).toJson(QJsonDocument::Compact));

	response.write(out.toUtf8());
	return;
}

void OpcodeHandler::handle_movedown(CWF::Request &req, CWF::Response &response, REQ_CONTEXT &ctx)
{
	QByteArray __url_ = req.getHttpParser().getUrl();
	QString url = QString::fromUtf8(QByteArray::fromPercentEncoding(__url_));

	QJsonObject result;
	QString path = getSpaceAndPagePathByUrl(url);

	_handle_page_movedown(path, result);
	QString out = QString(QJsonDocument(result).toJson(QJsonDocument::Compact));

	response.write(out.toUtf8());
	return;
}

void OpcodeHandler::handle_renamepage(CWF::Request &req, CWF::Response &response, REQ_CONTEXT &ctx)
{
	QByteArray __url_ = req.getHttpParser().getUrl();
	QString url = QString::fromUtf8(QByteArray::fromPercentEncoding(__url_));

	QJsonObject result;
	QString path = getSpaceAndPagePathByUrl(url);

	// check permission
	int authres = AuthMgr::SpaceWritePermissionCheck(*(ctx.sid), url);

	if (authres == AUTH_RET_INVALID_SESSION)
	{
		result.insert("status", "fail");
		result.insert("errcode", "invalid_session");

		QString out = QString(QJsonDocument(result).toJson(QJsonDocument::Compact));
		response.write(out.toUtf8());
		return;
	}

	if (authres != AUTH_RET_SUCCESS)
	{
		result.insert("status", "fail");
		result.insert("errcode", "access_denied");

		QString out = QString(QJsonDocument(result).toJson(QJsonDocument::Compact));
		response.write(out.toUtf8());
	}

	QByteArray title = req.getHttpParser().getParameter("title", true, false);
	QString stitle = QString::fromUtf8(QByteArray::fromPercentEncoding(title));

	_handle_page_rename(path, stitle, result);
	QString out = QString(QJsonDocument(result).toJson(QJsonDocument::Compact));

	response.write(out.toUtf8());
	return;
}

void OpcodeHandler::handle_test(CWF::Request &req, CWF::Response &response, REQ_CONTEXT &ctx)
{
	QJsonObject result;

	sendMail();

	result.insert("status", "ok");

	QString out = QString(QJsonDocument(result).toJson(QJsonDocument::Compact));

	response.write(out.toUtf8());
	return;
}