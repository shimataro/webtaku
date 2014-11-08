/**
 * main function
 * @author shimataro
 */
#include <qglobal.h>
#include <QApplication>
#include <iostream>
#include <sysexits.h>
#include "common.h"
#include "gyotaku.h"

static bool parseParams(const QStringList &arguments, QUrl &url, PARAMS &params);
static void silentMsgHandler(QtMsgType type, const char *msg);
static void version();

int main(int argc, char *argv[])
{
	if(argc < 2)
	{
		std::cerr << "Usage:" << std::endl;
		std::cerr << argv[0] << " [OPTIONS] <url>" << std::endl;
		std::cerr << std::endl;
		std::cerr << "see manpage for more information" << std::endl;
		return EX_USAGE;
	}

	QApplication app(argc, argv);

	QUrl url;
	PARAMS params = {
		"",                             // outputFilename
		"PPM",                          // outputFormat
		"",                             // userAgent
		"UTF-8",                        // defaultEncoding
		"",                             // acceptLanguage
		"",                             // cookie
		QSize(1024, 768),               // minSize
		QSize(0, 0),                    // scaledSize
		Qt::KeepAspectRatio,            // aspectRatioMode
		false,                          // crop
		500,                            // timer_ms
		15,                             // timeout_sec
		100,                            // maxRequests
		-1,                             // quality
	};
	if(!parseParams(app.arguments(), url, params))
	{
		return EX_USAGE;
	}

	Gyotaku gyotaku(params);
	gyotaku.rub(url);

	return app.exec();
}

/**
 * parse command line parameters
 * @param arguments: parameters to be parsed (app.arguments())
 * @param url:       [out]target URL
 * @param params:    [out]parsed parameters
 * @returns: Success/Failure
 */
static bool parseParams(const QStringList &arguments, QUrl &url, PARAMS &params)
{
	bool result = true;

	const QRegExp regexp_format          ("--format=(\\w+)");
	const QRegExp regexp_output          ("--output=(.*)");
	const QRegExp regexp_user_agent      ("--user-agent=(.*)");
	const QRegExp regexp_default_encoding("--default-encoding=(.*)");
	const QRegExp regexp_accept_language ("--accept-language=(.*)");
	const QRegExp regexp_cookie          ("--cookie=(.*)");
	const QRegExp regexp_min_size        ("--min-size=(\\d+)?([xX:])(\\d+)?");
	const QRegExp regexp_scaled_size     ("--scaled-size=(\\d+)?([xX:])(\\d+)?");
	const QRegExp regexp_crop            ("--crop");
	const QRegExp regexp_timer           ("--timer=(\\d+)");
	const QRegExp regexp_timeout         ("--timeout=(\\d+)");
	const QRegExp regexp_max_requests    ("--max-requests=(\\d+)");
	const QRegExp regexp_silent          ("--silent");
	const QRegExp regexp_version         ("--version");
	const QRegExp regexp_url             ("^\\w+://");

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
		if(regexp_default_encoding.exactMatch(arg))
		{
			params.defaultEncoding = regexp_default_encoding.cap(1);
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
			const int     width  = regexp_scaled_size.cap(1).toInt();
			const int     height = regexp_scaled_size.cap(3).toInt();
			const QString mode   = regexp_scaled_size.cap(2);
			if(width > 0)
			{
				params.scaledSize.setWidth (width);
			}
			if(height > 0)
			{
				params.scaledSize.setHeight(height);
			}
			params.aspectRatioMode = Qt::KeepAspectRatio;
			if(mode == "X")
			{
				params.aspectRatioMode = Qt::KeepAspectRatioByExpanding;
			}
			if(mode == ":")
			{
				params.aspectRatioMode = Qt::IgnoreAspectRatio;
			}
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
		if(regexp_timeout.exactMatch(arg))
		{
			params.timeout_sec = regexp_timeout.cap(1).toInt();
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
		if(regexp_version.exactMatch(arg))
		{
			version();
			break;
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

	if(url.isEmpty())
	{
		std::cerr << "URL is not specified" << std::endl;
		result = false;
	}
	return result;
}

/**
 * silent message handler
 *
 * @param type: message type (debug/warning/critical/fatal)
 * @param msg:  message
 */
static void silentMsgHandler(QtMsgType type, const char * /* msg */)
{
	if(type == QtFatalMsg)
	{
		abort();
	}
}

/**
 * displays version information and exit
 */
static void version()
{
	std::cout
		<< SOFTWARE_NAME << " version: " << SOFTWARE_VERSION << std::endl
		<< "    Qt version: " << qVersion() << std::endl
		<< "    WebKit version: " << qWebKitVersion().toStdString() << std::endl
	;
	exit(EXIT_SUCCESS);
}
