/**
 * customized QWebPage
 */
#include "customwebpage.h"

CustomWebPage::CustomWebPage(QObject *parent) : QWebPage(parent)
{
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


////////////////////////////////////////////////////////////////////////////////
// slot methods

bool CustomWebPage::shouldInterruptJavaScript()
{
	return true;
}

void CustomWebPage::slot_Gyotaku_paramsChanged(const PARAMS &params)
{
	// User-Agent
	m_userAgent = params.userAgent;

	// viewport size
	setViewportSize(params.minSize);

	// default encoding
	settings()->setDefaultTextEncoding(params.defaultEncoding);
}
