/**
 * customized QWebView
 */
#include "customwebview.h"

CustomWebView::CustomWebView(QWidget *parent) : QWebView(parent)
{
}


////////////////////////////////////////////////////////////////////////////////
// slot methods

/**
 * fired when the parameter has been changed
 * * set the view's size
 *
 * @param params: parameters
 */
void CustomWebView::slot_Gyotaku_paramsChanged(const PARAMS &params)
{
	setMinimumSize(params.minSize);
}
