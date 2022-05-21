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

#ifndef _STATS_H
#define _STATS_H

#include <stdint.h>

class StatsMgr
{
public:
	StatsMgr();

public:
	// increasePeerAddrStatsCounter(const QString &peerAddr);

	// increaseUserRequestStatsCounter(const QString &account);

public:
	uint64_t     m_requestCounter;
	uint64_t     m_siteRequestCounter;

	uint64_t     m_create_page;
	uint64_t     m_read_page;
	uint64_t     m_write_page;
	uint64_t     m_upload_file;
	uint64_t     m_del_file;

	uint64_t     m_stats_chrome;
	uint64_t     m_stats_edge;
	uint64_t     m_stats_safari;
	uint64_t     m_stats_firefox;
	uint64_t     m_stats_other;
};

#endif // _STATS_H