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

struct SNAPPARAMS
{
	QString outputFilename;
	QString outputFormat;
	QString userAgent;
	QSize   minSize;
	int     timer_ms;
	int     quality;
};

class Snapshot : QObject
{
	Q_OBJECT

	CustomWebPage *m_page;
	QWebView      *m_view;
	QTimer        *m_timer;

	QUrl m_redirectUrl;
	int  m_statusCode;
	int  m_tries;

	SNAPPARAMS m_params;

public:
	Snapshot(QObject *parent = NULL);
	~Snapshot();
	void shot(const QUrl &url, const SNAPPARAMS &params);

private:
	QWebView *_getView();
	QTimer   *_getTimer();
	bool _handleRedirect();
	bool _doShot();
	bool _outputPixmap(const QPixmap &pixmap);
	static bool _needsRedirect(int statusCode);

private slots:
	void doneLoading(bool);
	void doneWaiting();
	void gotReply(QNetworkReply *reply);
	void sslErrors(QNetworkReply *reply, const QList<QSslError> &errors);
};

#endif // SNAPSHOT_H
