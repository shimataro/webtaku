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

enum SNAPSHOTSTATUS
{
	SSS_TOOMANYREQUESTS = 1,
	SSS_FAILEDTOSAVE    = 2,
};

struct SNAPSHOTPARAMS
{
	QString outputFilename;
	QString outputFormat;
	QString userAgent;
	QSize   minSize;
	QSize   scaledSize;
	bool    crop;
	int     timer_ms;
	size_t  maxRequests;
	int     quality;
};

class Snapshot : QObject
{
	Q_OBJECT

	QWebPage *m_qWebPage;
	QWebView *m_qWebView;
	QTimer   *m_qTimer;

	size_t m_requestCount;

	SNAPSHOTPARAMS m_params;

public:
	Snapshot(QObject *parent = NULL);
	~Snapshot();
	void shot(const QUrl &url, const SNAPSHOTPARAMS &params);

private:
	QWebPage *_getWebPage() const;
	QWebView *_getWebView() const;
	QTimer   *_getTimer();

	bool _doShot();
	bool _outputPixmap(const QPixmap &pixmap) const;
	static bool _needsRedirect(int statusCode);

private slots:
	void doneLoading(bool);
	void doneWaiting();
	void gotReply(QNetworkReply *reply);
	void sslErrors(QNetworkReply *reply, const QList<QSslError> &errors);
};

#endif // SNAPSHOT_H
