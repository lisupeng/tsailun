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

#ifndef _UTILS_H
#define _UTILS_H

#include <QString>

#define UTILS_RET_OK                 0
#define UTILS_RET_PERMISSION_ERR     1
#define UTILS_RET_DIR_MISSING_ERR    2

QString PathNameConcat(const QString &path, const QString &filename);

QString CreateUid();

bool    ForceCopy(const QString &src, const QString &target);

QString getSpacePathByUrl(QString url);

bool    checkSpacePathInput(const QString &spacePath, QString &spacePathOut);

int     serverDirCheck(const QString &path);

void    copyPath(QString src, QString dst);

QString getSpaceAndPagePathByUrl(QString url);

bool    sendMail();

void    buildContentType(const QString &name, QString &contentType, QString &disposition);

QString getUserAccountBySessionId(const QString &sid);


#endif // _UTILS_H