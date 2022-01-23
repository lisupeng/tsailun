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

#include "basedbmgr.h"

BaseDbMgr::BaseDbMgr()
{
	m_recursiveMgrObjLock = new QMutex(QMutex::Recursive);
	m_stopFlag = false;

}
BaseDbMgr::~BaseDbMgr()
{
	if (m_recursiveMgrObjLock)
		delete m_recursiveMgrObjLock;
}

void BaseDbMgr::handleDbRequest(void *data)
{



}

void BaseDbMgr::dbRequest(void *data)
{
	QMutexLocker autolock(m_recursiveMgrObjLock);
	m_reqData = data;

	m_reqSem.release(1);

	m_replySem.acquire(1);
}

void BaseDbMgr::run()
{
	while (!m_stopFlag)
	{
		// get sem
		m_reqSem.acquire(1);

		if (m_stopFlag)
			return;

		handleDbRequest(m_reqData);

		// signal reply sem
		m_replySem.release(1);
	}

}

void BaseDbMgr::cleanup()
{
	QMutexLocker autolock(m_recursiveMgrObjLock);

	m_stopFlag = true;

	m_reqSem.release(1);

	// wait thread exit
	this->wait();
}