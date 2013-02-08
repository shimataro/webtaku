/**
 * main function
 * @author shimataro
 */
#include <qglobal.h>
#include <QApplication>
#include <iostream>
#include "snapshot.h"

static void parseParams(const QStringList &arguments, QUrl &url, SNAPSHOTPARAMS &params);
static void silentMsgHandler(QtMsgType type, const char *msg);

int main(int argc, char *argv[])
{
	if(argc < 2)
	{
		std::cerr << "Usage:" << std::endl;
		std::cerr << argv[0] << " [--format=<BMP|JPEG|PNG|PPM|XBM|XPM>] [--output=<output-filename>] [--user-agent=<user-agent>] [--min-size=<minimum-width>x<minimum-height>] [--scaled-size=<scaled-width>x<scaled-height>] [--crop] [--timer=<milliseconds>] [--max-requests=<max-requests] [--silent] <url>" << std::endl;
		return -1;
	}

	QApplication app(argc, argv);

	QUrl url;
	SNAPSHOTPARAMS params = {"", "PPM", "", QSize(1024, 768), QSize(0, 0), false, false, 3, 1024, -1};
	parseParams(app.arguments(), url, params);

	Snapshot shot;
	shot.shot(url, params);

	return app.exec();
}

/**
 * parse command line parameters
 * @param arguments [in]parameters to be parsed (app.arguments())
 * @param url       [out]target URL
 * @param params    [out]parsed parameters
 */
static void parseParams(const QStringList &arguments, QUrl &url, SNAPSHOTPARAMS &params)
{
	QRegExp regexp_format      ("--format=(\\w+)");
	QRegExp regexp_output      ("--output=(.*)");
	QRegExp regexp_user_agent  ("--user-agent=(.*)");
	QRegExp regexp_min_size    ("--min-size=(\\d+)?([xX])(\\d+)?");
	QRegExp regexp_scaled_size ("--scaled-size=(\\d+)?([xX])(\\d+)?");
	QRegExp regexp_crop        ("--crop");
	QRegExp regexp_timer       ("--timer=(\\d+)");
	QRegExp regexp_max_requests("--max-requests=(\\d+)");
	QRegExp regexp_silent      ("--silent");

	for(QStringList::const_iterator p = arguments.begin(); p != arguments.end(); p++)
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
		url = QUrl(arg);
	}
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
