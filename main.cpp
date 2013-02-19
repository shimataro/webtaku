/**
 * main function
 * @author shimataro
 */
#include <qglobal.h>
#include <QApplication>
#include <iostream>
#include "common.h"
#include "gyotaku.h"

static bool parseParams(const QStringList &arguments, QUrl &url, PARAMS &params);
static void silentMsgHandler(QtMsgType type, const char *msg);

int main(int argc, char *argv[])
{
	if(argc < 2)
	{
		std::cerr << "Usage:" << std::endl;
		std::cerr << argv[0] << " [--format=<BMP|JPEG|PNG|PPM|XBM|XPM>] [--output=<output-filename>] [--user-agent=<user-agent>] [--accept-language=<accept-language>] [--cookie=<cookie>] [--min-size=<minimum-width>x<minimum-height>] [--scaled-size=<scaled-width>x<scaled-height>] [--crop] [--timer=<milliseconds>] [--max-requests=<max-requests>] [--silent] <url>" << std::endl;
		return -1;
	}

	QApplication app(argc, argv);

	QUrl url;
	PARAMS params = {"", "PPM", "", "", "", QSize(1024, 768), QSize(0, 0), false, false, 500, 1024, -1};
	if(!parseParams(app.arguments(), url, params))
	{
		return EC_INVALIDARGUMENT;
	}

	Gyotaku gyotaku(params);
	gyotaku.rub(url);

	return app.exec();
}

/**
 * parse command line parameters
 * @param arguments [in]parameters to be parsed (app.arguments())
 * @param url       [out]target URL
 * @param params    [out]parsed parameters
 */
static bool parseParams(const QStringList &arguments, QUrl &url, PARAMS &params)
{
	bool result = true;

	const QRegExp regexp_format         ("--format=(\\w+)");
	const QRegExp regexp_output         ("--output=(.*)");
	const QRegExp regexp_user_agent     ("--user-agent=(.*)");
	const QRegExp regexp_accept_language("--accept-language=(.*)");
	const QRegExp regexp_cookie         ("--cookie=(.*)");
	const QRegExp regexp_min_size       ("--min-size=(\\d+)?([xX])(\\d+)?");
	const QRegExp regexp_scaled_size    ("--scaled-size=(\\d+)?([xX])(\\d+)?");
	const QRegExp regexp_crop           ("--crop");
	const QRegExp regexp_timer          ("--timer=(\\d+)");
	const QRegExp regexp_max_requests   ("--max-requests=(\\d+)");
	const QRegExp regexp_silent         ("--silent");
	const QRegExp regexp_url            ("^\\w+://");

	QStringList::const_iterator p = arguments.begin();
	while(++p != arguments.end())
	{
		QString arg = *p;
		if(regexp_format.exactMatch(arg))
		{
			params.outputFormat = regexp_format.cap(1);
			continue;
		}
		if(regexp_output.exactMatch(arg))
		{
			params.outputFilename = regexp_output.cap(1);
			continue;
		}
		if(regexp_user_agent.exactMatch(arg))
		{
			params.userAgent = regexp_user_agent.cap(1);
			continue;
		}
		if(regexp_accept_language.exactMatch(arg))
		{
			params.acceptLanguage.append(regexp_accept_language.cap(1) + ",");
			continue;
		}
		if(regexp_cookie.exactMatch(arg))
		{
			params.cookie.append(regexp_cookie.cap(1) + ";");
			continue;
		}
		if(regexp_min_size.exactMatch(arg))
		{
			const int width  = regexp_min_size.cap(1).toInt();
			const int height = regexp_min_size.cap(3).toInt();
			if(width > 0)
			{
				params.minSize.setWidth (width);
			}
			if(height > 0)
			{
				params.minSize.setHeight(height);
			}
			continue;
		}
		if(regexp_scaled_size.exactMatch(arg))
		{
			const int width  = regexp_scaled_size.cap(1).toInt();
			const int height = regexp_scaled_size.cap(3).toInt();
			if(width > 0)
			{
				params.scaledSize.setWidth (width);
			}
			if(height > 0)
			{
				params.scaledSize.setHeight(height);
			}
			params.scaleMax = (regexp_scaled_size.cap(2) == "X");
			continue;
		}
		if(regexp_crop.exactMatch(arg))
		{
			params.crop = true;
			continue;
		}
		if(regexp_timer.exactMatch(arg))
		{
			params.timer_ms = regexp_timer.cap(1).toInt();
			continue;
		}
		if(regexp_max_requests.exactMatch(arg))
		{
			params.maxRequests = regexp_max_requests.cap(1).toInt();
			continue;
		}
		if(regexp_silent.exactMatch(arg))
		{
			qInstallMsgHandler(silentMsgHandler);
			continue;
		}

		if(arg[0] == QChar('-'))
		{
			std::cerr << "unknown option: " << qPrintable(arg) << std::endl;
			result = false;
			continue;
		}

		if(regexp_url.indexIn(arg) != 0)
		{
			arg = "http://" + arg;
		}
		url = QUrl(arg);
	}
	return result;
}

/**
 * silent message handler
 * @param type [in]message type (debug/warning/critical/fatal)
 * @param msg  [in]message
 */
static void silentMsgHandler(QtMsgType type, const char * /* msg */)
{
	if(type == QtFatalMsg)
	{
		abort();
	}
}
