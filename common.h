#ifndef COMMON_H
#define COMMON_H

#include <Qt>
#include <QString>
#include <QSize>
#include <stddef.h>

enum EXITCODE
{
	EC_SUCCESS         = 0,
	EC_INVALIDARGUMENT = 1,
	EC_TOOMANYREQUESTS = 2,
	EC_FAILEDTOSAVE    = 3,
};

struct PARAMS
{
	QString             outputFilename;
	QString             outputFormat;
	QString             userAgent;
	QByteArray          acceptLanguage;
	QByteArray          cookie;
	QSize               minSize;
	QSize               scaledSize;
	Qt::AspectRatioMode aspectRatioMode;
	bool                crop;
	int                 timer_ms;
	size_t              maxRequests;
	int                 quality;
};

#endif // COMMON_H
