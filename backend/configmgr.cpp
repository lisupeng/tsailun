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

#include "configmgr.h"
#include <QCoreApplication>
#include <QDir>

static ConfigMgr *g_cfgMgrInstance = nullptr;

ConfigMgr* ConfigMgr::GetInstance()
{
	if (g_cfgMgrInstance == nullptr)
		g_cfgMgrInstance = new ConfigMgr;

	return g_cfgMgrInstance;
}

ConfigMgr::ConfigMgr()
{
	QString binDir = QCoreApplication::applicationDirPath();
	QDir binParentDir = QDir(binDir);
	binParentDir.cdUp();

	m_appRootDir = binParentDir.absolutePath() + "/server";
	m_appRootUrl = "/";
}

ConfigMgr::~ConfigMgr()
{

}

QString ConfigMgr::getAppRootDir()
{
	return m_appRootDir;
}

QString ConfigMgr::getAppRootUrl()
{
	return m_appRootUrl;
}