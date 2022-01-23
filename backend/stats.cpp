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
}

void StatsMgr::increaseRequestStatsCounter()
{
	m_requestCounter += 1;
}

unsigned int StatsMgr::getRequestStatsCounter()
{
	return m_requestCounter;
}

void StatsMgr::increaseSiteRequestStatsCounter()
{
	m_siteRequestCounter++;
}

unsigned int StatsMgr::getSiteRequestStatsCounter()
{
	return m_siteRequestCounter;
}