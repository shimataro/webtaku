/**
 * customized QWebPage
 */
#include "customwebpage.h"

CustomWebPage::CustomWebPage(QObject *parent) : QWebPage(parent)
{
}

/**
 * returns user agent
 *
 * @param url: URL
 * @returns: user agent
 */
QString CustomWebPage::userAgentForUrl(const QUrl &url) const
{
	if(!m_userAgent.isEmpty())
	{
		return m_userAgent;
	}
	return QWebPage::userAgentForUrl(url);
}

/**
 * This function is called whenever a JavaScript program running inside frame calls the alert() function with the message msg.
 */
void CustomWebPage::javaScriptAlert(QWebFrame * /* originatingFrame */, const QString &msg)
{
	qDebug() << "Alert: " << msg;
}

/**
 * This function is called whenever a JavaScript program running inside frame calls the confirm() function with the message, msg.
 *
 * @returns: whether the user confirms the message
 */
bool CustomWebPage::javaScriptConfirm(QWebFrame * /* originatingFrame */, const QString &msg)
{
	qDebug() << "Confirm: " << msg;
	return false;
}

/**
 * This function is called whenever a JavaScript program running inside frame tries to prompt the user for input. The program may provide an optional message, msg, as well as a default value for the input in defaultValue.
 *
 * @returns: If the prompt was cancelled by the user the implementation should return false; otherwise the result should be written to result and true should be returned. If the prompt was not cancelled by the user, the implementation should return true and the result string must not be null.
 */
bool CustomWebPage::javaScriptPrompt(QWebFrame * /* originatingFrame */, const QString &msg, const QString & /* defaultValue */, QString * /* result */)
{
	qDebug() << "Prompt: " << msg;
	return false;
}


////////////////////////////////////////////////////////////////////////////////
// slot methods

/**
 * fired when a JavaScript is running for a long period of time
 *
 * @returns: interrupt or not
 */
bool CustomWebPage::shouldInterruptJavaScript()
{
	return true;
}

/**
 * fired when the parameter has been changed
 * * set user agent
 * * set viewport size
 * * set default encoding
 *
 * @param params: parameters
 */
void CustomWebPage::slot_Gyotaku_paramsChanged(const PARAMS &params)
{
	// User-Agent
	m_userAgent = params.userAgent;

	// viewport size
	setViewportSize(params.minSize);

	// default encoding
	settings()->setDefaultTextEncoding(params.defaultEncoding);
}
