#ifndef _WIDGETWINDOW_H_
#define _WIDGETWINDOW_H_

#include <QWidget>
#include <QPushButton>
#include <QHBoxLayout>

#define BUTTON_WIDTH    24
#define BUTTON_HEIGHT   24

class ScreenShot;
class WidgetWindow: public QWidget
{
    Q_OBJECT
    
public:
    WidgetWindow(QWidget *parent = NULL);
    ~WidgetWindow();

protected:
    virtual void keyPressEvent(QKeyEvent *e);

private:
    void initWindow();

public slots:
    void onWidgetSelected();
    void onWidgetPressed();

private:
    QPoint start;
    QPushButton *m_pen;
    QPushButton *m_rect;
    QPushButton *m_revoke;
    QPushButton *m_save;
    QPushButton *m_copy;
    QHBoxLayout *m_layout;
    QList<QPushButton*> widget_list;
};

#endif // _WIDGETWINDOW_H_
