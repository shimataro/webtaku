//    Wiget - Website to image render tool (forked from Webimage)
//    Copyright (C) 2013 Shimataro

//    This program is free software: you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation, either version 3 of the License, or
//    (at your option) any later version.

//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.

//    You should have received a copy of the GNU General Public License
//    along with this program.  If not, see <http://www.gnu.org/licenses/>.

#include <QApplication>
#include <QDebug>
#include <iostream>
#include "snapshot.h"

static void parseParams(const QStringList &arguments, QUrl &url, SNAPPARAMS &params);

int main(int argc, char *argv[])
{
	if(argc < 2)
	{
		std::cerr << "Usage:" << std::endl;
		std::cerr << argv[0] << " [--format=<BMP|JPEG|PNG|PPM|XBM|XPM>] [--output=<output-filename>] [--user-agent=<user-agent>] [--min-width=<minimum-width>] [--min-height=<minimun-height>] [--min-size=<minimum-width>x<minimum-height>] [--crop] [--timer=<milliseconds>] <url>" << std::endl;
		return -1;
	}

	QApplication app(argc, argv);

	QUrl url;
	SNAPPARAMS params = {"", "PPM", "", QSize(1024, 768), false, 3, -1};
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
static void parseParams(const QStringList &arguments, QUrl &url, SNAPPARAMS &params)
{
	QRegExp regexp_format    ("--format=(\\w+)");
	QRegExp regexp_output    ("--output=(.*)");
	QRegExp regexp_user_agent("--user-agent=(.*)");
	QRegExp regexp_min_width ("--min-width=(\\d+)");
	QRegExp regexp_min_height("--min-height=(\\d+)");
	QRegExp regexp_min_size  ("--min-size=(\\d+)x(\\d+)");
	QRegExp regexp_crop      ("--crop");
	QRegExp regexp_timer     ("--timer=(\\d+)");

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
		if(regexp_min_width.exactMatch(arg))
		{
			params.minSize.setWidth(regexp_min_width.cap(1).toInt());
			continue;
		}
		if(regexp_min_height.exactMatch(arg))
		{
			params.minSize.setHeight(regexp_min_height.cap(1).toInt());
			continue;
		}
		if(regexp_min_size.exactMatch(arg))
		{
			params.minSize.setWidth (regexp_min_size.cap(1).toInt());
			params.minSize.setHeight(regexp_min_size.cap(2).toInt());
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
		url = QUrl(arg);
	}
}
