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

#include "demomgr.h"

#include <QString>

#include<stdio.h>
#include<time.h>
#include "userdbmgr.h"
#include "spacedbmgr.h"
#include "crypto.h"
#include <QJsonDocument>
#include "urlhandlers.h"
#include "configmgr.h"
#include "utils.h"

extern UserDbMgr     g_userDbMgr;
extern SpaceDbMgr    g_spaceDbMgr;

static QString createSpaceAndUser()
{

	int count = 0;

	QString name;
	while (count < 1000) // try 1000 times
	{
		count++;

		name = g_userDbMgr.autoGenUserAccount();

		if (g_userDbMgr.isUserAccountExists(name) || g_spaceDbMgr.isSpaceExists(name))
			continue;

		// create space
		if (!g_spaceDbMgr.addSpace(name, QString("/spaces/") + name))
		{
			return "";
		}

		// create user
		if (!g_userDbMgr.createNewUserPwd(name, "", name, "123", "regular", name))
		{
			return "";
		}

		// add user to space r/w list
		if (!g_spaceDbMgr.addToSpaceRlist(name, "user", name))
			return "";
		if (!g_spaceDbMgr.addToSpaceRlist(name, "user", "*"))
			return "";
		if (!g_spaceDbMgr.addToSpaceRlist(name, "user", "admin"))
			return "";
		if (!g_spaceDbMgr.addToSpaceRlist(name, "user", "guest"))
			return "";

		if (!g_spaceDbMgr.addToSpaceWlist(name, "user", name))
			return "";
		if (!g_spaceDbMgr.addToSpaceWlist(name, "user", "*"))
			return "";
		if (!g_spaceDbMgr.addToSpaceWlist(name, "user", "admin"))
			return "";

		return name;
	}

	return "";
}

static void copyDemodataToDemospace(const QString &account)
{

	ConfigMgr *cfgMgr = ConfigMgr::GetInstance();
	QString appRootPath = cfgMgr->getAppRootDir();

	QString srcPath = appRootPath + "/misc/demodata/pages";
	QString destPath = appRootPath + "/data/spaces/" + account + "/pages";

	copyPath(srcPath, destPath);
	// void    copyPath(QString src, QString dst);
	/*
	// create space path
	ConfigMgr *cfgMgr = ConfigMgr::GetInstance();
	QString appRootPath = cfgMgr->getAppRootDir();
	QString spacePath = appRootPath + "/data"+path+"/pages";
	QDir dir(spacePath);
	if (!dir.mkpath(spacePath))
		return false;
	*/
}

static void _handle_create_demo_instance(QJsonObject &result)
{
	QString useraccount = createSpaceAndUser();

	if (useraccount == "")
	{
		result.insert("status", "error");
		return;
	}

	if (useraccount != "")
	{
		copyDemodataToDemospace(useraccount);
	}

	QJsonObject obj;
	QString credential = Crypto::genCredentialByPassword("123");
	obj.insert("account", useraccount);
	obj.insert("credential", credential);

	QString param = QString(QJsonDocument(obj).toJson(QJsonDocument::Compact));

	return _handleSignIn(param, result);
}

DemoMgr::DemoMgr()
{
	m_mutex = new QMutex(QMutex::Recursive);
}

DemoMgr::~DemoMgr()
{
	if (m_mutex)
		delete m_mutex;
}

void DemoMgr::handle_create_demo_instance(QJsonObject &result)
{
	QMutexLocker autolock(m_mutex);

	_handle_create_demo_instance(result);
}