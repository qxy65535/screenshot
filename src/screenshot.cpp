#include <QApplication>
#include <QDesktopWidget>
#include <QPainter>
#include <QKeyEvent>
#include <QClipboard>
#include <QScreen>
#include <QGuiApplication>
#include <QWindow>
#include <QFileDialog>
#include <QDateTime>
#include <QDebug>

#include "screenshot.h"

ScreenShot::ScreenShot(QWidget *parent): QMainWindow(parent)
{
    isclicked = false;
    screen_w = screen_h = 0;

    initWindow();
    printScreen();
    resetCapture();
}

ScreenShot::~ScreenShot()
{

}
void ScreenShot::initWindow()
{
    m_widget = new WidgetWindow(this);

    // 监听非点击时的鼠标移动事件
    this->setMouseTracking(true);
    // 去掉窗口的边框
    this->setWindowFlags(Qt::FramelessWindowHint);
    // 获取键盘焦点，全屏
    this->setWindowState(Qt::WindowActive | Qt::WindowFullScreen);
    this->grabKeyboard();

    this->setFixedSize(QApplication::desktop()->size());
}

void ScreenShot::printScreen()
{
    m_screen = QGuiApplication::primaryScreen()->grabWindow(QApplication::desktop()->winId());
    screen_w = m_screen.width();
    screen_h = m_screen.height();
}

void ScreenShot::resetCapture()
{
    m_start = QPoint(0, 0);
    m_end = QPoint(0, 0);
    have_capture = false;
    isclicked = false;
    isdragged = false;
    m_action = NO_WIDGET_SELECTED;
    start_x = start_y = 0;
    end_x = end_y = 0;
    
    m_widget->onWidgetSelected();

    m_pen.setStyle(Qt::SolidLine);
    m_pen.setWidth(2);
    m_pen.setBrush(Qt::red);
    m_pen.setCapStyle(Qt::RoundCap);
    m_pen.setJoinStyle(Qt::RoundJoin);
}

bool ScreenShot::inCaptureArea(QPoint p)
{
    if (!have_capture) return false;
    return p.x() >= start_x &&
           p.y() >= start_y &&
           p.x() <= end_x &&
           p.y() <= end_y;
}

void ScreenShot::addHistory(QPixmap p)
{
    if (m_history.size() > MAX_HISTORY) {
        m_history.removeFirst();
    }
    m_history.append(p);
}

void ScreenShot::revokeHistory()
{
    if (m_history.size() == 1) {
        m_draw = m_history.first();
    } else {
        m_history.removeLast();
        m_draw = m_history.last();
    }
}

void ScreenShot::copyToClipboard()
{
    if (have_capture) {
        QPainter painter(&m_capture);
        painter.drawPixmap(m_draw.rect(), m_draw);
        QApplication::clipboard()->setImage(m_capture.toImage());
        m_widget->close();
        this->close();
    }
}

void ScreenShot::saveCapture()
{
    if (have_capture) {
        QString save_path = QDateTime::currentDateTime().toString("yyyyMMddHHmmss") + ".png";
        QFileDialog file_dialog(NULL, "save", save_path, tr("Images (*.png *.bmp *.jpg)"));
        file_dialog.setAcceptMode(QFileDialog::AcceptSave);

        if (file_dialog.exec() == QDialog::Accepted) {
            save_path = file_dialog.selectedFiles()[0];
            QPainter painter(&m_capture);
            painter.drawPixmap(m_draw.rect(), m_draw);
            m_capture.save(save_path, "png");
            m_widget->close();
            this->close();
        }
    }
}

void ScreenShot::onPenSelected()
{
    m_action = PEN_SELECTED;
}

void ScreenShot::onRectSelected()
{
    m_action = RECT_SELECTED;
}

void ScreenShot::onRevokeSelected()
{
    m_action = REVOKE_SELECTED;
    this->revokeHistory();
    this->update();
}

void ScreenShot::paintEvent(QPaintEvent *e)
{
    QPainter painter(this);
    QColor shadowColor = QColor(0, 0, 0, 130);
    // m_draw = m_history.last().copy();
    // 绘制截屏
    painter.drawPixmap(0, 0, m_screen);
    // 绘制阴影蒙版
    painter.fillRect(m_screen.rect(), shadowColor);
    
    // 校正起点终点
    QRect rect(QPoint(start_x, start_y), QPoint(end_x, end_y));
    
    // 绘制涂鸦到选区
    // m_capture = m_screen.copy(rect);
    if (m_action == NO_WIDGET_SELECTED) {
        m_capture = m_screen.copy(rect);
    }
    painter.drawPixmap(rect, m_capture);
    switch (m_action) {
        // case NO_WIDGET_SELECTED:
        // {
        //     m_capture = m_screen.copy(rect);
        //     break;
        // }
        case PEN_SELECTED:
        {
            QPainter action_painter(&m_draw);
            action_painter.setPen(m_pen);
            action_painter.drawLine(action_start.x()-start_x, action_start.y()-start_y, 
                                    action_end.x()-start_x, action_end.y()-start_y);
            action_start = action_end;
            break;
        }
        case RECT_SELECTED:
            painter.setPen(m_pen);
            painter.drawRect(action_start.x(), action_start.y(), 
                             action_end.x()-action_start.x(), action_end.y()-action_start.y());
            break;
        case REVOKE_SELECTED:
            break;
            
    }
    
    // 绘制截图选区
    // painter.drawPixmap(rect, m_capture);
    painter.drawPixmap(rect, m_draw);

    // 绘制选区边框
    QColor rectColor(255, 0, 0);
    painter.setPen(rectColor);
    painter.drawRect(rect);


}

void ScreenShot::keyPressEvent(QKeyEvent *e)
{
    if (e->key() == Qt::Key_Escape) {
        m_widget->close();
        this->close();
    }
}

void ScreenShot::mousePressEvent(QMouseEvent *e)
{
    isdragged = false;
    if (have_capture) {
        if (e->button() == Qt::RightButton) {
            have_capture = false;
            m_widget->hide();
            resetCapture();
            this->update();
            return;
        }
        isclicked = true;
        switch (m_action)
        {
        case NO_WIDGET_SELECTED:
            m_start = e->pos();
            break;
        case PEN_SELECTED:
            action_start = e->pos();
            break;
        case RECT_SELECTED:
            action_start = e->pos();
            // 校正
            if (action_start.x() < start_x)
                action_start.setX(start_x);
            else if (action_start.x() > end_x)
                action_start.setX(end_x);
            if (action_start.y() < start_y)
                action_start.setY(start_y);
            else if (action_start.y() > end_y)
                action_start.setY(end_y);
            break;
        
        default:
            break;
        }
        return;
    } else {
        if (e->button() == Qt::RightButton) {
            m_widget->close();
            this->close();
            return;
        }
    }
    if (!isclicked) {
        m_start = e->pos();
        isclicked = true;
        have_capture = false;
    }
}

void ScreenShot::mouseDoubleClickEvent(QMouseEvent *e)
{
    if (have_capture) {
        this->copyToClipboard();
    }
}

void ScreenShot::mouseMoveEvent(QMouseEvent* e)
{
    if (inCaptureArea(e->pos()) && m_action != NO_WIDGET_SELECTED) {
        this->setCursor(Qt::CrossCursor);
    } else {
        this->setCursor(Qt::ArrowCursor);
    }
    if (isclicked) {
        isdragged = true;
        switch (m_action)
        {
        case NO_WIDGET_SELECTED:
            m_widget->hide();
            have_capture = true;
            m_end = e->pos();
            start_x = qMin(m_start.x(), m_end.x());
            start_y = qMin(m_start.y(), m_end.y());
            end_x = qMax(m_start.x(), m_end.x());
            end_y = qMax(m_start.y(), m_end.y());
            break;
        case PEN_SELECTED:
            action_end = e->pos();
            break;
        case RECT_SELECTED:
            action_end = e->pos();
            // 校正
            if (action_end.x() < start_x)
                action_end.setX(start_x);
            else if (action_end.x() > end_x)
                action_end.setX(end_x);
            if (action_end.y() < start_y)
                action_end.setY(start_y);
            else if (action_end.y() > end_y)
                action_end.setY(end_y);
            break;
        default:
            break;
        }

        this->update();
    }
}

void ScreenShot::mouseReleaseEvent(QMouseEvent *e)
{
    isclicked = false;
    if (isdragged && have_capture) {
        switch (m_action)
        {
        case NO_WIDGET_SELECTED:
        {
            m_draw = m_capture.copy(m_capture.rect());
            m_draw.fill(QColor(0, 0, 0, 0));
            m_history.clear();
            this->addHistory(m_draw);
            
            m_widget->show();
            int x = end_x - m_widget->width() - 5;
            int y = end_y + 5;
            if (y + m_widget->height() > m_screen.height())
                y = m_screen.height() - m_widget->height() - 5;
            m_widget->move(x, y);
            break;
        }
        case PEN_SELECTED:
            this->addHistory(m_draw);
            break;
        case RECT_SELECTED:
        {
            QPainter action_painter(&m_draw);
            action_painter.setPen(m_pen);
            action_painter.drawRect(action_start.x()-start_x, action_start.y()-start_y, 
                                    action_end.x()-action_start.x(), action_end.y()-action_start.y());
            this->addHistory(m_draw);
            break;
        }

        default:
            break;
        }
    }
    isdragged = false;
}
