/**
 * Gyotaku object
 * @author shimataro
 */
#include <QApplication>
#include "gyotaku.h"
#include "customwebpage.h"
#include "customwebview.h"


Gyotaku::Gyotaku(const PARAMS &params, QObject *parent) : QObject(parent)
{
	// create objects
	CustomWebPage *qWebPage = new CustomWebPage;
	CustomWebView *qWebView = new CustomWebView;
	QTimer        *qTimer   = new QTimer(this);

	// setup objects
	qWebPage->mainFrame()->setScrollBarPolicy(Qt::Vertical, Qt::ScrollBarAlwaysOff);
	qWebView->setPage(qWebPage);

	QNetworkAccessManager *qNetworkAccessManager = qWebPage->networkAccessManager();

	// setup signal/slot
	connect(qTimer               , SIGNAL(timeout())                                 , SLOT(slot_Timer_timeout()));
	connect(qWebPage             , SIGNAL(loadFinished(bool))                        , SLOT(slot_WebPage_loadFinished(bool)));
	connect(qNetworkAccessManager, SIGNAL(finished(QNetworkReply*))                  , SLOT(slot_NetworkAccessManager_finished(QNetworkReply*)));
	connect(qNetworkAccessManager, SIGNAL(sslErrors(QNetworkReply*,QList<QSslError>)), SLOT(slot_NetworkAccessManager_sslErrors(QNetworkReply*,QList<QSslError>)));

	connect(this, SIGNAL(signal_paramsChanged(PARAMS)), qWebPage, SLOT(slot_Gyotaku_paramsChanged(PARAMS)));
	connect(this, SIGNAL(signal_paramsChanged(PARAMS)), qWebView, SLOT(slot_Gyotaku_paramsChanged(PARAMS)));

	m_qWebPage = qWebPage;
	m_qWebView = qWebView;
	m_qTimer   = qTimer;
	m_requestCount = 0;
	m_loaded = false;

	setParams(params);
}

Gyotaku::~Gyotaku()
{
	delete m_qWebPage; m_qWebPage = NULL;
	delete m_qWebView; m_qWebView = NULL;
	delete m_qTimer; m_qTimer = NULL;
}

void Gyotaku::setParams(const PARAMS &params)
{
	m_params = params;
	m_request.setRawHeader("Cookie"         , params.cookie);
	m_request.setRawHeader("Accept-Language", params.acceptLanguage);

	emit signal_paramsChanged(params);
}

void Gyotaku::rub(const QUrl &url)
{
	// set timeout
	if(m_params.timeout_sec > 0)
	{
		m_qTimer->start(m_params.timeout_sec * 1000);
	}

	m_requestCount = 0;
	m_request.setUrl(url);
	m_qWebPage->mainFrame()->load(m_request);
}


////////////////////////////////////////////////////////////////////////////////
// private methods

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

	return pixmap.scaled(scaledSize, m_params.aspectRatioMode, Qt::SmoothTransformation);
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
		QApplication::exit(ES_FAILEDTOSAVE);
		return;
	}

	// timeout
	if(!m_loaded)
	{
		qCritical() << "Fatal error: request timeout";
		QApplication::exit(ES_TIMEOUT);
		return;
	}

	qDebug() << "Done.";
	QApplication::quit();
}

void Gyotaku::slot_WebPage_loadFinished(bool)
{
	// A reasonable waiting time for any script to execute
	m_loaded = true;
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
		QApplication::exit(ES_TOOMANYREQUESTS);
		return;
	}
}

void Gyotaku::slot_NetworkAccessManager_sslErrors(QNetworkReply *reply, const QList<QSslError> & /* errors */)
{
	reply->ignoreSslErrors();
}
