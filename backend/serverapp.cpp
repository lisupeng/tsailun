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

#include "serverapp.h"
#include "syslog.h"
#include <QDir>

extern Syslog        g_syslog;

ServerApp::ServerApp(int argc, char *argv[], const QString &serverPath) : CWF::CppWebApplication(argc, argv, serverPath)
{

}

ServerApp::~ServerApp()
{
}

int ServerApp::start()
{
	if (!configuration.isValid() || !valid)
	{
		g_syslog.logMessage(SYSLOG_LEVEL_ERROR, "", "E1001 Invalid configuration. Server offline.");
		QThread::msleep(3000);
		return -1;
	}

	QString msg = QString("Listen on port %1").arg(configuration.getPort());
	g_syslog.logMessage(SYSLOG_LEVEL_INFO, "", msg);

	if (!server->listen(configuration.getHost(), configuration.getPort()))
	{
		QString msg = QString("E1000 Failed to listen on port %1. Server offline.").arg(configuration.getPort());
		g_syslog.logMessage(SYSLOG_LEVEL_ERROR, "", msg);
		QThread::msleep(3000);
		return -1;
	}

	g_syslog.logMessage(SYSLOG_LEVEL_INFO, "", "Server started successfully.");

	return application.exec();
}


QString ServerApp::getServerDataDir()
{
	QString binDir = QCoreApplication::applicationDirPath();
	QDir binParentDir = QDir(binDir);
	binParentDir.cdUp();

	QString serverDataPath = binParentDir.absolutePath() + "/server";

	return serverDataPath;
}