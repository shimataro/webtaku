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

#define MAX_REQUESTS 1000

enum SNAPSHOTSTATUS
{
	SSS_TOOMANYREQUESTS = 1,
	SSS_FAILEDTOSAVE    = 2,
};

struct SNAPSHOTPARAMS
{
	QString outputFilename;
	QString outputFormat;
	QString userAgent;
	QSize   minSize;
	bool    crop;
	int     timer_ms;
	size_t  maxRequests;
	int     quality;
};

class Snapshot : QObject
{
	Q_OBJECT

	QWebPage *m_qWebPage;
	QWebView *m_qWebView;
	QTimer   *m_qTimer;

	size_t m_requestCount;

	SNAPSHOTPARAMS m_params;

public:
	Snapshot(QObject *parent = NULL);
	~Snapshot();
	void shot(const QUrl &url, const SNAPSHOTPARAMS &params);

private:
	QWebPage *_getWebPage() const;
	QWebView *_getWebView() const;
	QTimer   *_getTimer();

	bool _doShot();
	bool _outputPixmap(const QPixmap &pixmap) const;
	static bool _needsRedirect(int statusCode);

private slots:
	void doneLoading(bool);
	void doneWaiting();
	void gotReply(QNetworkReply *reply);
	void sslErrors(QNetworkReply *reply, const QList<QSslError> &errors);
};

#endif // SNAPSHOT_H
