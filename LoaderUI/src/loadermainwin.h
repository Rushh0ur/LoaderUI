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

#ifndef LOADERMAINWIN_H
#define LOADERMAINWIN_H

#include <QtGui/QMainWindow>
#include <QSystemTrayIcon>
#include "ui_loadermainwin.h"
#include "qcustomstylewidget.h"

class LoaderMainWin : public QWidget
{
	Q_OBJECT

public:
	 LoaderMainWin(QWidget *parent = 0, Qt::WFlags flags = 0);
	~LoaderMainWin();

protected:
	virtual void closeEvent			(QCloseEvent *event);
	virtual void resizeEvent		(QResizeEvent *event);
	virtual void mouseMoveEvent		(QMouseEvent *event);
	virtual void paintEvent			(QPaintEvent *event);
	virtual void changeEvent		(QEvent *event);
private:
	Ui::LoaderMainWin		ui;
	QCustomStyleWidget*		pCustomStyleWidget;
	QSystemTrayIcon*		pTrayIcon;

	QImage*					pLogoImage;
	QPair<QImage*, QImage*> pLogoImageArea;
	bool					pLogoMirrored;

	bool					pShowPluginArea;

	void checkLayoutDirection		(void);
	void initTray					(void);
	void initLogoImages				(void);
	void hideTabCloseButtons		(int index = -1);
	void checkResizeVisibility		(void);
	void setPluginAreaVisibility	(void);
	void adjustPluginAreaScrollStep (void);

	template<typename T>
    T* addTabWidget(const QString &aName = QString());

public slots:
	void messageFromSecondInstance		(const QString& message);
	void on_pushButtonPlay_clicked		(void);
	void on_pushButtonPause_clicked		(void);
	void on_pushButtonStop_clicked		(void);
	void on_pushButtonAdd_clicked		(void);
	void on_pushButtonInfo_clicked		(void);
	void on_pushButtonConfig_clicked	(void);
	void on_pushButtonPlugin_clicked	(void);
	void on_tabWidget_tabCloseRequested (int index);
};

#endif // LOADERMAINWIN_H
