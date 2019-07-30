#ifndef _SCREENSHOT_H_
#define _SCREENSHOT_H_

#include <QWidget>
#include <QPixmap>
#include <QMainWindow>
#include <QPen>

#include "widgetwindow.h"

#define MAX_HISTORY         5

#define NO_WIDGET_SELECTED  0
#define PEN_SELECTED        1
#define RECT_SELECTED       2
#define REVOKE_SELECTED     3

class ScreenShot: public QMainWindow
{
    Q_OBJECT

public:
    ScreenShot(QWidget *parent = NULL);
    ~ScreenShot();

private:
    void initWindow();
    void printScreen();
    void resetCapture();
    bool inCaptureArea(QPoint p);
    void addHistory(QPixmap p);
    void revokeHistory();
    

protected:
    virtual void paintEvent(QPaintEvent *e);
    virtual void keyPressEvent(QKeyEvent *e);
    virtual void mousePressEvent(QMouseEvent *e);
    virtual void mouseDoubleClickEvent(QMouseEvent *e);
    virtual void mouseMoveEvent(QMouseEvent* e);
    virtual void mouseReleaseEvent(QMouseEvent *e);

public slots:
    void copyToClipboard();
    void saveCapture();
    void onPenSelected();
    void onRectSelected();
    void onRevokeSelected();

private:
    QPixmap m_screen, m_shadow, m_capture, m_draw;
    QList<QPixmap> m_history;
    QPoint m_start, m_end;
    int screen_w, screen_h;
    int start_x, start_y;
    int end_x, end_y;
    int m_action;
    QPoint action_start, action_end;
    bool isclicked, isdragged, have_capture;
    WidgetWindow *m_widget;
    QPen m_pen;
};

#endif // _SCREENSHOT_H_