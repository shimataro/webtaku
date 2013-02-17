/**
 * customized QWebView
 */
#include "customwebview.h"

CustomWebView::CustomWebView(QWidget *parent) : QWebView(parent)
{
}


////////////////////////////////////////////////////////////////////////////////
// slot methods

void CustomWebView::slot_Gyotaku_paramsChanged(const PARAMS &params)
{
	setMinimumSize(params.minSize);
}
