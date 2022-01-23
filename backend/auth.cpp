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
 
#include "auth.h"
#include "utils.h"
#include "sessionmgr.h"
#include "userdbmgr.h"
#include "spacedbmgr.h"
#include "groupdbmgr.h"

extern SessionMgr    g_sessionMgr;
extern UserDbMgr     g_userDbMgr;
extern SpaceDbMgr    g_spaceDbMgr;
extern GroupDbMgr    g_groupDbMgr;

int AuthMgr::SpaceReadPermissionCheck(const QString &sid, const QString &url)
{
	QString spacePath = getSpacePathByUrl(url);
	if (spacePath == "")
	{
		return AUTH_RET_ACCESS_DENIED;
	}

	QString account;
	QJsonObject sessionobj;
	if (sid.isNull() || sid == "")
	{
		account = "guest";
	}
	else
	{
		if (!g_sessionMgr.getSession(sid, sessionobj))
		{
			return AUTH_RET_INVALID_SESSION;
		}

		account = sessionobj.value("account").toString();
	}

	QString spaceName;
	if (!g_spaceDbMgr.getSpaceNameByPath(spacePath, spaceName))
	{
		return AUTH_RET_ACCESS_DENIED;
	}

	// getSpaceRlist
	QJsonArray rlist;
	if (!g_spaceDbMgr.getSpaceRlist(spaceName, rlist))
	{
		return AUTH_RET_ACCESS_DENIED;
	}

	for (int i = 0; i < rlist.size(); i++)
	{
		QJsonObject obj = rlist[i].toObject();

		QString item = obj.value("item").toString();
		QString itemtype = obj.value("itemtype").toString();

		if (item == "*")
		{
			return AUTH_RET_SUCCESS;
		}

		if (itemtype == "user" && item == account)
		{
			return AUTH_RET_SUCCESS;
		}

		if (itemtype == "group" && g_groupDbMgr.isUserInGroup(item, account))
		{
			return AUTH_RET_SUCCESS;
		}

	}

	return AUTH_RET_ACCESS_DENIED;
}

int AuthMgr::SpaceWritePermissionCheck(const QString &sid, const QString &url)
{
	QString spacePath = getSpacePathByUrl(url);
	if (spacePath == "")
	{
		return AUTH_RET_ACCESS_DENIED;
	}

	QString account;
	QJsonObject sessionobj;
	if (sid.isNull() || sid == "")
	{
		account = "guest";
	}
	else
	{
		if (!g_sessionMgr.getSession(sid, sessionobj))
		{
			return AUTH_RET_INVALID_SESSION;
		}

		account = sessionobj.value("account").toString();
	}

	QString spaceName;
	if (!g_spaceDbMgr.getSpaceNameByPath(spacePath, spaceName))
	{
		return AUTH_RET_ACCESS_DENIED;
	}

	QJsonArray wlist;
	if (!g_spaceDbMgr.getSpaceWlist(spaceName, wlist))
	{
		return AUTH_RET_ACCESS_DENIED;
	}

	for (int i = 0; i < wlist.size(); i++)
	{
		QJsonObject obj = wlist[i].toObject();

		QString item = obj.value("item").toString();
		QString itemtype = obj.value("itemtype").toString();

		if (item == "*")
		{
			return AUTH_RET_SUCCESS;
		}

		if (itemtype == "user" && item == account)
		{
			return AUTH_RET_SUCCESS;
		}

		if (itemtype == "group" && g_groupDbMgr.isUserInGroup(item, account))
		{
			return AUTH_RET_SUCCESS;
		}

	}

	return AUTH_RET_ACCESS_DENIED;
}

int AuthMgr::AdminPermissionCheck(const QString &sid)
{
	QString account;
	QJsonObject sessionobj;
	if (sid.isNull() || sid == "")
	{
		account = "guest";
		return AUTH_RET_ACCESS_DENIED;
	}
	else
	{
		if (!g_sessionMgr.getSession(sid, sessionobj))
		{
			return AUTH_RET_INVALID_SESSION;
		}

		QString role = sessionobj.value("role").toString();

		if (role == "admin")
			return AUTH_RET_SUCCESS;
	}

	return AUTH_RET_ACCESS_DENIED;
}

int AuthMgr::SpaceReadPermissionCheckBySpaceName(const QString &sid, const QString &spaceName)
{
	QString account;
	QJsonObject sessionobj;
	if (sid.isNull() || sid == "")
	{
		account = "guest";
	}
	else
	{
		if (!g_sessionMgr.getSession(sid, sessionobj))
		{
			return AUTH_RET_INVALID_SESSION;
		}

		account = sessionobj.value("account").toString();
	}

	// getSpaceRlist
	QJsonArray rlist;
	if (!g_spaceDbMgr.getSpaceRlist(spaceName, rlist))
	{
		return AUTH_RET_ACCESS_DENIED;
	}

	for (int i = 0; i < rlist.size(); i++)
	{
		QJsonObject obj = rlist[i].toObject();

		QString item = obj.value("item").toString();
		QString itemtype = obj.value("itemtype").toString();

		if (itemtype == "user" && item == account)
		{
			return AUTH_RET_SUCCESS;
		}

		if (itemtype == "group" && g_groupDbMgr.isUserInGroup(item, account))
		{
			return AUTH_RET_SUCCESS;
		}

	}

	return AUTH_RET_ACCESS_DENIED;
}