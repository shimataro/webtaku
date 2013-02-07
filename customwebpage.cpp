/**
 * customized QWebPage
 */
#include "customwebpage.h"

CustomWebPage::CustomWebPage(const QString &userAgent, QObject *parent) : QWebPage(parent)
{
	m_userAgent = userAgent;
}

QString CustomWebPage::userAgentForUrl(const QUrl &url) const
{
	if(!m_userAgent.isEmpty())
	{
		return m_userAgent;
	}
	return QWebPage::userAgentForUrl(url);
}

void CustomWebPage::javaScriptAlert(QWebFrame * /* originatingFrame */, const QString & /* msg */)
{
}

bool CustomWebPage::javaScriptConfirm(QWebFrame * /* originatingFrame */, const QString & /* msg */)
{
	return false;
}

bool CustomWebPage::shouldInterruptJavaScript()
{
	return true;
}
