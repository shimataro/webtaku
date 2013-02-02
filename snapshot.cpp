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

Snapshot::Snapshot(QObject *parent) : QObject(parent), m_page(new CustomWebPage), m_statusCode(0), m_tries(0)
{
}

void Snapshot::shot(const QUrl &url, const QString &output, const QString &outputFormat, const QSize &minSize, const int timer_ms, const int quality)
{
	this->m_minSize = minSize;
	this->m_quality = quality;
	this->m_output  = output;
	this->m_outputFormat = outputFormat.toUpper();
    this->m_timer_ms = timer_ms;

	qDebug() << "Loading fake UI...";
	m_view = new QWebView;
	m_view->setPage(m_page);
	m_view->setMinimumSize(minSize);

	m_timer = new QTimer(this);
	connect(m_timer, SIGNAL(timeout()), SLOT(doneWaiting()));

	connect(m_page->networkAccessManager(), SIGNAL(finished(QNetworkReply*)), SLOT(gotReply(QNetworkReply*)));
	connect(m_page, SIGNAL(loadFinished(bool)), SLOT(doneLoading(bool)));
	connect(m_page->networkAccessManager(), SIGNAL(sslErrors(QNetworkReply*,QList<QSslError>)), SLOT(sslErrors(QNetworkReply*,QList<QSslError>)));

	m_page->mainFrame()->load(url);
	m_page->setViewportSize(minSize);
	m_page->mainFrame()->setScrollBarPolicy(Qt::Vertical, Qt::ScrollBarAlwaysOff);
}

void Snapshot::doneLoading(bool)
{
	// A reasonable waiting time for any script to execute
	m_timer->start(m_timer_ms);
}

void Snapshot::doneWaiting()
{
	if( m_statusCode != 0 &&
		m_statusCode != 301 &&
		m_statusCode != 302 &&
		m_statusCode != 303
	   )
	{
		bool sizeChanged = false;
		const int contentWidth  = m_page->mainFrame()->contentsSize().width();
		const int contentHeight = m_page->mainFrame()->contentsSize().height();
		if(m_minSize.width() < contentWidth)
		{
			sizeChanged = true;
		}
		if(m_minSize.height() < contentHeight)
		{
			sizeChanged = true;
		}
		if(sizeChanged)
		{
			m_view->setMinimumSize(contentWidth, contentHeight);
			m_view->repaint();
		}

		QPixmap pix = QPixmap::grabWidget(m_view, 0, 0, contentWidth, contentHeight);

		// output image data
		const char *format = m_outputFormat.toStdString().c_str();
		bool result = true;
		if(m_output.length() > 0)
		{
			result = pix.save(m_output, format, m_quality);
		}
		else
		{
			QFile file;
			file.open(stdout, QIODevice::WriteOnly);
			result = pix.save(&file, format, m_quality);
		}

		if (!result)
		{
			qDebug() << "Failed to save image.";
		}

		QApplication::quit();
	}
	else if(m_statusCode != 0) {
		m_statusCode = 0;
		qDebug() << "Redirecting to: " + m_redirectUrl.toString();
		if(m_page->mainFrame()->url().toString().isEmpty()) {
			qDebug() << "about:blank";
			m_page->mainFrame()->load(this->m_redirectUrl);
			qDebug() << "Loading";
		}
	}

	// This should ensure that the program never hangs
	if(m_statusCode == 0) {
		if(m_tries > 5) {
			qDebug() << "Giving up.";
			QApplication::quit();
		}
		m_tries++;
	}
}

void Snapshot::gotReply(QNetworkReply *reply)
{
	if(reply->header(QNetworkRequest::ContentTypeHeader).toString().contains(QString("text/html")))
	{
		qDebug() << "Got reply " + reply->url().toString() + " - " + reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toString() + " - " + reply->header(QNetworkRequest::ContentTypeHeader).toString();
	}

	if(reply->header(QNetworkRequest::ContentTypeHeader).toString().contains(QString("text/html")) && m_statusCode != 200) {
		m_statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
		m_redirectUrl = QUrl(reply->header(QNetworkRequest::LocationHeader).toUrl());
	}
}

void Snapshot::sslErrors(QNetworkReply *reply, const QList<QSslError> & /* errors */)
{
	reply->ignoreSslErrors();
}
