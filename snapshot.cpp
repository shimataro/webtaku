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
#include <QPrinter>
#include "snapshot.h"
#include "customwebpage.h"

#define countof(a) (sizeof(a)/sizeof(a[0]))

Snapshot::Snapshot(QObject *parent) : QObject(parent), page(new CustomWebPage), statusCode(0), tries(0)
{
}

void Snapshot::shot(const QUrl &url, const QString &outputFormat, const QSize &minSize, const int timer_ms, const int quality)
{
	this->minSize = minSize;
	this->quality = quality;
	this->outputFormat = outputFormat.toUpper();
    this->timer_ms = timer_ms;

	qDebug() << "Loading fake UI...";
	view = new QWebView;
	view->setPage(page);
	view->setMinimumSize(minSize);

	timer = new QTimer(this);
	connect(timer, SIGNAL(timeout()), SLOT(doneWaiting()));

	connect(page->networkAccessManager(), SIGNAL(finished(QNetworkReply*)), SLOT(gotReply(QNetworkReply*)));
	connect(page, SIGNAL(loadFinished(bool)), SLOT(doneLoading(bool)));
	connect(page->networkAccessManager(), SIGNAL(sslErrors(QNetworkReply*,QList<QSslError>)), SLOT(sslErrors(QNetworkReply*,QList<QSslError>)));

	page->mainFrame()->load(url);
	page->setViewportSize(minSize);
	page->mainFrame()->setScrollBarPolicy(Qt::Vertical, Qt::ScrollBarAlwaysOff);
}

void Snapshot::doneLoading(bool)
{
	// A reasonable waiting time for any script to execute
	timer->start(timer_ms);
}

void Snapshot::doneWaiting()
{
	if( statusCode != 0 &&
		statusCode != 301 &&
		statusCode != 302 &&
		statusCode != 303
	   ) {

		bool sizeChanged = false;
		const int contentWidth  = page->mainFrame()->contentsSize().width();
		const int contentHeight = page->mainFrame()->contentsSize().height();
		if(minSize.width() < contentWidth) {
			sizeChanged = true;
		}
		if(minSize.height() < contentHeight) {
			sizeChanged = true;
		}
		if(sizeChanged) {
			view->setMinimumSize(contentWidth, contentHeight);
			view->repaint();
		}

		QPixmap pix = QPixmap::grabWidget(view, 0, 0, contentWidth, contentHeight);

		QFile stdout;
		stdout.open(1, QIODevice::WriteOnly);
		if (pix.save(&stdout, outputFormat.toStdString().c_str(), quality)) {
			qDebug() << "Saved image.";
		} else {
			qDebug() << "Failed to save image.";
		}

		QApplication::quit();
	}
	else if(statusCode != 0) {
		statusCode = 0;
		qDebug() << "Redirecting to: " + redirectUrl.toString();
		if(page->mainFrame()->url().toString().isEmpty()) {
			qDebug() << "about:blank";
			page->mainFrame()->load(this->redirectUrl);
			qDebug() << "Loading";
		}
	}

	// This should ensure that the program never hangs
	if(statusCode == 0) {
		if(tries > 5) {
			qDebug() << "Giving up.";
			QApplication::quit();
		}
		tries++;
	}
}

void Snapshot::gotReply(QNetworkReply *reply)
{
	if(reply->header(QNetworkRequest::ContentTypeHeader).toString().contains(QString("text/html")))
	{
		qDebug() << "Got reply " + reply->url().toString() + " - " + reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toString() + " - " + reply->header(QNetworkRequest::ContentTypeHeader).toString();
	}

	if(reply->header(QNetworkRequest::ContentTypeHeader).toString().contains(QString("text/html")) && statusCode != 200) {
		statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
		redirectUrl = QUrl(reply->header(QNetworkRequest::LocationHeader).toUrl());
	}
}

void Snapshot::sslErrors(QNetworkReply *reply, const QList<QSslError> & /* errors */)
{
	reply->ignoreSslErrors();
}
