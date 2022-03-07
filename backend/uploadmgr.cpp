/*
* This file is part of the Tsailun project
*
* Copyright (c) 2022 Li Supeng
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

#include "uploadmgr.h"

UploadMgr::UploadMgr()
{
	m_mutex = new QMutex(QMutex::Recursive);
}

UploadMgr::~UploadMgr()
{
	if (m_mutex)
		delete m_mutex;
}

bool UploadMgr::addRecord(const QString &tid, const UploadData &data)
{
	if (!(tid.size() > 0))
		return false;

	QMutexLocker autolock(m_mutex);

	m_uploadMap[tid] = data;

	return true;
}
bool UploadMgr::getRecord(const QString &tid, UploadData &data)
{
	QMutexLocker autolock(m_mutex);

	std::map<QString, UploadData>::iterator it = m_uploadMap.find(tid);
	if (it == m_uploadMap.end())
		return false;

	data = m_uploadMap[tid];

	return true;
}