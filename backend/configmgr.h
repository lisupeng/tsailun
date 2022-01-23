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

#ifndef _CONFIGMGR_H
#define _CONFIGMGR_H

#include <QString>


class ConfigMgr
{
public:
	static ConfigMgr* GetInstance();

	QString getAppRootDir();
	QString getAppRootUrl();

protected:
	ConfigMgr();
	virtual ~ConfigMgr();

	QString m_appRootDir;
	QString m_appRootUrl;
};

#endif // _CONFIGMGR_H