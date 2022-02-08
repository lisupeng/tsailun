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

#include "historymgr.h"
#include "xdiff/src/xdiff_interface.h"
#include "configmgr.h"
#include "versionrecordmgr.h"
#include "crypto.h"
#include <QMutexLocker>
#include <QFile>
#include <QByteArray>
#include <QDir>
#include <QTemporaryFile>
#include <QList>
#include <QDateTime>
#include "utils.h"

bool   HistoryMgr::m_init = false;

static bool addFileToRepository(VersionRecordMgr &recordMgr, const QString &path, const QString &version, 
	                            const QString &sha, const QString &type, const QString &filename, const QString &author)
{
	QString datafile = path + "/" + version;
	QFile file(filename);
	if (!file.copy(datafile))
		return false;

	QDateTime local(QDateTime::currentDateTime());
	QDateTime UTC(local.toUTC());
	QString utcDataTime = UTC.toString("yyyy-MM-dd HH:mm:ss");

	utcDataTime += " UTC";

	return recordMgr.append(version, author, utcDataTime, sha, type);
}

HistoryMgr::HistoryMgr()
{
	if (m_init == false)
	{
		libxdiff_init();
		m_init = true;
	}
}

bool HistoryMgr::diff(QString oldfile, QString newfile, QString patchfile)
{
	QMutexLocker autolock(&m_mutex);

	QByteArray old = QFile::encodeName(oldfile);
	QByteArray newf = QFile::encodeName(newfile);
	QByteArray patch = QFile::encodeName(patchfile);

	if (libxdiff_diff(old.constData(), newf.constData(), patch.constData()) != 0)
		return false;
	else
		return true;
}

bool HistoryMgr::patch(QString oldfile, QString patchfile, QString newfile, QString rejfile)
{
	QMutexLocker autolock(&m_mutex);

	QByteArray old = QFile::encodeName(oldfile);
	QByteArray patch = QFile::encodeName(patchfile);
	QByteArray newf = QFile::encodeName(newfile);
	QByteArray rej = QFile::encodeName(rejfile);
	if (libxdiff_patch(old.constData(), patch.constData(), newf.constData(), rej.constData()) != 0)
		return false;
	else
		return true;
}

bool HistoryMgr::addVersion(const QString &uid, const QString &oldfile, const QString &newfile, const QString &author)
{
	QString dirForUid = getDirForUid(uid);
	QString sha = Crypto::genFileSHA(newfile);
	if (sha.isEmpty())
		return false;

	if (dirForUid == "") // Check if versioned this uid. If not SHA it and add init version
	{
		dirForUid = createDirForUid(uid);

		// add entire newfile
		VersionRecordMgr versionRecord(dirForUid);
		return addFileToRepository(versionRecord, dirForUid, "1", sha, "full", newfile, author);
	}

	VersionRecordMgr versionRecord(dirForUid);

	// build version for next record
	int count = versionRecord.count();
	int previous = count;
	QString sPreviousVer = QString("%1").arg(previous);
	QString version = QString("%1").arg(count+1);

	QString previousSHA = versionRecord.getVersionHash(sPreviousVer);

	// If oldfile isn't previous version, use previous version to compute diff
	QString oldFileSha = Crypto::genFileSHA(oldfile);
	QString orgfile = oldfile;
	QTemporaryFile tmp;
	tmp.open();
	if (oldFileSha != previousSHA)
	{
		// get previousVersion, and use it to compute diff
		if (!getVersion(uid, previous, tmp.fileName()))
			return false;

		orgfile = tmp.fileName();
	}

	// If diff generation fails, save entire file
	QTemporaryFile patchfile;
	patchfile.open();
	patchfile.close();
	if (!diff(orgfile, newfile, patchfile.fileName()))
	{
		return addFileToRepository(versionRecord, dirForUid, version, sha, "full", newfile, author);
	}

	// Make sure isn't the same as previous version by checking if diff is empty and if SHA the same
	if (patchfile.size() == 0)
	{
		if (previousSHA == sha)
			return true; // diff empty and sha the same
		else
		{ // save full file
			return addFileToRepository(versionRecord, dirForUid, version, sha, "full", newfile, author);
		}
	}

	// validate patch file
	if (!validatePatchFile(orgfile, patchfile.fileName(), sha))
	{
		return addFileToRepository(versionRecord, dirForUid, version, sha, "full", newfile, author);
	}

	return addFileToRepository(versionRecord, dirForUid, version, sha, "patch", patchfile.fileName(), author);
}

QString HistoryMgr::getDirForUid(const QString &uid)
{
	ConfigMgr *cfgMgr = ConfigMgr::GetInstance();
	QString appRootPath = cfgMgr->getAppRootDir();
	QString historyDir = appRootPath + "/data/history";

	for (int i = 1; i < 1000; i++)
	{
		QString dirpath = historyDir + "/" + QString("%1").arg(i);
		QDir dir(dirpath);
		if (!(dir.exists()))
			return "";

		QString uidpath = dirpath + "/" + uid;
		if (QDir(uidpath).exists())
			return uidpath;
	}

	return "";
}

QString HistoryMgr::createDirForUid(const QString &uid)
{
	// make sure 'history' exists
	ConfigMgr *cfgMgr = ConfigMgr::GetInstance();
	QString appRootPath = cfgMgr->getAppRootDir();
	QString historyDir = appRootPath + "/data/history";
	if (!QDir(historyDir).exists())
	{
		QDir appDir(appRootPath);
		if (!(appDir.mkdir("history")))
		{
			return "";
		}
	}

	// make sure '1' exists
	QString firstDir = historyDir + "/1";
	if (!QDir(firstDir).exists())
	{
		QDir histDir(historyDir);
		if (!(histDir.mkdir("1")))
		{
			return "";
		}
	}

	// go through <NUM> dirs
	for (int i = 1; i < 1000; i++)
	{
		QString dirpath = historyDir + "/" + QString("%1").arg(i);
		QString uidpath = dirpath + "/" + uid;

		if (QDir(uidpath).exists())
			return uidpath;

		if (QDir(dirpath).count() < 100000)
		{
			// create uid here
			QDir dir(dirpath);
			if (!(dir.mkdir(uid)))
			{
				return "";
			}
			return uidpath;
		}
		else
		{
			QString nextdirpath = historyDir + "/" + QString("%1").arg(i+1);
			if (QDir(nextdirpath).exists())
				continue; // if  <i+1> dir exists, do nothing, continue to loop i+1
			else
			{
				// if <i+1> dir doesn't exist, create <i+1> and continue to loop i+1
				QDir histDir(historyDir);
				if (!(histDir.mkdir(QString("%1").arg(i + 1))))
				{
					return "";
				}
			}
		}
	}
	return "";
}

bool HistoryMgr::validatePatchFile(QString oldfile, QString patchfile, QString sha)
{
	QTemporaryFile newfile;
	QTemporaryFile rejfile;
	newfile.open();
	newfile.close();
	rejfile.open();
	rejfile.close();

	if (!patch(oldfile, patchfile, newfile.fileName(), rejfile.fileName()))
		return false;

	if (rejfile.size() != 0)
		return false;

	QString newFileSha = Crypto::genFileSHA(newfile.fileName());

	if (sha != newFileSha)
		return false;

	return true;
}

bool HistoryMgr::getVersion(const QString &uid, int ver, const QString &outputfile)
{
	QString dirForUid = getDirForUid(uid);

	if (dirForUid == "")
		return false;

	VersionRecordMgr versionRecord(dirForUid);

	QList<QJsonObject> versionList;
	bool foundFullFile = false;

	for (int i = ver; i >= 1; i--)
	{
		QString sver = QString("%1").arg(i);
		QJsonObject jsonobj;

		if (!versionRecord.getRecordByVersion(sver, jsonobj))
			return false;

		versionList.push_front(jsonobj);

		if (jsonobj.value("type").toString() == "full")
		{
			foundFullFile = true;
			break;
		}
	}

	if (!foundFullFile)
		return false;

	// build file and check SHA
	QString previousfile;
	QString patchfile;
	QString newfile;
	QTemporaryFile tmp1;
	QTemporaryFile tmp2;
	QTemporaryFile rej;
	tmp1.open();
	tmp2.open();
	rej.open();

	for (int i = 0; i < versionList.size(); i++)
	{
		QJsonObject jsonobj = versionList[i];
		if (i == 0)
		{
			previousfile = dirForUid + "/" + jsonobj.value("version").toString();
			newfile = tmp1.fileName();
		}
		else
		{
			patchfile = dirForUid + "/" + jsonobj.value("version").toString();

			if (!patch(previousfile, patchfile, newfile, rej.fileName()))
				return false;

			previousfile = newfile;

			if (newfile == tmp1.fileName())
				newfile = tmp2.fileName();
			else
				newfile = tmp1.fileName();
		}
	}

	// check SHA
	QString sha = versionList[versionList.size() - 1].value("sha").toString();
	QString filesha = Crypto::genFileSHA(previousfile);

	if (filesha != sha)
		return false;
	
	//copy previousfile content to outputfile
	return ForceCopy(previousfile, outputfile);
}

bool HistoryMgr::getVersionList(const QString &uid, QJsonArray &list)
{
	QString dirForUid = getDirForUid(uid);

	if (dirForUid == "")
		return false;

	VersionRecordMgr versionRecord(dirForUid);

	return versionRecord.getVersionList(list);
}

