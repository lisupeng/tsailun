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

#include <QCryptographicHash>
#include <QFile>
#include "crypto.h"

QString Crypto::genCredentialByPassword(QString password)
{
	QByteArray hashresult = QCryptographicHash::hash(QString(password).toUtf8(), QCryptographicHash::Sha256);
	QString credential = QString::fromUtf8(hashresult.toBase64());

	return credential;
}

QString Crypto::genFileSHA(const QString &filename)
{
	QFile file(filename);
	if (!file.open(QIODevice::ReadOnly))
		return QString();

	QByteArray hashresult = QCryptographicHash::hash(file.readAll(), QCryptographicHash::Sha256);

	file.close();

	return hashresult.toHex();
}