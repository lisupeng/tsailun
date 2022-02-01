#ifndef _XDIFF_INTERFACE_H
#define _XDIFF_INTERFACE_H

#include <QtCore/qglobal.h>

#if defined(LIBXDIFF_LIBRARY)
#  define LIBXDIFF_EXPORT Q_DECL_EXPORT
#else    
#  define LIBXDIFF_EXPORT Q_DECL_IMPORT
#endif

#ifdef __cplusplus
extern "C" {
#endif

	LIBXDIFF_EXPORT void libxdiff_init();
	LIBXDIFF_EXPORT int  libxdiff_diff(const char *oldfile, const char *newfile, const char *patchfile);
	LIBXDIFF_EXPORT int  libxdiff_patch(const char *oldfile, const char *patchfile, const char *newfile, const char *rejfile);

#ifdef __cplusplus
}
#endif

#endif // _XDIFF_INTERFACE_H