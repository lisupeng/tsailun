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

#ifndef _AUTH_H
#define _AUTH_H

#include <QString>

enum AUTH_RET
{
	AUTH_RET_SUCCESS                = 0,
	AUTH_RET_INVALID_SESSION        = 10,
	AUTH_RET_ACCESS_DENIED          = 11
};

class AuthMgr
{
public:

	static int SpaceReadPermissionCheck(const QString &sid, const QString &url);
	static int SpaceReadPermissionCheckBySpaceName(const QString &sid, const QString &spaceName);
	static int SpaceWritePermissionCheck(const QString &sid, const QString &url);
	static int AdminPermissionCheck(const QString &sid);
};

#endif // _AUTH_H