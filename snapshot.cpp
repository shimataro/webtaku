/**
 * Snapshot object
 * @author shimataro
 */
#include <QApplication>
#include "snapshot.h"
#include "customwebpage.h"


Snapshot::Snapshot(QObject *parent) : QObject(parent)
{
	m_requestCount = 0;

	m_qWebPage = NULL;
	m_qWebView = NULL;
	m_qTimer = NULL;
}

Snapshot::~Snapshot()
{
	delete m_qWebPage; m_qWebPage = NULL;
	delete m_qWebView; m_qWebView = NULL;
	delete m_qTimer; m_qTimer = NULL;
}

void Snapshot::shot(const QUrl &url, const SNAPSHOTPARAMS &params)
{
	m_params = params;

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

	// get image data
	QPixmap pix = QPixmap::grabWidget(m_qWebView, 0, 0, size.width(), size.height());

	// scale
	const QSize &scaledSize = m_params.scaledSize;
	if(!scaledSize.isNull())
	{
		if(scaledSize.width() == 0)
		{
			pix = pix.scaledToHeight(scaledSize.height(), Qt::SmoothTransformation);
		}
		else if(scaledSize.height() == 0)
		{
			pix = pix.scaledToWidth (scaledSize.width() , Qt::SmoothTransformation);
		}
		else
		{
			const Qt::AspectRatioMode aspectRatioMode = m_params.scaleMax ? Qt::KeepAspectRatioByExpanding : Qt::KeepAspectRatio;
			pix = pix.scaled(scaledSize , aspectRatioMode, Qt::SmoothTransformation);
		}
	}

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
