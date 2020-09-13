#include <QtGui>
#include <QX11EmbedContainer>

class EmbedWidgetContainer : public QX11EmbedContainer
{
    Q_OBJECT

public:
    EmbedWidgetContainer(QWidget *parent = 0);
    QSize sizeHint() const;

protected:
    void paintEvent(QPaintEvent *event);

private:
    QRadialGradient gradient;
};


class Embedder : public QWidget
{
    Q_OBJECT
public:
    Embedder();
    void setButtonsEnabled();

private slots:
    void embed();
    void discard();
    void clientEmbedded();
    void clientClosed();
    void embedError(QX11EmbedContainer::Error error);

private:
    EmbedWidgetContainer *container;
    QPushButton *embedbutton;
    QPushButton *discardbutton;
    QLineEdit *widedit;
    QLabel *status;
};

/*
#ifndef EMBEDWIDGET_H
#define EMBEDWIDGET_H

#include <QRadialGradient>
#include <QSize>
#include <QX11EmbedWidget>

class QPaintEvent;

class EmbedWidget : public QX11EmbedWidget
{
    Q_OBJECT

public:
    EmbedWidget(QWidget *parent = 0);
    QSize sizeHint() const;

protected:
    void paintEvent(QPaintEvent *event);

private:
    QRadialGradient gradient;
};

#endif

*/
