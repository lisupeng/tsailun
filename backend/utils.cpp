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
#include <QJsonObject>
#include "utils.h"
#include "configmgr.h"
#include "SMTPEmail/src/SmtpMime"
#include "sessionmgr.h"

extern SessionMgr    g_sessionMgr;

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

bool sendMail()
{
	// First we need to create an SmtpClient object
	// We will use the Gmail's smtp server (smtp.gmail.com, port 465, ssl)

	//SmtpClient smtp("smtp.qq.com", 465, SmtpClient::SslConnection);
	SmtpClient smtp("192.168.5.125", 25, SmtpClient::TcpConnection);

	// We need to set the username (your email address) and password
	// for smtp authentification.


	//smtp.setUser("11954794@qq.com");
	//smtp.setPassword("yourpwd");

	// Now we create a MimeMessage object. This is the email.

	MimeMessage message;

	EmailAddress sender("test@test.com");
	message.setSender(&sender);

	EmailAddress to("11954794@qq.com");
	message.addRecipient(&to);

	message.setSubject("sendmail test");

	// Now add some text to the email.
	// First we create a MimeText object.

	MimeText text;

	text.setText("Hi,\nThis is a simple email message.\n");

	// Now add it to the mail

	message.addPart(&text);

	// Now we can send the mail

	if (!smtp.connectToHost()) {
		//qDebug() << "Failed to connect to host!" << endl;
		return false;
	}

	//if (!smtp.login()) {
	//	//qDebug() << "Failed to login!" << endl;
	//	return false;
	//}

	if (!smtp.sendMail(message)) {
		//qDebug() << "Failed to send mail!" << endl;
		return false;
	}

	smtp.quit();
	return true;
}

void buildContentType(const QString &name, QString &contentType, QString &disposition)
{
	// TODO use parser tree to improve performance if necessary
	if (name.endsWith(".jpg", Qt::CaseInsensitive))
	{
		contentType = "image/jpeg";
		disposition = "inline; filename=" + name.toUtf8();
	}
	else if (name.endsWith(".jpeg", Qt::CaseInsensitive))
	{
		contentType = "image/jpeg";
		disposition = "inline; filename=" + name.toUtf8();
	}
	else if (name.endsWith(".jfif", Qt::CaseInsensitive))
	{
		contentType = "image/jpeg";
		disposition = "inline; filename=" + name.toUtf8();
	}
	else if (name.endsWith(".png", Qt::CaseInsensitive))
	{
		contentType = "image/png";
		disposition = "inline; filename=" + name.toUtf8();
	}
	else if (name.endsWith(".gif", Qt::CaseInsensitive))
	{
		contentType = "image/gif";
		disposition = "inline; filename=" + name.toUtf8();
	}
	else if (name.endsWith(".mp3", Qt::CaseInsensitive))
	{
		contentType = "audio/mp3";
		disposition = "inline; filename=" + name.toUtf8();
	}
	else if (name.endsWith(".wav", Qt::CaseInsensitive))
	{
		contentType = "audio/wav";
		disposition = "inline; filename=" + name.toUtf8();
	}
	else if (name.endsWith(".mp4", Qt::CaseInsensitive))
	{
		contentType = "video/mpeg4";
		disposition = "inline; filename=" + name.toUtf8();
	}
	else if (name.endsWith(".pdf", Qt::CaseInsensitive))
	{
		contentType = "application/pdf";
		disposition = "inline; filename=" + name.toUtf8();
	}
	else if (name.endsWith(".html", Qt::CaseInsensitive))
	{
		contentType = "text/html";
		disposition = "inline; filename=" + name.toUtf8();
	}
	else if (name.endsWith(".htm", Qt::CaseInsensitive))
	{
		contentType = "text/htm";
		disposition = "inline; filename=" + name.toUtf8();
	}
	else if (name.endsWith(".txt", Qt::CaseInsensitive))
	{
		contentType = "text/plain";
		disposition = "inline; filename=" + name.toUtf8();
	}
	else if (name.endsWith(".svg", Qt::CaseInsensitive))
	{
		contentType = "text/xml";
		disposition = "inline; filename=" + name.toUtf8();
	}
	else
	{
		contentType = "application/octet-stream";
		disposition = "attachment; filename=" + name.toUtf8();
	}
}

QString getUserAccountBySessionId(const QString &sid)
{
	QString account = "";
	if (sid == "")
		account = "guest";
	else
	{
		QJsonObject sessionobj;
		if (g_sessionMgr.getSession(sid, sessionobj))
		{
			if (sessionobj.contains("account"))
				account = sessionobj.value("account").toString();
		}
	}

	return account;
}