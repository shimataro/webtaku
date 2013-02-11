/**
 * Snapshot object
 * @author shimataro
 */
#ifndef SNAPSHOT_H
#define SNAPSHOT_H
#include <QtCore>
#include <QtWebKit>
#include <QImage>
#include <QPainter>
#include <QTimer>
#include "common.h"

class Snapshot : QObject
{
	Q_OBJECT

	QWebPage *m_qWebPage;
	QWebView *m_qWebView;
	QTimer   *m_qTimer;

	size_t m_requestCount;

	PARAMS m_params;

public:
	Snapshot(QObject *parent = NULL);
	~Snapshot();
	void shot(const QUrl &url, const PARAMS &params);

private:
	QWebPage *_getWebPage() const;
	QWebView *_getWebView() const;
	QTimer   *_getTimer();

	QSize   _getImageSize();
	QPixmap _scaleImage (const QPixmap &pixmap) const;
	bool    _outputImage(const QPixmap &pixmap) const;
	static bool _needsRedirect(int statusCode);

private slots:
	void doneLoading(bool);
	void doneWaiting();
	void gotReply(QNetworkReply *reply);
	void sslErrors(QNetworkReply *reply, const QList<QSslError> &errors);
};

#endif // SNAPSHOT_H
