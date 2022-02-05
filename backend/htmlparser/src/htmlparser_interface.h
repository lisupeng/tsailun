#ifndef _HTMLPARSER_INTERFACE_H
#define _HTMLPARSER_INTERFACE_H

#include <QtCore/qglobal.h>
#include <string>
#include <QByteArray>

#if defined(LIBHTMLPARSER)
#  define LIBHTMLPARSER_EXPORT Q_DECL_EXPORT
#else    
#  define LIBHTMLPARSER_EXPORT Q_DECL_IMPORT
#endif


LIBHTMLPARSER_EXPORT std::string retrieve_text_from_html(QByteArray *html_in_utf8);


#endif // _HTMLPARSER_INTERFACE_H