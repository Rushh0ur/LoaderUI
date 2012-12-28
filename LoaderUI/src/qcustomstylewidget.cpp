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

#include <QApplication>
#include <QDesktopWidget>
#include <QPainter>
#include <QPaintEvent>
#include <QPixmap>
#include <QBitmap>
#include <QRect>
#include <QDebug>

#include "qcustomstylewidget.h"
#ifdef Q_WS_WIN
	#include <Windows.h>
#endif

#define WDRAG_NONE			0x0000
#define WDRAG_CENTER		0x0001
#define WDRAG_TOP			0x0002
#define WDRAG_RIGHT			0x0004
#define WDRAG_BOTTOM		0x0008
#define WDRAG_LEFT			0x0010

static const char* DYNAMIC_PROPERTY_WINDOW_STATE = "showrestore";


QCustomStyleWidget::QCustomStyleWidget(QWidget *parent)
	: QObject(parent), 
	  pCaptionImage(0, 0),
	  pButtonSize(16, 16), pButtonMargin(4, 2), pButtonSpacing(4), 
	  pContentMargins(5, 24, 5, 5),
	  pResizeGripWidth(5), pBorderRadius(7),
	  pLayoutDirection(Qt::LeftToRight),
	  pMaximized(false),
	  pMoving(WDRAG_NONE)
{	
	if (parent)
		parent->installEventFilter(this);

	this->centerOnScreen();
	this->parseStyleSheet();
	this->initInterface();
	this->rearangeSystemPushButtons();
}

QCustomStyleWidget::~QCustomStyleWidget()
{
	delete this->pCaptionImage.second;
	delete this->pCaptionImage.first;
}

void QCustomStyleWidget::parseStyleSheet(void)
{
	QWidget *parent = qobject_cast<QWidget*>(this->parent());
	if (!parent)
		return;

	QString styleSheet = parent->styleSheet();
	QMap<QString, QString> propertyMap;

	int pos;
	if ((pos = styleSheet.indexOf("QCustomStyleWidget")) >= 0)
	{
		pos = styleSheet.indexOf("{", pos) + 1;
		int end = styleSheet.indexOf("}", pos);
		styleSheet = styleSheet.mid(pos, end-pos);

		int start = 0;
		while ((pos = styleSheet.indexOf(":", start)) != -1) 
		{
			end = styleSheet.indexOf(";", pos);

			if (end != -1)
			{
				QString proprtyName  = styleSheet.mid(start, pos-start).trimmed();
				QString proprtyValue = styleSheet.mid(pos+1, end-pos-1).trimmed();
				propertyMap[proprtyName] = proprtyValue;
			}
			else
				break;
			
			start = end+1;
		}
	}
	
	{
		QRegExp regExp("^([0-9]+)(px)?$");
		QString value;

		value = propertyMap["button-width"];
		if (!value.isEmpty() && (regExp.indexIn(value) != -1))
			this->pButtonSize.setWidth( regExp.cap(1).toInt() );

		value = propertyMap["button-height"];
		if (!value.isEmpty() && (regExp.indexIn(value) != -1))
			this->pButtonSize.setHeight( regExp.cap(1).toInt() );

		value = propertyMap["button-spacing"];
		if (!value.isEmpty() && (regExp.indexIn(value) != -1))
			this->pButtonSpacing = regExp.cap(1).toInt();

		value = propertyMap["top"];
		if (!value.isEmpty() && (regExp.indexIn(value) != -1))
			this->pButtonMargin.setHeight( regExp.cap(1).toInt() );

		value = propertyMap["left"];
		if (!value.isEmpty() && (regExp.indexIn(value) != -1))
			this->pButtonMargin.setWidth( regExp.cap(1).toInt() );

		value = propertyMap["margin"];
		if (!value.isEmpty())
		{
			QStringList list = value.simplified().split(' ');
			int index = 0;
			int values[4] = {0};
			foreach (const QString &item, list)
			{
				if (regExp.indexIn(item) != -1)
				{
					values[index] = regExp.cap(1).toInt();
					++index;
					if (index>3)
						break;
				}
			}

			this->pContentMargins = QMargins(values[0], values[1], values[2], values[3]);
		}

		regExp.setPattern("url\\((.+)\\)");
		
		value = propertyMap["caption-image"];
		if (!value.isEmpty() && (regExp.indexIn(value.replace("'","")) != -1))
		{
			QString name = regExp.cap(1);
			this->pCaptionImage.first = new QImage(name);
		}

		value = propertyMap["caption-image-glow"];
		if (!value.isEmpty() && (regExp.indexIn(value.replace("'","")) != -1))
		{
			QString name = regExp.cap(1);
			this->pCaptionImage.second = new QImage(name);
		}
	}
}

void QCustomStyleWidget::initInterface(void)
{
	QWidget *parent = qobject_cast<QWidget*>(this->parent());
	if (!parent)
		return;

	this->pLayoutDirection = parent->layoutDirection();
	parent->setProperty(DYNAMIC_PROPERTY_WINDOW_STATE, false);
	parent->setWindowFlags(Qt::FramelessWindowHint);
	//parent->setAttribute(Qt::WA_TranslucentBackground);
	parent->setContentsMargins(this->pContentMargins);
	
	// create system buttons
	const char *systemButtonNames[3] = {"systemPushButtonExit", "systemPushButtonMaximize", "systemPushButtonMinimize"};

	for (unsigned int i=0; i<sizeof(systemButtonNames)/sizeof(char*); ++i)
	{
		QPushButton *tSystemPushButton = new QPushButton(parent);
		tSystemPushButton->setObjectName(systemButtonNames[i]);
		tSystemPushButton->setCursor(Qt::ArrowCursor);
		tSystemPushButton->resize(this->pButtonSize); // make this dynamic

		QByteArray connectName = QString("1on_%1_clicked()").arg(systemButtonNames[i]).toAscii();
		this->connect(tSystemPushButton, SIGNAL(clicked()), connectName.data());
		this->pSystemPushButtons.push_back(tSystemPushButton);
	}

	this->pSystemPushButtons[1]->setProperty(DYNAMIC_PROPERTY_WINDOW_STATE, false);

	QWidgetList widgetList = parent->findChildren<QWidget*>();
	foreach (QWidget *widget, widgetList)
		widget->setMouseTracking(true);
}

void QCustomStyleWidget::rearangeSystemPushButtons(void)
{
	QWidget *parent = qobject_cast<QWidget*>(this->parent());
	if (!parent)
		return;

	if (this->pLayoutDirection == Qt::RightToLeft)
	{
		int x = this->pButtonMargin.width();
		for (int i=this->pSystemPushButtons.count()-1; i>=0; --i)
		{
			QPushButton *pushButton = this->pSystemPushButtons[i];
		
			if (!pushButton->isHidden())
			{
				
				pushButton->move(x, this->pButtonMargin.height());	
				x += pushButton->width() + pButtonSpacing;
			}
		}
	}
	else
	{
		int x = parent->width() - this->pButtonMargin.width();
		for (int i=0; i<this->pSystemPushButtons.count(); ++i)
		{
			QPushButton *pushButton = this->pSystemPushButtons[i];
		
			if (!pushButton->isHidden())
			{
				x -= pushButton->width();
				pushButton->move(x, this->pButtonMargin.height());	
				x -= pButtonSpacing;
			}
		}
	}

}

int QCustomStyleWidget::checkMouseDragFlag(const QPoint &pos) const
{
	QWidget *parent = qobject_cast<QWidget*>(this->parent());
	if (!parent)
		return WDRAG_NONE;

	int flag = WDRAG_NONE;
	QRect tRect = parent->geometry();

	// mouse within a edge box
	bool top    = QRect(tRect.left(),  tRect.top(),    tRect.width(), 	 pResizeGripWidth).contains(pos);
	bool bottom = QRect(tRect.left(),  tRect.bottom(), tRect.width(), 	-pResizeGripWidth).contains(pos);
	bool left   = QRect(tRect.left(),  tRect.top(),     pResizeGripWidth, tRect.height()) .contains(pos);
	bool right  = QRect(tRect.right(), tRect.top(),    -pResizeGripWidth, tRect.height()) .contains(pos);

	// evaluate position calculations
	if (top)
		flag |= WDRAG_TOP;
	if (bottom)
		flag |= WDRAG_BOTTOM;
	if (left)
		flag |= WDRAG_LEFT;
	if (right)
		flag |= WDRAG_RIGHT;
	if (!flag)
		flag |= WDRAG_CENTER;

	return flag;
}

void QCustomStyleWidget::centerOnScreen(int screen )
{
	QWidget *parent = qobject_cast<QWidget*>(this->parent());
	if (!parent)
		return;

	QRect fRect = parent->frameGeometry();
	fRect.moveCenter(QDesktopWidget().availableGeometry(screen).center());
	parent->move(fRect.topLeft());
}

int	QCustomStyleWidget::getMoveState(void)
{
	return this->pMoving;
}

bool QCustomStyleWidget::eventFilter(QObject *obj, QEvent *event)
{
	switch (event->type())
	{
		case QEvent::Resize:
			this->resizeEvent(dynamic_cast<QResizeEvent*>(event));
			break;
		case QEvent::MouseMove:
			this->mouseMoveEvent(dynamic_cast<QMouseEvent*>(event));
			break;
		case QEvent::MouseButtonPress:
			this->mousePressEvent(dynamic_cast<QMouseEvent*>(event));
			break;
		case QEvent::MouseButtonRelease:
			this->mouseReleaseEvent(dynamic_cast<QMouseEvent*>(event));
			break;
		case QEvent::Paint:
			this->paintEvent(dynamic_cast<QPaintEvent*>(event));
			break;
		default:
			break;
	}

	return QObject::eventFilter(obj, event);
}

void QCustomStyleWidget::resizeEvent(QResizeEvent * event)
{
	Q_UNUSED(event);

	QWidget *parent = qobject_cast<QWidget*>(this->parent());
	if (!parent)
		return;

	this->rearangeSystemPushButtons();

	// create bitmap mask
	QPixmap tPixmap(parent->width(), parent->height());
	tPixmap.fill();

	// draw mask
	QPainter tPainter(&tPixmap);
	tPainter.setBrush( QBrush( QColor(0, 0, 0) ));

	if (this->pMaximized)
		tPainter.drawRect(0, 0, parent->width()-1, parent->height()-1);
	else
		tPainter.drawRoundedRect(QRect(0, 0, parent->width()-1, parent->height()-1 ), this->pBorderRadius, this->pBorderRadius);

	// apply mask
	parent->setMask(tPixmap);
}

void QCustomStyleWidget::mousePressEvent(QMouseEvent *event)
{
	QWidget *parent = qobject_cast<QWidget*>(this->parent());
	if (!parent)
		return;

	if (event->button() == Qt::LeftButton)
	{
		this->pMoving = this->checkMouseDragFlag(event->globalPos());

		if (this->pMoving & WDRAG_CENTER)
			this->pOffset = event->globalPos() - parent->geometry().topLeft();
		else
		{
			this->pRect = parent->geometry();

			if (this->pMoving & WDRAG_RIGHT)
				this->pOffset.setX( event->globalPos().x() - this->pRect.right());
			else
			if (this->pMoving & WDRAG_LEFT)
				this->pOffset.setX( event->globalPos().x() - this->pRect.left());

			if (this->pMoving & WDRAG_BOTTOM)
				this->pOffset.setY( event->globalPos().y() - this->pRect.bottom());
			else
			if (this->pMoving & WDRAG_TOP)
				this->pOffset.setY( event->globalPos().y() - this->pRect.top());
		}
	}
}

void QCustomStyleWidget::mouseMoveEvent(QMouseEvent *event)
{
	if (this->pMaximized)
		return;

	QWidget *parent = qobject_cast<QWidget*>(this->parent());
	if (!parent)
		return;

	// handle resize and move events
	if (this->pMoving & WDRAG_CENTER)
		parent->move(event->globalPos() - this->pOffset);
	else
	if (this->pMoving)
	{
		QRect tRect = this->pRect;
		QSize tSize = parent->minimumSize();
		QPoint tPoint = event->globalPos() - this->pOffset;

		if (this->pMoving & WDRAG_RIGHT)
			tRect.setRight(tPoint.x());
		else
		if (this->pMoving & WDRAG_LEFT)
		{
			tRect.setLeft(tPoint.x());
			if (tRect.width() < tSize.width())
				tRect.setLeft(tRect.right() - tSize.width());
		}

		if (this->pMoving & WDRAG_BOTTOM)
			tRect.setBottom(tPoint.y());
		else
		if (this->pMoving & WDRAG_TOP)
		{
			tRect.setTop(tPoint.y());
			if (tRect.height() < tSize.height())
				tRect.setTop(tRect.bottom() - tSize.height());
		}

		parent->setGeometry(tRect);
	}

	// change mouse cursor on the edges of the window
	if (this->pMoving == WDRAG_NONE)
	{
		Qt::CursorShape cursor = Qt::ArrowCursor;
		int mouseFlag = this->checkMouseDragFlag(event->globalPos());

		if (mouseFlag & WDRAG_TOP)
		{
			if (mouseFlag & WDRAG_LEFT)
				cursor = Qt::SizeFDiagCursor;
			else
			if (mouseFlag & WDRAG_RIGHT)
				cursor = Qt::SizeBDiagCursor;
			else
				cursor = Qt::SizeVerCursor;
		}
		else if (mouseFlag & WDRAG_BOTTOM)
		{
			if (mouseFlag & WDRAG_LEFT)
				cursor = Qt::SizeBDiagCursor;
			else
			if (mouseFlag & WDRAG_RIGHT)
				cursor = Qt::SizeFDiagCursor;
			else
				cursor = Qt::SizeVerCursor;
		}
		else
		if ((mouseFlag & WDRAG_LEFT) || (mouseFlag & WDRAG_RIGHT))
			cursor = Qt::SizeHorCursor;

		QApplication::restoreOverrideCursor();	
		if (cursor != Qt::ArrowCursor)
		{
			QCursor newCursor(cursor);
			QApplication::setOverrideCursor(newCursor);
			QApplication::changeOverrideCursor(newCursor);
		}
	}

#ifdef Q_WS_WIN
    MSG msg;
    while(PeekMessage(&msg, parent->winId(), WM_MOUSEMOVE, WM_MOUSEMOVE, PM_REMOVE));
#endif
}

void QCustomStyleWidget::mouseReleaseEvent(QMouseEvent *event)
{
	if (event->button() == Qt::LeftButton)
		this->pMoving = WDRAG_NONE;
}

void QCustomStyleWidget::paintEvent(QPaintEvent *event)
{
	QWidget *parent = qobject_cast<QWidget*>(this->parent());
	if (!parent)
		return;

	event->accept();
	QPainter tPainter(parent);
	const QRect tRect(0, 0, parent->width()-1, parent->height()-1);

	QImage *tCaptionImage = parent->isActiveWindow() ? this->pCaptionImage.first : this->pCaptionImage.second;
	if (tCaptionImage)
		tPainter.drawImage(QRect(1,0, parent->width()-2, tCaptionImage->height()), *tCaptionImage, tCaptionImage->rect());
}

void QCustomStyleWidget::on_systemPushButtonExit_clicked(void)
{	
	QWidget *parent = qobject_cast<QWidget*>(this->parent());
	if (!parent)
		return;

	parent->close();
}

void QCustomStyleWidget::on_systemPushButtonMaximize_clicked(void)
{
	QWidget *parent = qobject_cast<QWidget*>(this->parent());
	if (!parent)
		return;

	if (!this->pMaximized)
	{
		this->pMaximized = true;
		this->pRestoreGeometry = parent->saveGeometry();
		parent->setGeometry(QApplication::desktop()->availableGeometry(parent));
	}
	else
	{
		this->pMaximized = false;
		parent->restoreGeometry(this->pRestoreGeometry);
	}
		
	this->pSystemPushButtons[1]->setProperty(DYNAMIC_PROPERTY_WINDOW_STATE, this->pMaximized);	
	parent->setProperty(DYNAMIC_PROPERTY_WINDOW_STATE, this->pMaximized);
	parent->setStyleSheet(parent->styleSheet());
}

void QCustomStyleWidget::on_systemPushButtonMinimize_clicked(void)
{
	QWidget *parent = qobject_cast<QWidget*>(this->parent());
	if (!parent)
		return;

	parent->setWindowState(parent->windowState() | Qt::WindowMinimized);
}

void QCustomStyleWidget::setLayoutDirection(Qt::LayoutDirection layoutDirection)
{
	this->pLayoutDirection = layoutDirection;
	this->rearangeSystemPushButtons();
}

Qt::LayoutDirection QCustomStyleWidget::layoutDirection(void)
{
	return this->pLayoutDirection;
}