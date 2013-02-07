/**
 * Customized QWebPage
 * @author shimataro
 * <ul>
 *  <li>customized user-agent</li>
 *  <li>ignore JavaScript alert/confirm</li>
 * </ul>
 */
#ifndef CUSTOMWEBPAGE_H
#define CUSTOMWEBPAGE_H

#include <QtCore>
#include <QtWebKit>
#include <QWebPage>

class CustomWebPage : public QWebPage
{
public:
	CustomWebPage(const QString &userAgent, QObject *parent = NULL);

private:
	QString m_userAgent;

protected:
	QString userAgentForUrl(const QUrl &url) const;
	void javaScriptAlert(QWebFrame *originatingFrame, const QString &msg);
	bool javaScriptConfirm(QWebFrame *originatingFrame, const QString &msg);

public slots:
	bool shouldInterruptJavaScript();
};

#endif // CUSTOMWEBPAGE_H
