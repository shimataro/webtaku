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
	QWebView      *m_view;
	QTimer        *m_timer;

	QString m_outputFilename;
	QString m_outputFormat;
	QSize   m_minSize;
	int     m_timer_ms;
	int     m_quality;

	QUrl m_redirectUrl;
	int  m_statusCode;
	int  m_tries;

public:
	Snapshot(QObject *parent = NULL);
	~Snapshot();
	void shot(const QUrl &url, const QString &outputFilename, const QString &outputFormat, const QSize &minSize, const int timer_ms = 3, const int quality = -1);

private:
	QWebView *_getView();
	QTimer   *_getTimer();
	bool      _outputPixmap(const QPixmap &pixmap);

private slots:
	void doneLoading(bool);
	void doneWaiting();
	void gotReply(QNetworkReply *reply);
	void sslErrors(QNetworkReply *reply, const QList<QSslError> &errors);
};

#endif // SNAPSHOT_H
