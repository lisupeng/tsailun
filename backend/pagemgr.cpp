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

#include <QFile>
#include <QTextStream>
#include <QXmlInputSource>
#include <QXmlSimpleReader>
#include "pagemgr.h"
#include "syslog.h"

extern Syslog        g_syslog;

Page::Page()
{
	initNewPage();
}

Page::~Page()
{
}

void Page::initNewPage()
{
	m_Dom.clear();

	QDomElement html = m_Dom.createElement("html");
	m_Dom.appendChild(html);

	QDomElement head = m_Dom.createElement("head");
	html.appendChild(head);
	QDomElement title = m_Dom.createElement("title");
	QDomAttr attr_uid = m_Dom.createAttribute("data-pageuid");
	attr_uid.setValue("");
	title.setAttributeNode(attr_uid);

	head.appendChild(title);

	QDomElement body = m_Dom.createElement("body");
	html.appendChild(body);
}

bool Page::readFromFile(QString filename)
{
	QFile file(filename);
	if (!file.open(QIODevice::ReadOnly))
	{
		return false;
	}

	QXmlInputSource xmlinput;
	xmlinput.setData(file.readAll());
	QXmlSimpleReader reader;
	
	if (!m_Dom.setContent(&xmlinput, &reader)) {
		file.close();
		return false;
	}
	file.close();

	return true;
}

void Page::clear()
{
	m_Dom.clear();

	initNewPage();
}

bool Page::savePage(QString filename)
{

	QFile file(filename);

	if (!file.open(QFile::WriteOnly | QFile::Truncate | QFile::Text))
		return false;
	QTextStream out(&file);
	out.setCodec("UTF-8");
	m_Dom.normalize();
	m_Dom.save(out, 4, QDomNode::EncodingFromTextStream);
	file.close();

	return true;
}

void Page::setUid(QString uid)
{
	QString title = getTitle();
	QDomNode domNode = _getDomByNodePath("/html/head/title");

	if (domNode.isNull())
		return;

	QDomElement e = domNode.toElement();
	QDomAttr a = e.attributeNode("data-pageuid");
	QString oldV = a.value();
	a.setValue(uid);
}

QString Page::getUid()
{
	QDomNode domNode = _getDomByNodePath("/html/head/title");

	if (domNode.isNull())
		return "";

	return domNode.toElement().attribute("data-pageuid");
}


void Page::setTitle(QString title)
{
	QDomNode domNode = _getDomByNodePath("/html/head/title");

	if (domNode.isNull())
		return;

	bool found = false;
	QDomText textNode;

	QDomNode child = domNode.firstChild();
	while (!child.isNull())
	{
		if (child.isText())
		{
			textNode = child.toText();
			found = true;
			break;
		}
		child = child.nextSibling();
	}

	if (found)
	{
		textNode.setData(title);
	}
	else
	{
		textNode = m_Dom.createTextNode(title);
		domNode.appendChild(textNode);
	}
}

QString Page::getTitle()
{
	QDomNode domNode = _getDomByNodePath("/html/head/title");

	if (domNode.isNull())
		return "";

	QString title = domNode.toElement().text();

	return title;
}

QDomNode Page::_getDomByNodePath(QString nodePath)
{
	if (nodePath == "/")
	{
		return m_Dom;
	}

	QDomNode curNode = m_Dom;
	QDomNode nextNode;

	// TODO don't use QT's split
	QStringList tagList = nodePath.split("/");
	for (int i = 0; i < tagList.size()-1; i++)
	{
		nextNode = curNode.firstChildElement(tagList[i+1]);
		if (nextNode.isNull())
		{
			QDomNode ret;
			return ret;
		}

		curNode = nextNode;
	}

	return curNode;
}

bool Page::setContent(const QString &content)
{
	QDomNode domNode = _getDomByNodePath("/html");

	if (domNode.isNull())
	{
		return false;
	}

	// import content to m_Dom
	QDomDocument d;
	QXmlInputSource xmlinput;
	xmlinput.setData(content);
	QXmlSimpleReader reader;
	QString errMsg;
	int errLine, errCol;
	bool ret = d.setContent(&xmlinput, &reader, &errMsg, &errLine, &errCol);
	if (!ret)
	{
		g_syslog.logMessage(SYSLOG_LEVEL_ERROR, "", "E1006 Failed to parse xml, "+errMsg);
	}

	//d.setContent(content);
	QDomNode imported = m_Dom.importNode(d.firstChildElement("body"), true);


	QDomNode oldBody = domNode.firstChildElement("body");
	if (!oldBody.isNull())
	{
		domNode.replaceChild(imported, oldBody);
	}
	else
	{
		domNode.appendChild(imported);
	}

	return true;
}

QString Page::getContent()
{
	QString ret;
	QTextStream out(&ret);
	QDomNode domNode = _getDomByNodePath("/html");
	QDomNode body = domNode.firstChildElement("body");
	if (body.isNull())
		return "";

	body.save(out, 0, QDomNode::EncodingFromTextStream);

	return ret;
}
