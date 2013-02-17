/**
 * Gyotaku object
 * @author shimataro
 */
#include <QApplication>
#include "gyotaku.h"
#include "customwebpage.h"


Gyotaku::Gyotaku(QObject *parent) : QObject(parent)
{
	m_requestCount = 0;

	m_qWebPage = NULL;
	m_qWebView = NULL;
	m_qTimer = NULL;
}

Gyotaku::~Gyotaku()
{
	delete m_qWebPage; m_qWebPage = NULL;
	delete m_qWebView; m_qWebView = NULL;
	delete m_qTimer; m_qTimer = NULL;
}

void Gyotaku::rub(const QUrl &url, const PARAMS &params)
{
	m_params = params;

	m_qWebPage = _createWebPage();
	m_qWebView = _createWebView();
	m_qTimer   = _createTimer();

	// setup signal/slot
	connect(m_qTimer                          , SIGNAL(timeout())                                 , SLOT(slot_Timer_timeout()));
	connect(m_qWebPage                        , SIGNAL(loadFinished(bool))                        , SLOT(slot_WebPage_loadFinished(bool)));
	connect(m_qWebPage->networkAccessManager(), SIGNAL(finished(QNetworkReply*))                  , SLOT(slot_NetworkAccessManager_finished(QNetworkReply*)));
	connect(m_qWebPage->networkAccessManager(), SIGNAL(sslErrors(QNetworkReply*,QList<QSslError>)), SLOT(slot_NetworkAccessManager_sslErrors(QNetworkReply*,QList<QSslError>)));

	QNetworkRequest request(url);
	request.setRawHeader("Cookie", params.cookie);

	// setup page
	m_qWebPage->setViewportSize(params.minSize);
	m_qWebPage->mainFrame()->setScrollBarPolicy(Qt::Vertical, Qt::ScrollBarAlwaysOff);
	m_qWebPage->mainFrame()->load(request);
}


////////////////////////////////////////////////////////////////////////////////
// private methods

QWebPage *Gyotaku::_createWebPage() const
{
	QWebPage *qWebPage = new CustomWebPage(m_params.userAgent);
	return qWebPage;
}

QWebView *Gyotaku::_createWebView() const
{
	QWebView *qWebView = new QWebView;
	qWebView->setPage(m_qWebPage);
	qWebView->setMinimumSize(m_params.minSize);
	return qWebView;
}

QTimer *Gyotaku::_createTimer()
{
	QTimer *qTimer = new QTimer(this);
	return qTimer;
}

QSize Gyotaku::_getImageSize()
{
	const QSize &size = m_params.minSize;
	if(m_params.crop)
	{
		return size;
	}

	// resize view
	const QSize contentsSize  = m_qWebPage->mainFrame()->contentsSize();
	if(size.width() < contentsSize.width() || size.height() < contentsSize.height())
	{
		m_qWebView->setMinimumSize(contentsSize);
		m_qWebView->repaint();
	}
	return contentsSize;
}

QPixmap Gyotaku::_scaleImage(const QPixmap &pixmap) const
{
	const QSize &scaledSize = m_params.scaledSize;
	if(scaledSize.isNull())
	{
		// do not scale
		return pixmap;
	}

	if(scaledSize.width() == 0)
	{
		return pixmap.scaledToHeight(scaledSize.height(), Qt::SmoothTransformation);
	}
	if(scaledSize.height() == 0)
	{
		return pixmap.scaledToWidth (scaledSize.width() , Qt::SmoothTransformation);
	}

	const Qt::AspectRatioMode aspectRatioMode = m_params.scaleMax ? Qt::KeepAspectRatioByExpanding : Qt::KeepAspectRatio;
	return pixmap.scaled(scaledSize, aspectRatioMode, Qt::SmoothTransformation);
}

bool Gyotaku::_outputImage(const QPixmap &pixmap) const
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

bool Gyotaku::_needsRedirect(int statusCode)
{
	return (statusCode == 301 || statusCode == 302 || statusCode == 303 || statusCode == 307);
}


////////////////////////////////////////////////////////////////////////////////
// slot methods

void Gyotaku::slot_Timer_timeout()
{
	// get image data
	const QSize imageSize = _getImageSize();
	QPixmap pixmap = QPixmap::grabWidget(m_qWebView, 0, 0, imageSize.width(), imageSize.height());

	// scale
	pixmap = Gyotaku::_scaleImage(pixmap);

	// output
	if(!_outputImage(pixmap))
	{
		qCritical() << "Fatal error: failed to save image";
		QApplication::exit(EC_FAILEDTOSAVE);
		return;
	}

	qDebug() << "Done.";
	QApplication::quit();
}

void Gyotaku::slot_WebPage_loadFinished(bool)
{
	// A reasonable waiting time for any script to execute
	m_qTimer->start(m_params.timer_ms);
}

void Gyotaku::slot_NetworkAccessManager_finished(QNetworkReply *reply)
{
	const QString statusCode  = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toString();
	const QString contentType = reply->header(QNetworkRequest::ContentTypeHeader).toString();
	qDebug() << "Got reply " + reply->url().toString() + " - " + statusCode + " - " + contentType;

	if(m_requestCount++ > m_params.maxRequests)
	{
		qCritical() << "Fatal error: too many requests";
		QApplication::exit(EC_TOOMANYREQUESTS);
		return;
	}
}

void Gyotaku::slot_NetworkAccessManager_sslErrors(QNetworkReply *reply, const QList<QSslError> & /* errors */)
{
	reply->ignoreSslErrors();
}
