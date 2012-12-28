/****************************************************************************
** Copyright 2012 Rushh0ur. All rights reserved.
**
** Redistribution and use in source and binary forms, with or without 
** modification, are permitted provided that the following conditions are met:
**
** 1. Redistributions of source code must retain the above copyright notice, 
**    this list of conditions and the following disclaimer.
**
** 2. Redistributions in binary form must reproduce the above copyright notice,
**    this list of conditions and the following disclaimer in the documentation 
**    and/or other materials provided with the distribution.
**
** THIS SOFTWARE IS PROVIDED BY <COPYRIGHT HOLDER> ''AS IS'' AND ANY EXPRESS OR 
** IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF 
** MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO 
** EVENT SHALL EELI REILIN OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, 
** INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT 
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
** OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF 
** LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING 
** NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
** EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
**
****************************************************************************/

#include <QEvent>
#include <QMouseEvent>
#include <QDesktopServices>
#include <QUrl>
#include "infotab.h"

InfoTab::InfoTab(QWidget *parent)
	: QWidget(parent)
{
	this->ui.setupUi(this);
	this->ui.label->installEventFilter(this);
}

InfoTab::~InfoTab()
{

}

/* ************************************************************************************************
 * Event Handlers
 * ************************************************************************************************ */
bool InfoTab::eventFilter(QObject *obj, QEvent *event)
{
	switch (event->type())
	{
		case QEvent::MouseMove:
			QWidget::mouseMoveEvent(dynamic_cast<QMouseEvent*>(event));
			break;
		case QEvent::MouseButtonRelease:
			QWidget::mouseReleaseEvent(dynamic_cast<QMouseEvent*>(event));
			break;
		default:
			break;
	}
	return QObject::eventFilter(obj, event);
}

/* ************************************************************************************************
 * Slot Handlers
 * ************************************************************************************************ */
void InfoTab::on_label_linkActivated(const QString &link)
{
	QDesktopServices::openUrl(QUrl(link));
}