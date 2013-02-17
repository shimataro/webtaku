/**
 * Customized QWebView
 * @author shimataro
 * <ul>
 *  <li>slot for changing parameters</li>
 * </ul>
 */
#ifndef CUSTOMWEBVIEW_H
#define CUSTOMWEBVIEW_H

#include <QtCore>
#include <QtWebKit>
#include <QWebView>
#include "common.h"

class CustomWebView : public QWebView
{
	Q_OBJECT
public:
	CustomWebView(QWidget *parent = NULL);
	
signals:
	
public slots:
	void slot_Gyotaku_paramsChanged(const PARAMS &params);
};

#endif // CUSTOMWEBVIEW_H
