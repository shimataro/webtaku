//    Webimage - Website to image render tool
//    Copyright (C) 2012  Raphael Cruzeiro

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

int main(int argc, char *argv[])
{
	if(argc < 2) {
		std::cerr << "Usage: webimage [--format=<BMP|JPEG|PNG|PPM|XBM|XPM>] [--min-width=<minimum-width>] [--min-height=<minimun-height>] [--timer=<milliseconds>] <url>" << std::endl;
		return -1;
	}

	QApplication a(argc, argv);

	QUrl    url;
	QString format = "PPM";
	QSize   minSize(1024, 768);
	int     timer = 3;

	QRegExp regexp_format    ("--format=(\\w+)");
	QRegExp regexp_min_width ("--min-width=(\\d+)");
	QRegExp regexp_min_height("--min-height=(\\d+)");
	QRegExp regexp_timer     ("--timer=(\\d+)");

	for(int i = 0; i < argc; i++) {
		QString arg = argv[i];
		if(regexp_format.exactMatch(arg)) {
			format = regexp_format.cap(1);
		} else if(regexp_min_width.exactMatch(arg)) {
			minSize.setWidth(regexp_min_width.cap(1).toInt());
		} else if(regexp_min_height.exactMatch(arg)) {
			minSize.setHeight(regexp_min_height.cap(1).toInt());
		} else if(regexp_timer.exactMatch(arg)) {
			timer = regexp_timer.cap(1).toInt();
		} else {
			url = QUrl(arg);
		}
	}

	Snapshot shot;
	shot.shot(url, format, minSize, timer);

	return a.exec();
}
