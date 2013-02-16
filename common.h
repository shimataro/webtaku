#ifndef COMMON_H
#define COMMON_H

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
	QString    outputFilename;
	QString    outputFormat;
	QString    userAgent;
	QByteArray cookie;
	QSize      minSize;
	QSize      scaledSize;
	bool       scaleMax;
	bool       crop;
	int        timer_ms;
	size_t     maxRequests;
	int        quality;
};

#endif // COMMON_H
