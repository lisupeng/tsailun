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

#include <cwf/cppwebapplication.h>
#include <QString>
#include <QDir>
#include <QLockFile>
#include <QJsonObject>
#include <QCryptographicHash>
#include <QThread>
#include <QTemporaryFile>
#include <map>
#include <string>
#include "configmgr.h"
#include "pagelistmgr.h"
#include "pagemgr.h"
#include "utils.h"
#include "uidmgr.h"
#include "userdbmgr.h"
#include "spacedbmgr.h"
#include "sessionmgr.h"
#include "auth.h"
#include "syslog.h"
#include "serverapp.h"
#include "demomgr.h"
#include "stats.h"
#include "sysdefs.h"

#include "opcodehandlers.h"

extern UserDbMgr        g_userDbMgr;
extern SpaceDbMgr       g_spaceDbMgr;
extern SessionMgr       g_sessionMgr;
extern Syslog           g_syslog;
extern StatsMgr         g_statsMgr;
OpcodeHandler           g_opcodeHandler;


static bool getPostData(CWF::Request &req, QJsonObject &postobj)
{
	QByteArray body = req.getHttpParser().getBody();

	if (body.isNull() || body.size() == 0)
		return false;

	QJsonParseError error;
	QJsonDocument jsonDoc = QJsonDocument::fromJson(body, &error);

	postobj = jsonDoc.object();

	return true;
}

void _handleSignIn(QString data, QJsonObject &result)
{
	QJsonParseError error;
	QJsonDocument jsonDoc = QJsonDocument::fromJson(data.toUtf8(), &error);

	QJsonObject jsonObj = jsonDoc.object();

	QString account = jsonObj.value("account").toString();
	QString credential = jsonObj.value("credential").toString();

	QJsonObject userRecord;

	userRecord = g_userDbMgr.getUserInfoByName(account);

	// Can't find user info in db
	if (userRecord.isEmpty())
	{
		result.insert("status", "fail");
		result.insert("errcode", "username_or_pwd_incorrect");
		QThread::msleep(100); // sleep incase enumeration attack
		return;
	}

	// Password incorrect
	if (userRecord.value("credential").toString() != credential)
	{
		result.insert("status", "fail");
		result.insert("errcode", "username_or_pwd_incorrect");
		QThread::msleep(100); // sleep incase enumeration attack
		return;
	}

	// add session to sessionmgr
	QString sid = CreateUid();
	QJsonObject sessionObj;

	if (!(userRecord.value("account").isNull()))
		sessionObj.insert("account", userRecord.value("account"));
	if (!(userRecord.value("email").isNull()))
		sessionObj.insert("email", userRecord.value("email"));
	if (!(userRecord.value("role").isNull()))
		sessionObj.insert("role", userRecord.value("role"));
	if (!(userRecord.value("fullname").isNull()))
		sessionObj.insert("fullname", userRecord.value("fullname"));

	sessionObj.insert("sid", sid);

	// get user space and also save to sessionObj
	QString spaceName = userRecord.value("space").toString();

	bool setspace = false;
	if (spaceName.size() > 0)
	{
		QJsonObject spaceDetail;
		if (g_spaceDbMgr.getSpaceDetail(spaceName, spaceDetail))
		{
			QString spacePath = spaceDetail.value("path").toString();

			if (spacePath.size() > 0)
			{
				result.insert("space", spacePath);
				sessionObj.insert("default_spacepath", spacePath);

				setspace = true;
			}
		}
	}

	if (!setspace)
	{
		result.insert("space", "/spaces/default");
		sessionObj.insert("space", "/spaces/default");
	}

	g_sessionMgr.addSession(sid, sessionObj);

	QString sessionStr = QString(QJsonDocument(sessionObj).toJson(QJsonDocument::Compact));
	result.insert("session", sessionStr);
	result.insert("status", "ok");
}

static bool access_upload_file(const QString &url)
{
	QStringList list = url.split("/");
	int n = list.size();

	QString upload = list.at(n - 2);
	if (upload != "upload")
		return false;
	else
		return true;
}


class TslController : public CWF::Controller
{
public:
	void doGet(CWF::Request &req, CWF::Response &response) const override
	{
		_dispatchRequest(req, response, true);
	}

	void doPost(CWF::Request &req, CWF::Response &response) const override
	{
		_dispatchRequest(req, response, false);
	}


protected:
	void _dispatchRequest(CWF::Request &req, CWF::Response &response, bool flagGet) const
	{
		// TODO params check.
		QByteArray __url_ = req.getHttpParser().getUrl();
		QString url = QString::fromUtf8(QByteArray::fromPercentEncoding(__url_));
		QByteArray op = req.getHttpParser().getParameter("op");
		//QByteArray api = req.getHttpParser().getParameter("api");

		QString peerAddr = req.getSocket().peerAddress().toString();

#ifdef DEV_EDITION
		QString logmsg = QString("Request from IP: %1, url: %2").arg(peerAddr).arg(url);

		if (op.size() > 0)
			logmsg = logmsg + ", op: " + op;

		if (op == "checkupdate")
		{
			QByteArray bRn = req.getHttpParser().getParameter("rn", true, false);
			QString rn = QString::fromUtf8(QByteArray::fromPercentEncoding(bRn));

			logmsg = logmsg + ", rn: " + rn;
		}

		QByteArray src = req.getHttpParser().getParameter("src");

		if (src.size() > 0)
			logmsg = logmsg + ", src: " + src;

		if (op != "watchdog")
			g_syslog.logMessage(SYSLOG_LEVEL_INFO, "", logmsg);
#endif

		if (op != "watchdog")
			g_statsMgr.increaseRequestStatsCounter();

		QString sid = ""; // default is guest
		QJsonObject postobj;
		if (getPostData(req, postobj))
		{
			if (postobj.contains("sid"))
			{
				sid = postobj.value("sid").toString();
				// session check
				if (sid.size() > 0 && !sessionCheck(sid))
				{
					QJsonObject result;
					result.insert("status", "fail");
					result.insert("errcode", "invalid_session");

					QString out = QString(QJsonDocument(result).toJson(QJsonDocument::Compact));
					response.write(out.toUtf8());
					return;
				}
			}
		}

		REQ_CONTEXT ctx;
		ctx.sid = &sid;
		ctx.postobj = &postobj;

		if (op.size() > 0)
		{
			if (g_opcodeHandler.handle_opcode(op, req, response, ctx))
				return;
		}

		if (url == "/")
		{
			ConfigMgr *cfgMgr = ConfigMgr::GetInstance();
			QString appRootPath = cfgMgr->getAppRootDir();
			QString siteIndex = appRootPath + "/site.html";

			if (QFile(siteIndex).exists())
			{
				g_statsMgr.increaseSiteRequestStatsCounter();
				response.sendRedirect("/site.html");
			}
			else
				response.sendRedirect("/signin");
		}
		else if (url.startsWith("/file/"))
		{
			//handle_file_access(req, response);
		}
		else if (url == (URL_SIGNIN) || url == (URL_SIGNUP))
		{
			handle_frontend_routing(response);
		}
		else if (url.startsWith("/spaces"))
		{
			if (access_upload_file(url))
				handle_file_access(req, response);
			else
				handle_frontend_routing(response);
		}
		else if (url.startsWith("/admin") || url.startsWith("/search"))
		{
			handle_frontend_routing(response);
		}
		else if (url.startsWith("/demo"))
		{
			handle_frontend_routing(response);
		}
		else if (url == "/auth")
		{
			QByteArray body = req.getHttpParser().getBody();
			QString data = QString::fromUtf8(body);

			// 
			QJsonObject result;
			handleSignIn(data, result);

			QString out = QString(QJsonDocument(result).toJson(QJsonDocument::Compact));

			response.write(out.toUtf8());

			return;
		}
		else if (url == "/sysver")
		{
			QJsonObject result;

			result.insert("status", "ok");
			result.insert("sysver", __SYS_VERSION_);

			QString out = QString(QJsonDocument(result).toJson(QJsonDocument::Compact));

			response.write(out.toUtf8());

			return;
		}
		else
		{
			send_response_msg(response, "<html><body>Unexpected url</body></html>");
		}
	}

	void handle_frontend_routing(CWF::Response &response) const
	{
		static QString appRootDir = ConfigMgr::GetInstance()->getAppRootDir();
		static QString filename = appRootDir + "/" + "index.html";

		QFile file(filename);
		if (file.open(QIODevice::ReadOnly))
		{
			response.write(file.readAll());
		}
		else
		{
			response.sendError(0, "Failed to open file.");
		}
	}

	void send_response_msg(CWF::Response &response, const char *msg) const
	{
		response.write(QByteArray(msg));
	}

	bool sessionCheck(QString sid) const
	{
		QJsonObject sessionobj;
		if (!g_sessionMgr.getSession(sid, sessionobj))
			return false;

		return true;
	}

	void handleSignIn(QString data, QJsonObject &result) const
	{
		_handleSignIn(data, result);
	}

	void handle_file_access(CWF::Request &req, CWF::Response &response) const
	{
		QByteArray __url_ = req.getHttpParser().getUrl();
		QString url = QString::fromUtf8(QByteArray::fromPercentEncoding(__url_));

		// TODO-3 check permission

		static QString appRootPath = "";

		if (appRootPath == "")
		{
			ConfigMgr *cfgMgr = ConfigMgr::GetInstance();
			appRootPath = cfgMgr->getAppRootDir();
		}

		QString fullFilePath = appRootPath + "/data" + url;

		QStringList list = url.split("/");
		int n = list.size();

		QString upload = list.at(n-2);
		if (upload != "upload")
			return;

		QString name = list.at(n - 1);

		QFile file(fullFilePath);
		if (file.exists())
		{
			QString contentType;
			QString disposition;
			buildContentType(name, contentType, disposition);

			response.write_file(req, fullFilePath, contentType, disposition);

			//response.write(file.readAll());
		}
		else
		{
			response.sendError(0, "Unable to read file");
		}

		return;

	}
};


void InitUrlHandlers(ServerApp &server)
{
	server.addController<TslController>("/");
	server.addController<TslController>("/spaces");
	server.addController<TslController>("/spaces/*");
	server.addController<TslController>("/file/*");
	server.addController<TslController>(URL_SIGNIN);
	server.addController<TslController>(URL_SIGNUP);
	server.addController<TslController>("/auth");
	server.addController<TslController>("/sysver");
	server.addController<TslController>("/admin");
	server.addController<TslController>("/admin/*");
	server.addController<TslController>("/search");
	server.addController<TslController>("/search/*");
	server.addController<TslController>("/demo");
	server.addController<TslController>("/demo/*");
	server.addController<TslController>("/api");

	// don't allow below urls.
	server.addController<TslController>("/config/ssl/*");
	server.addController<TslController>("/config/conf");
	server.addController<TslController>("/config/log/*");
	server.addController<TslController>("/index");
}