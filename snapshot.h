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

#ifndef SNAPSHOT_H
#define SNAPSHOT_H
#include <QtCore>
#include <QtWebKit>
#include <QImage>
#include <QPainter>
#include <QTimer>

class CustomWebPage;

class Snapshot : QObject
{
	Q_OBJECT

	CustomWebPage *m_page;
	QTimer        *m_timer;
	QWebView      *m_view;
	QString m_outputFilename;
	QString m_outputFormat;
	QUrl    m_redirectUrl;
	QSize   m_minSize;

	int m_statusCode;
    int m_timer_ms;
	int m_tries;
    int m_quality;

public:
	Snapshot(QObject *parent = 0);
	void shot(const QUrl &url, const QString &outputFilename, const QString &outputFormat, const QSize &minSize, const int timer_ms = 3, const int quality = -1);

private:
	bool _outputPixmap(const QPixmap &pixmap);

private slots:
	void doneLoading(bool);
	void doneWaiting();
	void gotReply(QNetworkReply *reply);
	void sslErrors(QNetworkReply *reply, const QList<QSslError> &errors);
};

#endif // SNAPSHOT_H
