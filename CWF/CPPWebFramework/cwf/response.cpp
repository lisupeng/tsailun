/*
 Copyright 2017 Herik Lima de Castro and Marcelo Medeiros Eler
 Distributed under MIT license, or public domain if desired and
 recognized in your jurisdiction.
 See file LICENSE for detail.
*/

#include "response.h"
#include "request.h"
#include "configuration.h"
#include <QDateTime>
#include <QFileInfo>

CWF_BEGIN_NAMESPACE

Response::Response(QTcpSocket &socket, const Configuration &configuration) : socket(socket),
                                                                             configuration(configuration)

{
}

static bool waitForSend(QTcpSocket &socket)
{
	int retry = 0;
	qint64 left = -1;
	qint64 left_last = -1;

	do
	{
		socket.flush();

		if (socket.ConnectingState > 0)
		{
			socket.waitForBytesWritten(60 * 1000);
		}
		else
		{
			return false;
		}

		left = socket.bytesToWrite();

		if (left == 0)
			break;

		if (left_last != -1 && left == left_last)
			return false;

		left_last = left;

		retry++;

	} while ((left > 0) && retry <= 60);

	if (left == 0)
		return true;
	else
		return false;
}

static void sendBytes(QTcpSocket &socket, const QByteArray &bytes, int timeOutms)
{
	qint64 total = bytes.size();
	qint64 bytes_left = total;

	int retry = 0;
    if(socket.ConnectingState > 0 && bytes.size() > 0)
    {
		while (bytes_left > 0 && retry <= 20)
		{
			qint64 ret = socket.write((bytes.data()) + (total - bytes_left), bytes_left);
			if (ret >= 0)
				bytes_left -= ret;
			else
				return;

			if (bytes_left == 0)
				break;

			QThread::msleep(20);
			retry++;
		}
        //socket.flush();

        //if(socket.ConnectingState > 0)
        //{
        //    socket.waitForBytesWritten(timeOut);
        //}
    }
}

void buildHeadersString(QByteArray &temp, const QMap<QByteArray, QByteArray> &headers)
{
    QList<QByteArray> headersList(headers.keys());

    for(const auto &i : headersList)
    {
        temp.push_back(i);
        temp.push_back(HTTP::SEPARATOR);
        temp.push_back(headers.value(i));
        temp.push_back(HTTP::END_LINE);
    }
}

void buildCookiesString(QByteArray &temp, const QVector<QNetworkCookie> &cookies)
{
    for(const auto &i : cookies)
    {
        temp.push_back(HTTP::SET_COOKIE);
        temp.push_back(i.toRawForm());
        temp.push_back(HTTP::END_LINE);
    }
}

void sendHeaders(int statusCode,
                 int timeOut,
                 const QByteArray &statusText,
                 QMap<QByteArray, QByteArray> &headers,
                 QVector<QNetworkCookie> &cookies,
                 QTcpSocket &socket)
{
    QByteArray temp(HTTP::HTTP_1_1);
    temp.reserve(100);
    temp.push_back(QByteArray::number(statusCode));
    temp.push_back(' ');
    temp.push_back(statusText);
    temp.push_back(HTTP::END_LINE);

    if(!headers.contains(HTTP::CONTENT_TYPE))
    {
        headers.insert(HTTP::CONTENT_TYPE, HTTP::TEXT_HTML_UTF8);
    }

    buildHeadersString(temp, headers);
    buildCookiesString(temp, cookies);
    temp.push_back(HTTP::END_LINE);
    sendBytes(socket, temp, timeOut);
}

void Response::flushBuffer()
{
    const int max = 32768; // increase this ?

	int timeOut = 3600000;//configuration.getTimeOut();
	bool biggerThanLimit = content.size() > max;
	headers.insert(HTTP::CONTENT_LENGTH, QByteArray::number(content.size()));
	headers.insert(HTTP::SERVER, HTTP::SERVER_VERSION);
	headers.insert(HTTP::DATA, QByteArray(QDateTime::currentDateTime().toString("ddd, dd MMM yyyy hh:mm:ss").toLatin1() + " GMT"));

	if (biggerThanLimit)
		headers.insert(HTTP::TRANSFER_ENCODING, HTTP::CHUNKED);

	sendHeaders(statusCode, timeOut, statusText, headers, cookies, socket);


    if (!content.isEmpty())
	{


        if(!biggerThanLimit)
        {
            sendBytes(socket, content, timeOut);
        }
        else
        {
            int total = (content.size() / max) + 1, last = 0;

            QVector<QByteArray> vetor;
            for(int i = 0; i < total; ++i)
            {
                vetor.push_back(content.mid(last, max));
                last += max;
            }

            for(auto &i : vetor)
            {
                QByteArray data(std::move(i));
                if(!data.isEmpty())
                {
                    sendBytes(socket, (QByteArray::number(data.size(), 16) + HTTP::END_LINE), timeOut);
                    sendBytes(socket, data, timeOut);
                    sendBytes(socket, HTTP::END_LINE, timeOut);
                }
            }
            sendBytes(socket, HTTP::END_OF_MESSAGE_WITH_ZERO, timeOut);
        }

    }

	waitForSend(socket);

	socket.disconnectFromHost();
	socket.close();
	content.clear();
}

void Response::sendError(int sc, const QByteArray &msg)
{
	int timeOut = 3600000;// configuration.getTimeOut();
    sendHeaders(statusCode, timeOut, statusText, headers, cookies, socket);
    sendBytes(socket, "<html><body><h1>" + QByteArray::number(sc) + " " + msg + "</h1></body></html>", timeOut);
}

/*
void Response::write(const QJsonObject &json, bool writeContentType)
{
    if(writeContentType)
        addHeader(CWF::HTTP::CONTENT_TYPE, CWF::HTTP::APPLICATION_JSON);
    content = QJsonDocument(json).toJson();
    flushBuffer();
}
*/

/*
void Response::write(const QJsonArray &array, bool writeContentType)
{
    if(writeContentType)
        addHeader(CWF::HTTP::CONTENT_TYPE, CWF::HTTP::APPLICATION_JSON);
    content = QJsonDocument(array).toJson();
    flushBuffer();
}
*/

/*
void Response::write(QByteArray &&data)
{
    content = std::move(data);
    flushBuffer();
}
*/

void Response::write(const QByteArray &data)
{
	// avoid mem copy in the future
	content = data;
	flushBuffer();
}

/*
void Response::write(const QByteArray &data, bool flush)
{
    content += data;
    if(flush)
        flushBuffer();
}
*/

bool Response::handle_etag(Request &request, const QString &filepath)
{
	QByteArray etag = request.getHttpParser().getHeaderField("If-None-Match");

	if (etag.size() == 0)
		return false;

	QString curEtag = getFileEtag(filepath);

	if (curEtag.toUtf8() != etag)
		return false;

	setStatus(304, "Not Modified");

	// send_data
	QByteArray data;
	write(data);

	return true;
}

bool Response::handle_partialcontent(Request &request, const QString &filepath)
{
	QByteArray range = request.getHttpParser().getHeaderField("Range");

	if (range.size() == 0)
		return false;

	QString srange = range;
	srange = srange.remove("bytes=");

	// split by '-'
	QStringList list = srange.split('-');

	if (list.size() <= 1)
		return false;

	qint64 start = list[0].toLongLong();
	qint64 end = 0;

	if (list.size() >= 2)
		end = list[1].toLongLong();

	QFile file(filepath);

	if (!(file.open(QIODevice::ReadOnly)))
		return false;

	if (!file.seek(start))
		return false;

	// read a range between 1M and 20M
	qint64 size;
	if (end <= start)
		size = 1 * 1024 * 1024;
	else if (((end - start) >= 20 * 1024 * 1024) || ((end - start) <= 1 * 1024 * 1024))
		size = 20 * 1024 * 1024;
	else
		size = (end - start) + 1;

	QByteArray _data = file.read(size);

	if (_data.size() <= 0)
		return false;

	size = _data.size();

	end = start + size - 1;

	addEtagToHeader(filepath);

	// set range
	QString contentRange = QString("bytes %1-%2/%3").arg(start).arg(end).arg(file.size());
	headers.insert("Content-Range", contentRange.toUtf8());

	setStatus(206, "Partial Content");

	// send_data
	write(_data);

	return true;
}

QString Response::getFileEtag(const QString &filename)
{
	QFileInfo info(filename);
	qint64 ts = info.lastModified().toMSecsSinceEpoch();
	qint64 total = QFile(filename).size();
	QString etag = QString::number(ts, 16) + "-" + QString::number(total, 16);

	return etag;
}

void Response::addEtagToHeader(const QString &filename)
{
	QString etag = getFileEtag(filename);

	headers.insert("ETag", etag.toUtf8());
}

void Response::write_file(Request &request, const QString &filepath, QString contentType, QString contentDisposition)
{
	//const int max = 32768;
	const int max = 1 * 1024 * 1024;

	if (contentType.size() > 0)
		headers.insert(CWF::HTTP::CONTENT_TYPE, contentType.toUtf8());

	if (contentDisposition.size() > 0)
		headers.insert("content-disposition", contentDisposition.toUtf8());

	if (handle_etag(request, filepath))
		return;

	if (handle_partialcontent(request, filepath))
		return;

	QFile file(filepath);
	qint64 filesize = file.size();

	if (!(file.open(QIODevice::ReadOnly)))
		goto CLOSE_SOCKET;

	int timeOut = 3600000;// configuration.getTimeOut();
	bool biggerThanLimit = filesize > max;
	headers.insert(HTTP::CONTENT_LENGTH, QByteArray::number(filesize));
	headers.insert(HTTP::SERVER, HTTP::SERVER_VERSION);
	headers.insert(HTTP::DATA, QByteArray(QDateTime::currentDateTime().toString("ddd, dd MMM yyyy hh:mm:ss").toLatin1() + " GMT"));

	addEtagToHeader(filepath);

	if (!biggerThanLimit)
	{
		sendHeaders(statusCode, timeOut, statusText, headers, cookies, socket);

		QByteArray data = file.read(max);
		sendBytes(socket, data, timeOut);
	}
	else
	{
		headers.insert(HTTP::TRANSFER_ENCODING, HTTP::CHUNKED);
		sendHeaders(statusCode, timeOut, statusText, headers, cookies, socket);

		qint64 total = (filesize / max) + 1;

		for (qint64 i = 0; i < total; ++i)
		{
			QByteArray data = file.read(max);

			if (!data.isEmpty())
			{
				sendBytes(socket, (QByteArray::number(data.size(), 16) + HTTP::END_LINE), timeOut);
				sendBytes(socket, data, timeOut);
				sendBytes(socket, HTTP::END_LINE, timeOut);
			}
		}
		sendBytes(socket, HTTP::END_OF_MESSAGE_WITH_ZERO, timeOut);
	}

CLOSE_SOCKET:

	waitForSend(socket);

	socket.disconnectFromHost();
	socket.close();
	content.clear();
}

void Response::setStatus(int statusCode, const QByteArray &description)
{
    this->statusCode = statusCode;
    statusText       = description;
}

void Response::sendRedirect(const QByteArray &url)
{
    setStatus(Response::SC_SEE_OTHER, HTTP::SEE_OTHER);
    addHeader(HTTP::LOCATION, url);
    //write(HTTP::REDIRECT, true);

	content += HTTP::REDIRECT;
	flushBuffer();
}

const int Response::SC_CONTINUE = 100;

const int Response::SC_SWITCHING_PROTOCOLS = 101;

const int Response::SC_OK = 200;

const int Response::SC_CREATED = 201;

const int Response::SC_ACCEPTED = 202;

const int Response::SC_NON_AUTHORITATIVE_INFORMATION = 203;

const int Response::SC_NO_CONTENT = 204;

const int Response::SC_RESET_CONTENT = 205;

const int Response::SC_PARTIAL_CONTENT = 206;

const int Response::SC_MULTIPLE_CHOICES = 300;

const int Response::SC_MOVED_PERMANENTLY = 301;

const int Response::SC_MOVED_TEMPORARILY = 302;

const int Response::SC_FOUND = 302;

const int Response::SC_SEE_OTHER = 303;

const int Response::SC_NOT_MODIFIED = 304;

const int Response::SC_USE_PROXY = 305;

const int Response::SC_TEMPORARY_REDIRECT = 307;

const int Response::SC_BAD_REQUEST = 400;

const int Response::SC_UNAUTHORIZED = 401;

const int Response::SC_PAYMENT_REQUIRED = 402;

const int Response::SC_FORBIDDEN = 403;

const int Response::SC_NOT_FOUND = 404;

const int Response::SC_METHOD_NOT_ALLOWED = 405;

const int Response::SC_NOT_ACCEPTABLE = 406;

const int Response::SC_PROXY_AUTHENTICATION_REQUIRED = 407;

const int Response::SC_REQUEST_TIMEOUT = 408;

const int Response::SC_CONFLICT = 409;

const int Response::SC_GONE = 410;

const int Response::SC_LENGTH_REQUIRED = 411;

const int Response::SC_PRECONDITION_FAILED = 412;

const int Response::SC_REQUEST_ENTITY_TOO_LARGE = 413;

const int Response::SC_REQUEST_URI_TOO_LONG = 414;

const int Response::SC_UNSUPPORTED_MEDIA_TYPE = 415;

const int Response::SC_REQUESTED_RANGE_NOT_SATISFIABLE = 416;

const int Response::SC_EXPECTATION_FAILED = 417;

const int Response::SC_INTERNAL_SERVER_ERROR = 500;

const int Response::SC_NOT_IMPLEMENTED = 501;

const int Response::SC_BAD_GATEWAY = 502;

const int Response::SC_SERVICE_UNAVAILABLE = 503;

const int Response::SC_GATEWAY_TIMEOUT = 504;

const int Response::SC_HTTP_VERSION_NOT_SUPPORTED = 505;

CWF_END_NAMESPACE
