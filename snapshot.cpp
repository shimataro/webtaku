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

Snapshot::Snapshot(QObject *parent) : QObject(parent)
{
	m_page       = new CustomWebPage;
	m_statusCode = 0;
	m_tries      = 0;
}

Snapshot::~Snapshot()
{
	delete m_page; m_page  = NULL;
	delete m_view; m_view  = NULL;
	delete m_timer; m_timer = NULL;
}

void Snapshot::shot(const QUrl &url, const SNAPPARAMS &params)
{
	m_params = params;
	m_page->setUserAgent(params.userAgent);

	qDebug() << "Loading fake UI...";
	m_view  = _getView();
	m_timer = _getTimer();

	connect(m_timer, SIGNAL(timeout()), SLOT(doneWaiting()));
	connect(m_page->networkAccessManager(), SIGNAL(finished(QNetworkReply*)), SLOT(gotReply(QNetworkReply*)));
	connect(m_page, SIGNAL(loadFinished(bool)), SLOT(doneLoading(bool)));
	connect(m_page->networkAccessManager(), SIGNAL(sslErrors(QNetworkReply*,QList<QSslError>)), SLOT(sslErrors(QNetworkReply*,QList<QSslError>)));

	// setup page
	m_page->mainFrame()->load(url);
	m_page->setViewportSize(params.minSize);
	m_page->mainFrame()->setScrollBarPolicy(Qt::Vertical, Qt::ScrollBarAlwaysOff);
}


////////////////////////////////////////////////////////////////////////////////
// private methods

QWebView *Snapshot::_getView()
{
	QWebView *view = new QWebView;
	view->setPage(m_page);
	view->setMinimumSize(m_params.minSize);
	return view;
}

QTimer *Snapshot::_getTimer()
{
	QTimer *timer = new QTimer(this);
	return timer;
}

bool Snapshot::_handleRedirect()
{
	// no redirect
	if(!_needsRedirect(m_statusCode))
	{
		m_tries = 0;
		return false;
	}

	// avoid endless redirect loops
	if(m_tries++ > 5)
	{
		qDebug() << "Giving up.";
		QApplication::quit();
		return true;
	}

	qDebug() << "Redirecting to: " + m_redirectUrl.toString();
	if(m_page->mainFrame()->url().toString().isEmpty())
	{
		qDebug() << "about:blank";
		m_page->mainFrame()->load(this->m_redirectUrl);
		qDebug() << "Loading";
	}
	return true;
}

bool Snapshot::_doShot()
{
	// resize view
	const QSize contentsSize  = m_page->mainFrame()->contentsSize();
	if(m_params.minSize.width() < contentsSize.width() || m_params.minSize.height() < contentsSize.height())
	{
		m_view->setMinimumSize(contentsSize);
		m_view->repaint();
	}

	// output image data
	QPixmap pix = QPixmap::grabWidget(m_view, 0, 0, contentsSize.width(), contentsSize.height());
	return _outputPixmap(pix);
}

bool Snapshot::_outputPixmap(const QPixmap &pixmap)
{
	if(!m_params.outputFilename.isEmpty())
	{
		return pixmap.save(m_params.outputFilename, NULL, m_params.quality);
	}
	else
	{
		QFile file;
		file.open(stdout, QIODevice::WriteOnly);
		return pixmap.save(&file, qPrintable(m_params.outputFormat), m_params.quality);
	}
}

bool Snapshot::_needsRedirect(int statusCode)
{
	return (statusCode == 301 || statusCode == 302 || statusCode == 303 || statusCode == 307);
}


////////////////////////////////////////////////////////////////////////////////
// event handler

void Snapshot::doneLoading(bool)
{
	// A reasonable waiting time for any script to execute
	m_timer->start(m_params.timer_ms);
}

void Snapshot::doneWaiting()
{
	if(_handleRedirect())
	{
		return;
	}

	if(!_doShot())
	{
		qDebug() << "Failed to save image.";
	}

	QApplication::quit();
}

void Snapshot::gotReply(QNetworkReply *reply)
{
	if(reply->header(QNetworkRequest::ContentTypeHeader).toString().contains(QString("text/html")))
	{
		qDebug() << "Got reply " + reply->url().toString() + " - " + reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toString() + " - " + reply->header(QNetworkRequest::ContentTypeHeader).toString();
	}

	if(reply->header(QNetworkRequest::ContentTypeHeader).toString().contains(QString("text/html")) && m_statusCode != 200)
	{
		m_statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
		m_redirectUrl = QUrl(reply->header(QNetworkRequest::LocationHeader).toUrl());
	}
}

void Snapshot::sslErrors(QNetworkReply *reply, const QList<QSslError> & /* errors */)
{
	reply->ignoreSslErrors();
}
