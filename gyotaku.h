/**
 * Gyotaku object
 * @author shimataro
 */
#ifndef GYOTAKU_H
#define GYOTAKU_H
#include <QtCore>
#include <QtWebKit>
#include <QImage>
#include <QPainter>
#include <QTimer>
#include "common.h"

class Gyotaku : QObject
{
	Q_OBJECT
public:
	Gyotaku(const PARAMS &params, QObject *parent = NULL);
	~Gyotaku();
	void setParams(const PARAMS &params);
	void rub(const QUrl &url);

private:
	QSize   _getImageSize() const;
	QPixmap _scaleImage (const QPixmap &pixmap) const;
	bool    _outputImage(const QPixmap &pixmap) const;
	static bool _needsRedirect(int statusCode);

private slots:
	void slot_WebPage_loadFinished(bool);
	void slot_Timer_ready();
	void slot_Timer_timeout();
	void slot_NetworkAccessManager_finished(QNetworkReply *reply);
	void slot_NetworkAccessManager_sslErrors(QNetworkReply *reply, const QList<QSslError> &errors);

signals:
	void signal_paramsChanged(const PARAMS &params);

private:
	enum REQUEST_STATUS
	{
		RS_START,
		RS_LOADED,
		RS_TIMEOUT,
		RS_TOOMANYREQUESTS
	};

	QWebPage *m_qWebPage;
	QWebView *m_qWebView;
	QTimer   *m_qTimerReady;
	QTimer   *m_qTimerTimeout;

	QNetworkRequest m_request;
	size_t          m_requestCount;

	REQUEST_STATUS m_status;
	PARAMS m_params;
};

#endif // GYOTAKU_H
