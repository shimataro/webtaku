//    Wiget - Website to image render tool (forked from Webimage)
//    Copyright (C) 2013 Shimataro

//    This program is free software: you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation, either version 3 of the License, or
//    (at your option) any later version.

//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.

//    You should have received a copy of the GNU General Public License
//    along with this program.  If not, see <http://www.gnu.org/licenses/>.

#include "customwebpage.h"

CustomWebPage::CustomWebPage(QObject *parent) : QWebPage(parent) { }

void CustomWebPage::setUserAgent(const QString &userAgent)
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
