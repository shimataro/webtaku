#ifndef COMMON_H
#define COMMON_H

#include <Qt>
#include <QByteArray>
#include <QString>
#include <QSize>
#include <stddef.h>

#define SOFTWARE_NAME    "webtaku"
#define SOFTWARE_VERSION "1.2.5"


/**
 * exit status
 */
const int ES_WARNING_TOOMANYREQUESTS = 128;
const int ES_WARNING_TIMEOUT         = 129;


/**
 * GYOTAKU parameters
 */
struct PARAMS
{
	QString             outputFilename;
	QString             outputFormat;
	QString             userAgent;
	QString             defaultEncoding;
	QByteArray          acceptLanguage;
	QByteArray          cookie;
	QSize               minSize;
	QSize               scaledSize;
	Qt::AspectRatioMode aspectRatioMode;
	bool                crop;
	int                 delay_ms;
	int                 timeout_sec;
	size_t              maxRequests;
	int                 quality;
};

#endif // COMMON_H
