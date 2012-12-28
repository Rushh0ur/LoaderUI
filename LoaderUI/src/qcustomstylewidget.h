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

#ifndef QCUSTOMSTYLEWIDGET_H
#define QCUSTOMSTYLEWIDGET_H

#include <QWidget>
#include <QList>
#include <QPair>
#include <QImage>
#include <QMargins>
#include <QPushButton>

class QCustomStyleWidget: public QObject
{
	Q_OBJECT

public:
	 QCustomStyleWidget		(QWidget *parent);
	~QCustomStyleWidget		();

	void centerOnScreen		(int screen = -1);
	int	 getMoveState		(void);

	void					setLayoutDirection	(Qt::LayoutDirection layoutDirection);
	Qt::LayoutDirection		layoutDirection		(void);

protected:
	virtual bool eventFilter		(QObject *obj, QEvent *event);
	virtual void resizeEvent		(QResizeEvent * event);
	virtual void mousePressEvent	(QMouseEvent *event);
	virtual void mouseMoveEvent		(QMouseEvent *event);
	virtual void mouseReleaseEvent	(QMouseEvent *event);
	virtual void paintEvent			(QPaintEvent *event);

private:
	QList<QPushButton*>		pSystemPushButtons;
	QPair<QImage*, QImage*> pCaptionImage;
	QSize					pButtonSize;
	QSize					pButtonMargin;
	int						pButtonSpacing;
	QMargins				pContentMargins;
	int						pResizeGripWidth;
	int						pBorderRadius;

	Qt::LayoutDirection		pLayoutDirection;

	bool					pMaximized;
	QByteArray				pRestoreGeometry;

	QPoint					pOffset;
	QRect					pRect;
	int						pMoving;

	void initInterface				(void);
	void parseStyleSheet			(void);
	void rearangeSystemPushButtons	(void);
	int  checkMouseDragFlag			(const QPoint &pos) const;

public slots:
	void on_systemPushButtonExit_clicked	(void);
	void on_systemPushButtonMaximize_clicked(void);
	void on_systemPushButtonMinimize_clicked(void);
};

#endif // QCUSTOMSTYLEWIDGET_H
