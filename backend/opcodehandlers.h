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

#ifndef _OPCODEHANDLERS_H
#define _OPCODEHANDLERS_H

#include <string>
#include <map>
#include <cwf/cppwebapplication.h>

struct REQ_CONTEXT
{
	REQ_CONTEXT()
	{
		sid = nullptr;
		postobj = nullptr;
	}

	QString       *sid;
	QJsonObject   *postobj;
};

class OpcodeHandler;

typedef void (OpcodeHandler::*PHANDLER)(CWF::Request &req, CWF::Response &response, REQ_CONTEXT &ctx);

class OpcodeHandler
{
public:
	OpcodeHandler();

	bool handle_opcode(const QString &op, CWF::Request &req, CWF::Response &response, REQ_CONTEXT &ctx);

protected:
	void handle_delpage(CWF::Request &req, CWF::Response &response, REQ_CONTEXT &ctx);
	void handle_listpage(CWF::Request &req, CWF::Response &response, REQ_CONTEXT &ctx);
	void handle_savepage(CWF::Request &req, CWF::Response &response, REQ_CONTEXT &ctx);
	void handle_readpage(CWF::Request &req, CWF::Response &response, REQ_CONTEXT &ctx);
	void handle_createpage(CWF::Request &req, CWF::Response &response, REQ_CONTEXT &ctx);
	void handle_writelog(CWF::Request &req, CWF::Response &response, REQ_CONTEXT &ctx);
	void handle_exportpage(CWF::Request &req, CWF::Response &response, REQ_CONTEXT &ctx);
	void handle_search(CWF::Request &req, CWF::Response &response, REQ_CONTEXT &ctx);
	void handle_listuser(CWF::Request &req, CWF::Response &response, REQ_CONTEXT &ctx);
	void handle_updateuser(CWF::Request &req, CWF::Response &response, REQ_CONTEXT &ctx);
	void handle_updatecredential(CWF::Request &req, CWF::Response &response, REQ_CONTEXT &ctx);
	void handle_deluser(CWF::Request &req, CWF::Response &response, REQ_CONTEXT &ctx);
	void handle_getuserinfo(CWF::Request &req, CWF::Response &response, REQ_CONTEXT &ctx);
	void handle_getspaceinfo(CWF::Request &req, CWF::Response &response, REQ_CONTEXT &ctx);
	void handle_getgroupinfo(CWF::Request &req, CWF::Response &response, REQ_CONTEXT &ctx);
	void handle_delusergroup(CWF::Request &req, CWF::Response &response, REQ_CONTEXT &ctx);
	void handle_delspace(CWF::Request &req, CWF::Response &response, REQ_CONTEXT &ctx);
	void handle_listspace(CWF::Request &req, CWF::Response &response, REQ_CONTEXT &ctx);
	void handle_listuserspace(CWF::Request &req, CWF::Response &response, REQ_CONTEXT &ctx);
	void handle_updatespace(CWF::Request &req, CWF::Response &response, REQ_CONTEXT &ctx);
	void handle_listgroup(CWF::Request &req, CWF::Response &response, REQ_CONTEXT &ctx);
	void handle_updategroup(CWF::Request &req, CWF::Response &response, REQ_CONTEXT &ctx);
	void handle_creategroup(CWF::Request &req, CWF::Response &response, REQ_CONTEXT &ctx);
	void handle_createspace(CWF::Request &req, CWF::Response &response, REQ_CONTEXT &ctx);
	void handle_createdemo(CWF::Request &req, CWF::Response &response, REQ_CONTEXT &ctx);
	void handle_dirinfo(CWF::Request &req, CWF::Response &response, REQ_CONTEXT &ctx);
	void handle_viewfile(CWF::Request &req, CWF::Response &response, REQ_CONTEXT &ctx);
	void handle_listfile(CWF::Request &req, CWF::Response &response, REQ_CONTEXT &ctx);
	void handle_delfile(CWF::Request &req, CWF::Response &response, REQ_CONTEXT &ctx);
	void handle_upload(CWF::Request &req, CWF::Response &response, REQ_CONTEXT &ctx);
	void handle_uploadchunk(CWF::Request &req, CWF::Response &response, REQ_CONTEXT &ctx);
	void handle_uploadblob(CWF::Request &req, CWF::Response &response, REQ_CONTEXT &ctx);
	void handle_register(CWF::Request &req, CWF::Response &response, REQ_CONTEXT &ctx);
	void handle_listversion(CWF::Request &req, CWF::Response &response, REQ_CONTEXT &ctx);
	void handle_restore(CWF::Request &req, CWF::Response &response, REQ_CONTEXT &ctx);
	void handle_checkpermission(CWF::Request &req, CWF::Response &response, REQ_CONTEXT &ctx);
	void handle_sysver(CWF::Request &req, CWF::Response &response, REQ_CONTEXT &ctx);
	void handle_watchdog(CWF::Request &req, CWF::Response &response, REQ_CONTEXT &ctx);
	void handle_getstats(CWF::Request &req, CWF::Response &response, REQ_CONTEXT &ctx);
	void handle_getlog(CWF::Request &req, CWF::Response &response, REQ_CONTEXT &ctx);
	void handle_moveup(CWF::Request &req, CWF::Response &response, REQ_CONTEXT &ctx);
	void handle_movedown(CWF::Request &req, CWF::Response &response, REQ_CONTEXT &ctx);
	void handle_renamepage(CWF::Request &req, CWF::Response &response, REQ_CONTEXT &ctx);
	void handle_checkupdate(CWF::Request &req, CWF::Response &response, REQ_CONTEXT &ctx);
	void handle_test(CWF::Request &req, CWF::Response &response, REQ_CONTEXT &ctx);

protected:
	std::map<std::string, PHANDLER> m_handlerMap;
};

#endif // _OPCODEHANDLERS_H