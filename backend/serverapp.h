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

#ifndef _SERVERAPP_H
#define _SERVERAPP_H

#include <cwf/cppwebapplication.h>
#include <QString>

class ServerApp : public CWF::CppWebApplication
{
public:
	ServerApp(int argc, char *argv[], const QString &serverPath);
	virtual ~ServerApp();
public:
	int start();

	QString getServerDataDir();
};

#endif // _SERVERAPP_H