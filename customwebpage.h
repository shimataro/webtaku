/**
 * Customized QWebPage
 * @author shimataro
 * <ul>
 *  <li>slot for changing parameters</li>
 *  <li>customized user-agent</li>
 *  <li>ignore JavaScript alert/confirm</li>
 * </ul>
 */
#ifndef CUSTOMWEBPAGE_H
#define CUSTOMWEBPAGE_H

#include <QtCore>
#include <QtWebKit>
#include <QWebPage>
#include "common.h"

class CustomWebPage : public QWebPage
{
	Q_OBJECT
public:
	CustomWebPage(QObject *parent = NULL);

protected:
	QString userAgentForUrl(const QUrl &url) const;
	void javaScriptAlert(QWebFrame *originatingFrame, const QString &msg);
	bool javaScriptConfirm(QWebFrame *originatingFrame, const QString &msg);

public slots:
	bool shouldInterruptJavaScript();
	void slot_Gyotaku_paramsChanged(const PARAMS &params);

private:
	QString m_userAgent;
};

#endif // CUSTOMWEBPAGE_H
