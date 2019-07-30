#include <QDebug>
#include <QKeyEvent>

#include "widgetwindow.h"

WidgetWindow::WidgetWindow(QWidget *parent): QWidget(parent)
{
    initWindow();
}

WidgetWindow::~WidgetWindow()
{
    delete m_layout;
    delete m_pen;
    delete m_rect;
    delete m_revoke;
    delete m_save;
    delete m_copy;
}

void WidgetWindow::initWindow()
{
    this->setWindowFlags(Qt::FramelessWindowHint | Qt::Tool);

    m_layout = new QHBoxLayout;

    m_pen = new QPushButton;
    m_pen->setFixedSize(BUTTON_WIDTH, BUTTON_HEIGHT);
    m_pen->setIcon(QIcon(":icon/pen"));
    m_pen->setFlat(true);
    m_layout->addWidget(m_pen);
    widget_list.append(m_pen);
    connect(m_pen, SIGNAL(clicked()), this->parent(), SLOT(onPenSelected()));
    connect(m_pen, SIGNAL(pressed()), this, SLOT(onWidgetPressed()));
    connect(m_pen, SIGNAL(released()), this, SLOT(onWidgetSelected()));

    m_rect = new QPushButton;
    m_rect->setFixedSize(BUTTON_WIDTH, BUTTON_HEIGHT);
    m_rect->setIcon(QIcon(":icon/rect"));
    m_rect->setFlat(true);
    m_layout->addWidget(m_rect);
    widget_list.append(m_rect);
    connect(m_rect, SIGNAL(clicked()), this->parent(), SLOT(onRectSelected()));
    connect(m_rect, SIGNAL(pressed()), this, SLOT(onWidgetPressed()));
    connect(m_rect, SIGNAL(released()), this, SLOT(onWidgetSelected()));

    m_revoke = new QPushButton;
    m_revoke->setFixedSize(BUTTON_WIDTH, BUTTON_HEIGHT);
    m_revoke->setIcon(QIcon(":icon/revoke"));
    m_revoke->setFlat(true);
    m_layout->addWidget(m_revoke);
    widget_list.append(m_revoke);
    connect(m_revoke, SIGNAL(clicked()), this->parent(), SLOT(onRevokeSelected()));
    connect(m_revoke, SIGNAL(pressed()), this, SLOT(onWidgetPressed()));
    connect(m_revoke, SIGNAL(released()), this, SLOT(onWidgetSelected()));

    m_save = new QPushButton;
    m_save->setFixedSize(BUTTON_WIDTH, BUTTON_HEIGHT);
    m_save->setIcon(QIcon(":icon/save"));
    m_save->setFlat(true);
    m_layout->addWidget(m_save);
    widget_list.append(m_save);
    connect(m_save, SIGNAL(clicked()), this->parent(), SLOT(saveCapture()));
    connect(m_save, SIGNAL(pressed()), this, SLOT(onWidgetPressed()));
    connect(m_save, SIGNAL(released()), this, SLOT(onWidgetSelected()));

    m_copy = new QPushButton;
    m_copy->setFixedSize(BUTTON_WIDTH, BUTTON_HEIGHT);
    m_copy->setIcon(QIcon(":icon/complete"));
    m_copy->setFlat(true);
    m_layout->addWidget(m_copy);
    widget_list.append(m_copy);
    connect(m_copy, SIGNAL(clicked()), this->parent(), SLOT(copyToClipboard()));
    connect(m_copy, SIGNAL(pressed()), this, SLOT(onWidgetPressed()));
    connect(m_copy, SIGNAL(released()), this, SLOT(onWidgetSelected()));

    m_layout->setSpacing(5);
    m_layout->setContentsMargins(3, 2, 3, 2);
    this->setLayout(m_layout);
}

void WidgetWindow::keyPressEvent(QKeyEvent *e)
{
    if (e->key() == Qt::Key_Escape) {
        this->close();
        QWidget *parent =  qobject_cast<QWidget*>(this->parent());
        parent->close();
    }
}

void WidgetWindow::onWidgetSelected()
{
    for (auto w: widget_list) {
        if (w == this->sender() && w != m_save && w != m_copy && w != m_revoke) {
            w->setDown(true);
        } else {
            w->setDown(false);
        }
    }
}

void WidgetWindow::onWidgetPressed()
{
    for (auto w: widget_list) {
        if (w == this->sender()) {
            w->setDown(true);
        } else {
            w->setDown(false);
        }
    }
}