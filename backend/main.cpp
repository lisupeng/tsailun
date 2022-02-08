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
#include <QThread>

#include "serverapp.h"
#include "utils.h"
#include "groupdbmgr.h"
#include "userdbmgr.h"
#include "spacedbmgr.h"
#include "syslog.h"

#include "sysdefs.h"
#include "urlhandlers.h"

extern UserDbMgr     g_userDbMgr;
extern SpaceDbMgr    g_spaceDbMgr;
extern GroupDbMgr    g_groupDbMgr;
extern Syslog        g_syslog;

#define EXIT_APP() \
         do { \
           QThread::sleep(3000); \
           return -1; \
		 } while (0);

void static exitApp()
{}

int main(int argc, char *argv[])
{
	ServerApp server(argc, argv, "");

	int ret = serverDirCheck(server.getServerDataDir());
	if (ret != UTILS_RET_OK)
	{
		if (ret == UTILS_RET_PERMISSION_ERR)
		{
			g_syslog.logMessage(SYSLOG_LEVEL_ERROR, "", "Failed to read/write 'server' directory. Please confirm directory permission.");
		}
		else if (ret == UTILS_RET_DIR_MISSING_ERR)
		{
			g_syslog.logMessage(SYSLOG_LEVEL_ERROR, "", "Data directory missing, please re-install software and try again.");
		}
		else
		{
			g_syslog.logMessage(SYSLOG_LEVEL_ERROR, "", "Unknown internal error.");
		}

		EXIT_APP();
	}

	if (!g_userDbMgr.init() || !g_spaceDbMgr.init() || !g_groupDbMgr.init())
	{
		EXIT_APP();
	}

	if (!g_syslog.init())
	{
		EXIT_APP();
	}


	InitUrlHandlers(server);

	QString msg = QString("Software version ") + QString(__SYS_VERSION_);
	g_syslog.logMessage(SYSLOG_LEVEL_INFO, "", msg);

    return server.start();
}
