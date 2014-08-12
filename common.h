#ifndef COMMON_H
#define COMMON_H

#include <Qt>
#include <QByteArray>
#include <QString>
#include <QSize>
#include <stddef.h>

#define SOFTWARE_NAME    "webtaku"
#define SOFTWARE_VERSION "1.0"


/**
 * enumeration of exit status
 */
enum EXITSTATUS
{
	ES_SUCCESS         = 0,
	ES_INVALIDARGUMENT = 1,
	ES_TOOMANYREQUESTS = 2,
	ES_FAILEDTOSAVE    = 3,

	ES_NOPARAMETERS = 255
};

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
	int                 timer_ms;
	size_t              maxRequests;
	int                 quality;
};

#endif // COMMON_H
