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

#include <QPainter>
#include <QImage>
#include <QEvent>
#include <QToolButton>
#include <QTabBar>
#include <QScrollBar>
#include <QMouseEvent>
#include <QDebug>
#include "qjsontranslator.h"
#include "loadermainwin.h"
#include "infotab.h"
#include "configtab.h"
#include "containertab.h"

LoaderMainWin::LoaderMainWin(QWidget *parent, Qt::WFlags flags)
	: QWidget(parent, flags), pCustomStyleWidget(NULL),
	  pLogoImage(NULL), pLogoImageArea(NULL, NULL), pLogoMirrored(false),
	  pShowPluginArea(false)
{
	this->ui.setupUi(this);
	this->pCustomStyleWidget = new QCustomStyleWidget(this);

	this->initLogoImages();
	this->initTray();	
	this->setPluginAreaVisibility();
	this->adjustPluginAreaScrollStep();
	this->checkLayoutDirection();
	this->addTabWidget<ContainerTab>();
	this->hideTabCloseButtons();
}

LoaderMainWin::~LoaderMainWin()
{
	delete this->pTrayIcon;

	delete this->pLogoImageArea.second;
	delete this->pLogoImageArea.first;
	delete this->pLogoImage;

	delete this->pCustomStyleWidget;
}

/* ************************************************************************************************
 * Help Functions
 * ************************************************************************************************ */
void LoaderMainWin::checkLayoutDirection(void)
{
	if ( ((this->layoutDirection() == Qt::LeftToRight) && (this->pLogoMirrored == true)) ||
		 ((this->layoutDirection() == Qt::RightToLeft) && (this->pLogoMirrored == false))  )
	{
		if (this->pLogoImageArea.first)
			*this->pLogoImageArea.first = this->pLogoImageArea.first->mirrored(true, false);
		if (this->pLogoImageArea.second)
			*this->pLogoImageArea.second = this->pLogoImageArea.second->mirrored(true, false);

		this->pLogoMirrored = !(this->layoutDirection() == Qt::LeftToRight);
	}
}

void LoaderMainWin::initTray(void)
{
	this->pTrayIcon = new QSystemTrayIcon(QIcon(":/user/Resources/icon.png"));
    this->pTrayIcon->show();
	this->pTrayIcon->setToolTip(this->windowTitle());
}

void LoaderMainWin::initLogoImages(void)
{
	this->pLogoImage = new QImage(":/user/Resources/logo.png");
	this->pLogoImageArea.first = new QImage(":/user/Resources/logoArea.png");
	this->pLogoImageArea.second = new QImage(":/user/Resources/logoAreaDisable.png");
}

void LoaderMainWin::hideTabCloseButtons(int index)
{
	QTabBar *tabBar = this->ui.tabWidget->findChild<QTabBar*>();
	if (!tabBar)
		return;

	QList<QAbstractButton*> list = tabBar->findChildren<QAbstractButton*>();
	QList<QAbstractButton*> closeButtonsList;
	
	foreach (QObject *obj, list)
	{
		if (obj->inherits("CloseButton"))
			closeButtonsList.push_back(qobject_cast<QAbstractButton*>(obj));
	}

	if (index == -1)
	foreach(QAbstractButton *obj, closeButtonsList)
		obj->resize(0, 0);
}

void LoaderMainWin::checkResizeVisibility(void)
{
	// TODO: find good heights
	bool canShowPlugins = this->height() > 200 + this->ui.scrollAreaPlugins->height();

	if (canShowPlugins)
	{
		this->setPluginAreaVisibility();
		this->ui.pushButtonPlugin->show();
	}
	else
	{
		this->ui.scrollAreaPlugins->hide();
		this->ui.pushButtonPlugin->hide();
	}

	// this->ui.tabWidget->setVisible(this->height() > this->ui.tabWidget->minimumHeight()-5);
	// this->ui.widgetFoot->setFixedHeight(this->height() > 100 ? 16 : 0);
}

void LoaderMainWin::setPluginAreaVisibility(void)
{
	if (this->pShowPluginArea)
	{
		this->ui.pushButtonPlugin->setIcon(QIcon(":/system/Resources/systemDown.png"));
		this->ui.scrollAreaPlugins->show();
	}
	else
	{
		this->ui.pushButtonPlugin->setIcon(QIcon(":/system/Resources/systemUp.png"));
		this->ui.scrollAreaPlugins->hide();
	}
}

void LoaderMainWin::adjustPluginAreaScrollStep (void)
{
	QScrollBar *scrollBar;
	
	scrollBar = this->ui.scrollAreaPlugins->verticalScrollBar();
	if (scrollBar)
	{
		scrollBar->setSingleStep(32);
		scrollBar->setPageStep(32);
	}
}

template<typename T>
T* LoaderMainWin::addTabWidget(const QString &aName)
{
	T* tab = this->ui.tabWidget->findChild<T*>(aName);

	if (!tab)
	{
		tab = new T(this);
		this->ui.tabWidget->addTab(tab, tab->windowTitle());
	}
	
	if (tab)
	{
		int index = this->ui.tabWidget->indexOf(tab);
		this->ui.tabWidget->setCurrentIndex(index);
	}

	return tab;
}

/* ************************************************************************************************
 * Event Handlers
 * ************************************************************************************************ */
void LoaderMainWin::closeEvent(QCloseEvent *event)
{
	Q_UNUSED(event);
}

void LoaderMainWin::resizeEvent(QResizeEvent *event)
{
	this->checkResizeVisibility();
}

void LoaderMainWin::mouseMoveEvent(QMouseEvent *event)
{
	// do not change mouse cursor on the bottom plugin button
	if ( (!this->pCustomStyleWidget->getMoveState()) && 
		 (this->childAt(event->pos()) == this->ui.pushButtonPlugin) )
		QApplication::restoreOverrideCursor();
}

void LoaderMainWin::paintEvent(QPaintEvent *event)
{
	QPainter tPainter(this);

	QImage *tLogoImageArea = this->isActiveWindow() ? this->pLogoImageArea.first : this->pLogoImageArea.second;
	
	if (tLogoImageArea)
	{
		QRect srcRect = tLogoImageArea->rect();
		QRect dstRect = srcRect;

		if (this->layoutDirection() == Qt::RightToLeft)
		{
			dstRect.moveTopRight(QPoint(this->width()-1, 21));
			tPainter.drawImage(dstRect, *tLogoImageArea, srcRect);
		}
		else
		{
			dstRect.moveTopLeft(QPoint(1, 21));
			tPainter.drawImage(dstRect, *tLogoImageArea, srcRect);
		}
	}

	if (this->pLogoImage)
	{
		if (this->layoutDirection() == Qt::RightToLeft)
			tPainter.drawImage(this->width() - this->pLogoImage->width()-12, 5, *this->pLogoImage);
		else
			tPainter.drawImage(12, 5, *this->pLogoImage);
	}
}

void LoaderMainWin::changeEvent(QEvent *event)
{
	QWidget::changeEvent(event);
	if(event)
    {
		switch(event->type())
        {
			case QEvent::LayoutDirectionChange:
				this->checkLayoutDirection();
				this->pCustomStyleWidget->setLayoutDirection(this->layoutDirection());
				break;
			// this event is send if a translator is loaded
			case QEvent::LanguageChange:
				this->ui.retranslateUi(this);
			break;

			// this event is send, if the system, language changes
			case QEvent::LocaleChange:
				QJsonTranslator::loadLanguage();	
			break;
			default:
			break;
        }
	}
}

/* ************************************************************************************************
 * Slot Handlers
 * ************************************************************************************************ */
void LoaderMainWin::messageFromSecondInstance(const QString& message)
{
	Q_UNUSED(message);
	this->show();
}

void LoaderMainWin::on_pushButtonPlay_clicked(void)
{
}

void LoaderMainWin::on_pushButtonPause_clicked(void)
{
}

void LoaderMainWin::on_pushButtonStop_clicked(void)
{
}

void LoaderMainWin::on_pushButtonAdd_clicked(void)
{
}

void LoaderMainWin::on_pushButtonInfo_clicked(void)
{
	this->addTabWidget<InfoTab>();
}

void LoaderMainWin::on_pushButtonConfig_clicked(void)
{
	this->addTabWidget<ConfigTab>();
}

void LoaderMainWin::on_pushButtonPlugin_clicked(void)
{	
	this->pShowPluginArea = !this->pShowPluginArea;
	this->setPluginAreaVisibility();
}

void LoaderMainWin::on_tabWidget_tabCloseRequested(int index)
{
	QWidget *widget = this->ui.tabWidget->widget(index);
	this->ui.tabWidget->removeTab(index);
	if (widget)
		widget->deleteLater();
}