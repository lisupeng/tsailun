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

#ifndef _BASEDBMGR_H
#define _BASEDBMGR_H

#include <QMutex>
#include <QSemaphore>
#include <QThread>


class BaseDbMgr : public QThread
{
public:
	BaseDbMgr();
	virtual ~BaseDbMgr();

public:
	// should not be called in handleDbRequest
	void dbRequest(void *data);

	// shouldn't not be called in handleDbRequest 
	void cleanup();

public:
	// childclass should implement this func
	// handleDbRequest will be called in db thread instead of caller thread
	virtual void handleDbRequest(void *data);

protected:

	virtual void run(); // Thread entry

protected:
	QMutex        *m_recursiveMgrObjLock;
	QSemaphore     m_reqSem;
	QSemaphore     m_replySem;

	bool m_stopFlag;

	void          *m_reqData;
};

#endif // _BASEDBMGR_H