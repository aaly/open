
#include "embedder.h"


EmbedWidgetContainer::EmbedWidgetContainer(QWidget *parent)
    : QX11EmbedContainer(parent)
{
    gradient = QRadialGradient(100, 100, 90, 60, 60);
    gradient.setColorAt(0.0, Qt::white);
    gradient.setColorAt(0.9, QColor(192, 192, 255));
    gradient.setColorAt(1.0, QColor(0, 32, 64));
}

QSize EmbedWidgetContainer::sizeHint() const
{
    return QSize(200, 200);
}

void EmbedWidgetContainer::paintEvent(QPaintEvent *event)
{
    QPainter painter;
    painter.begin(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.fillRect(event->rect(), QBrush(gradient));
    painter.end();
}


Embedder::Embedder()
{
    // create widgets
    QLabel *widlabel = new QLabel("&Window ID:");
    widedit = new QLineEdit();
    widlabel->setBuddy(widedit);

    embedbutton = new QPushButton("Embed");
    embedbutton->setDefault(true);
    discardbutton = new QPushButton("Discard");

    status = new QLabel();

    container = new EmbedWidgetContainer();
    container->setPalette(QPalette(Qt::darkGray));
    container->setAutoFillBackground(true);

    // layout widgets
    QHBoxLayout *widlayout = new QHBoxLayout();
    widlayout->addWidget(widlabel);
    widlayout->addWidget(widedit);

    QVBoxLayout *leftlayout = new QVBoxLayout();
    leftlayout->addLayout(widlayout);
    leftlayout->addWidget(embedbutton);
    leftlayout->addWidget(discardbutton);
    leftlayout->addWidget(status);
    leftlayout->addStretch();

    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->addLayout(leftlayout, 0);
    layout->addWidget(container, 1);

    connect(embedbutton, SIGNAL(clicked()),
            this, SLOT(embed()));
    connect(discardbutton, SIGNAL(clicked()),
            this, SLOT(discard()));
    connect(container, SIGNAL(clientIsEmbedded()),
            this, SLOT(clientEmbedded()));
    connect(container, SIGNAL(clientClosed()),
            this, SLOT(clientClosed()));
    connect(container, SIGNAL(error(QX11EmbedContainer::Error)),
            this, SLOT(embedError(QX11EmbedContainer::Error)));

    setWindowTitle("XEmbed Embedder");
    setButtonsEnabled();
}

void Embedder::setButtonsEnabled()
{
    embedbutton->setEnabled(container->clientWinId() == 0);
    discardbutton->setEnabled(container->clientWinId() != 0);
}

void Embedder::embed()
{
    QApplication::setOverrideCursor(Qt::WaitCursor);

    bool ok;
    container->embedClient(widedit->text().toInt(&ok, 16));

    setButtonsEnabled();
}

void Embedder::discard()
{
    container->discardClient();
    setButtonsEnabled();
}

void Embedder::clientEmbedded()
{
    status->setText(QString("Window ID: %1").arg(container->clientWinId(), 16));
    QApplication::restoreOverrideCursor();
    setButtonsEnabled();
}

void Embedder::clientClosed()
{
    status->setText(QString());
    QApplication::restoreOverrideCursor();
    setButtonsEnabled();
}

void Embedder::embedError(QX11EmbedContainer::Error error)
{
    switch (error) {
      case QX11EmbedContainer::InvalidWindowID:
          status->setText("Invalid window ID");
          break;
      default:
          status->setText("Unknown error");
          break;
    }
    QApplication::restoreOverrideCursor();
    setButtonsEnabled();
}

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    Embedder embedder;
    embedder.resize(400, 200);
    embedder.show();

    return app.exec();
}


/*

#include <QtGui>

#include "embedder.h"
#include <QApplication>


EmbedWidget::EmbedWidget(QWidget *parent)
    : QX11EmbedWidget(parent)
{
    gradient = QRadialGradient(100, 100, 90, 60, 60);
    gradient.setColorAt(0.0, Qt::white);
    gradient.setColorAt(0.9, QColor(192, 192, 255));
    gradient.setColorAt(1.0, QColor(0, 32, 64));
}

QSize EmbedWidget::sizeHint() const
{
    return QSize(200, 200);
}

void EmbedWidget::paintEvent(QPaintEvent *event)
{
    QPainter painter;
    painter.begin(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.fillRect(event->rect(), QBrush(gradient));
    painter.end();
}


int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    if (app.arguments().count() != 2) {
        qFatal("Error - expected window id as argument");
        return 1;
    }

    QString windowId(app.arguments()[1]);
    EmbedWidget window;
    window.embedInto(windowId.toULong());
    window.show();

    return app.exec();
}
*/
