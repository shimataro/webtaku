/**
 * Gyotaku object
 * @author shimataro
 */
#include <QApplication>
#include <sysexits.h>
#include "gyotaku.h"
#include "customwebpage.h"
#include "customwebview.h"


Gyotaku::Gyotaku(const PARAMS &params, QObject *parent) : QObject(parent)
{
	// create objects
	CustomWebPage *qWebPage = new CustomWebPage;
	CustomWebView *qWebView = new CustomWebView;
	QTimer        *qTimerDelay   = new QTimer(this);
	QTimer        *qTimerTimeout = new QTimer(this);

	// setup objects
	qWebPage->mainFrame()->setScrollBarPolicy(Qt::Vertical, Qt::ScrollBarAlwaysOff);
	qWebView->setPage(qWebPage);

	QNetworkAccessManager *qNetworkAccessManager = qWebPage->networkAccessManager();

	// setup signal/slot
	connect(qTimerDelay          , SIGNAL(timeout())                                 , SLOT(slot_TimerDelay_timeout()));
	connect(qTimerTimeout        , SIGNAL(timeout())                                 , SLOT(slot_TimerTimeout_timeout()));
	connect(qWebPage             , SIGNAL(loadFinished(bool))                        , SLOT(slot_WebPage_loadFinished(bool)));
	connect(qNetworkAccessManager, SIGNAL(finished(QNetworkReply*))                  , SLOT(slot_NetworkAccessManager_finished(QNetworkReply*)));
	connect(qNetworkAccessManager, SIGNAL(sslErrors(QNetworkReply*,QList<QSslError>)), SLOT(slot_NetworkAccessManager_sslErrors(QNetworkReply*,QList<QSslError>)));

	connect(this, SIGNAL(signal_paramsChanged(PARAMS)), qWebPage, SLOT(slot_Gyotaku_paramsChanged(PARAMS)));
	connect(this, SIGNAL(signal_paramsChanged(PARAMS)), qWebView, SLOT(slot_Gyotaku_paramsChanged(PARAMS)));

	m_qWebPage = qWebPage;
	m_qWebView = qWebView;
	m_qTimerDelay   = qTimerDelay;
	m_qTimerTimeout = qTimerTimeout;
	m_requestCount   = 0;
	m_requestStopped = false;

	m_status = RS_START;
	setParams(params);
}

Gyotaku::~Gyotaku()
{
	delete m_qWebPage; m_qWebPage = NULL;
	delete m_qWebView; m_qWebView = NULL;
	delete m_qTimerDelay; m_qTimerDelay = NULL;
	delete m_qTimerTimeout; m_qTimerTimeout = NULL;
}

/**
 * sets parameters and fires "signal_paramsChanged" signal
 *
 * @param params: parameters
 */
void Gyotaku::setParams(const PARAMS &params)
{
	m_params = params;
	m_request.setRawHeader("Cookie"         , params.cookie);
	m_request.setRawHeader("Accept-Language", params.acceptLanguage);

	emit signal_paramsChanged(params);
}

/**
 * start capturing
 *
 * @param url: URL to be captured
 */
void Gyotaku::rub(const QUrl &url)
{
	// set timeout
	if(m_params.timeout_sec > 0)
	{
		m_qTimerTimeout->start(m_params.timeout_sec * 1000);
	}

	m_requestCount = 0;
	m_request.setUrl(url);
	m_qWebPage->mainFrame()->load(m_request);
}


////////////////////////////////////////////////////////////////////////////////
// private methods

/**
 * get actual image size
 *
 * @returns: image size; at least specified size by minSize
 */
QSize Gyotaku::_getImageSize() const
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

/**
 * scale the image
 *
 * @param pixmap: image to be scaled
 * @returns: scaled image
 */
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

/**
 * output pixmap to file or stdout
 *
 * @param pixmap: image to be output
 * @returns: OK/NG
 */
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

/**
 * stops remaining requests
 *
 * @param status: status to change
 */
void Gyotaku::_stopLoading(const REQUEST_STATUS &status)
{
	m_status = status;
	if(!m_requestStopped)
	{
		m_requestStopped = true;
		m_qWebView->stop();
	}
}

/**
 * is the resource needs to be redirected?
 *
 * @param statusCode: HTTP status code
 * @returns: Yes/No
 */
bool Gyotaku::_needsRedirect(int statusCode)
{
	return (statusCode == 301 || statusCode == 302 || statusCode == 303 || statusCode == 307);
}


////////////////////////////////////////////////////////////////////////////////
// slot methods

/**
 * fired when the page is ready
 * * generate screenshot
 * * scale the image
 * * output the image
 * * exit
 */
void Gyotaku::slot_TimerDelay_timeout()
{
	// get image data
	const QSize imageSize = _getImageSize();
	QPixmap pixmap = QPixmap::grabWidget(m_qWebView, 0, 0, imageSize.width(), imageSize.height());

	// scale
	pixmap = Gyotaku::_scaleImage(pixmap);

	// output
	if(!_outputImage(pixmap))
	{
		qCritical() << "Fatal: failed to save image";
		QApplication::exit(EX_CANTCREAT);
		return;
	}

	if(m_status == RS_TOOMANYREQUESTS)
	{
		qWarning() << "Warning: too many requests";
		QApplication::exit(ES_WARNING_TOOMANYREQUESTS);
		return;
	}
	if(m_status == RS_TIMEOUT)
	{
		qWarning() << "Warning: request timeout";
		QApplication::exit(ES_WARNING_TIMEOUT);
		return;
	}

	qDebug() << "Done.";
	QApplication::quit();
}

/**
 * fired when the page is out of time
 * * set the status to TIEOUT
 * * stop loading page
 */
void Gyotaku::slot_TimerTimeout_timeout()
{
	_stopLoading(RS_TIMEOUT);
}

/**
 * fired when the page has been loaded
 * * set the status to LOADED
 * * wait until the page will be ready
 *
 * @param ok: is the page loaded successfully?
 */
void Gyotaku::slot_WebPage_loadFinished(bool ok)
{
	if(ok)
	{
		m_status = RS_LOADED;
	}

	m_qTimerTimeout->stop();

	m_qTimerDelay->setSingleShot(true);
	m_qTimerDelay->start(m_params.delay_ms);
}

/**
 * fired when a resource has been loaded
 * * report the resource's information
 * * stop loading page when the number of resource is too many
 *
 * @param reply: loading result
 */
void Gyotaku::slot_NetworkAccessManager_finished(QNetworkReply *reply)
{
	m_requestCount++;
	if(m_params.maxRequests > 0 && m_requestCount > m_params.maxRequests)
	{
		_stopLoading(RS_TOOMANYREQUESTS);
		return;
	}

	const QString url         = reply->url().toString();
	const QString statusCode  = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toString();
	const QString contentType = reply->header(QNetworkRequest::ContentTypeHeader).toString();
	qDebug().nospace() << m_requestCount << '\t' << qPrintable(url) << '\t' << "status=" << qPrintable(statusCode) << '\t' << "content-type=" << qPrintable(contentType);
}

/**
 * fired when SSL error has occurred
 * * ignore the error
 *
 * @param reply: loading result
 * @param errors: error list
 */
void Gyotaku::slot_NetworkAccessManager_sslErrors(QNetworkReply *reply, const QList<QSslError> & /* errors */)
{
	reply->ignoreSslErrors();
}
