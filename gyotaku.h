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

	QWebPage *m_qWebPage;
	QWebView *m_qWebView;
	QTimer   *m_qTimer;

	size_t m_requestCount;

	PARAMS m_params;

public:
	Gyotaku(QObject *parent = NULL);
	~Gyotaku();
	void rub(const QUrl &url, const PARAMS &params);

private:
	QWebPage *_createWebPage() const;
	QWebView *_createWebView() const;
	QTimer   *_createTimer();

	QSize   _getImageSize();
	QPixmap _scaleImage (const QPixmap &pixmap) const;
	bool    _outputImage(const QPixmap &pixmap) const;
	static bool _needsRedirect(int statusCode);

private slots:
	void slot_WebPage_loadFinished(bool);
	void slot_Timer_timeout();
	void slot_NetworkAccessManager_finished(QNetworkReply *reply);
	void slot_NetworkAccessManager_sslErrors(QNetworkReply *reply, const QList<QSslError> &errors);
};

#endif // GYOTAKU_H
