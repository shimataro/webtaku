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


Snapshot::Snapshot(QObject *parent) : QObject(parent)
{
	m_requestCount = 0;

	m_qWebPage  = NULL;
	m_qWebView  = NULL;
	m_qTimer = NULL;
}

Snapshot::~Snapshot()
{
	delete m_qWebPage; m_qWebPage  = NULL;
	delete m_qWebView; m_qWebView  = NULL;
	delete m_qTimer; m_qTimer = NULL;
}

void Snapshot::shot(const QUrl &url, const SNAPSHOTPARAMS &params)
{
	m_params = params;

	qDebug() << "Loading fake UI...";
	m_qWebPage = _getWebPage();
	m_qWebView = _getWebView();
	m_qTimer   = _getTimer();

	connect(m_qTimer, SIGNAL(timeout()), SLOT(doneWaiting()));
	connect(m_qWebPage->networkAccessManager(), SIGNAL(finished(QNetworkReply*)), SLOT(gotReply(QNetworkReply*)));
	connect(m_qWebPage, SIGNAL(loadFinished(bool)), SLOT(doneLoading(bool)));
	connect(m_qWebPage->networkAccessManager(), SIGNAL(sslErrors(QNetworkReply*,QList<QSslError>)), SLOT(sslErrors(QNetworkReply*,QList<QSslError>)));

	// setup page
	m_qWebPage->mainFrame()->load(url);
	m_qWebPage->setViewportSize(params.minSize);
	m_qWebPage->mainFrame()->setScrollBarPolicy(Qt::Vertical, Qt::ScrollBarAlwaysOff);
}


////////////////////////////////////////////////////////////////////////////////
// private methods

QWebPage *Snapshot::_getWebPage() const
{
	QWebPage *qWebPage = new CustomWebPage(m_params.userAgent);
	return qWebPage;
}

QWebView *Snapshot::_getWebView() const
{
	QWebView *qWebView = new QWebView;
	qWebView->setPage(m_qWebPage);
	qWebView->setMinimumSize(m_params.minSize);
	return qWebView;
}

QTimer *Snapshot::_getTimer()
{
	QTimer *qTimer = new QTimer(this);
	return qTimer;
}


bool Snapshot::_doShot()
{
	QSize size = m_params.minSize;
	if(!m_params.crop)
	{
		// resize view
		const QSize contentsSize  = m_qWebPage->mainFrame()->contentsSize();
		if(size.width() < contentsSize.width() || size.height() < contentsSize.height())
		{
			size = contentsSize;
			m_qWebView->setMinimumSize(size);
			m_qWebView->repaint();
		}
	}

	// output image data
	QPixmap pix = QPixmap::grabWidget(m_qWebView, 0, 0, size.width(), size.height());
	return _outputPixmap(pix);
}

bool Snapshot::_outputPixmap(const QPixmap &pixmap) const
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
	m_qTimer->start(m_params.timer_ms);
}

void Snapshot::doneWaiting()
{
	if(!_doShot())
	{
		qCritical() << "Fatal error: failed to save image";
		QApplication::exit(SSS_FAILEDTOSAVE);
		return;
	}

	qDebug() << "Done.";
	QApplication::quit();
}

void Snapshot::gotReply(QNetworkReply *reply)
{
	const QString statusCode  = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toString();
	const QString contentType = reply->header(QNetworkRequest::ContentTypeHeader).toString();
	qDebug() << "Got reply " + reply->url().toString() + " - " + statusCode + " - " + contentType;

	if(m_requestCount++ > m_params.maxRequests)
	{
		qCritical() << "Fatal error: too many requests";
		QApplication::exit(SSS_TOOMANYREQUESTS);
		return;
	}
}

void Snapshot::sslErrors(QNetworkReply *reply, const QList<QSslError> & /* errors */)
{
	reply->ignoreSslErrors();
}
