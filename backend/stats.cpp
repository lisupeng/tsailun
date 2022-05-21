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

#include "stats.h"

StatsMgr::StatsMgr()
{
	m_requestCounter = 0;
	m_siteRequestCounter = 0;

	m_create_page = 0;
	m_read_page   = 0;
	m_write_page  = 0;
	m_upload_file = 0;
	m_del_file    = 0;

	m_stats_chrome = 0;
	m_stats_edge = 0;
	m_stats_safari = 0;
	m_stats_firefox = 0;
	m_stats_other = 0;
}
