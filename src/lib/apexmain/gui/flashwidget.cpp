#include "apextools/global.h"

#include "flashwidget.h"

#include <QAxWidget>
#include <QDebug>
#include <QFileInfo>
#include <QGridLayout>
#include <QMouseEvent>
#include <QShortcut>
#include <QTimer>

/**
  * Inherits from QAxWidget and emits a signal when the widget is clicked
  * Its only purpose is to implement mouseReleaseEvent
  */
class AxMouseFlash:
        public QAxWidget
{
public:
    AxMouseFlash(QWidget* parent, QObject* o, const char* name);
    void mouseReleaseEvent ( QMouseEvent * event ) ;

private:
    QObject* eventObject;
    QByteArray slotName;
};

AxMouseFlash::AxMouseFlash( QWidget* parent, QObject* o, const char* name):
    QAxWidget("ShockwaveFlash.ShockwaveFlash", parent),
    eventObject(o),
    slotName(name)
{
}

void AxMouseFlash::mouseReleaseEvent(QMouseEvent * event)
{
    Q_UNUSED(event);
    if (eventObject)
        QMetaObject::invokeMethod(eventObject, slotName);
}

class FlashWidgetPrivate:
        public QObject
{
    Q_OBJECT
public:
    FlashWidgetPrivate(FlashWidget* p);
    QTimer timer;
    AxMouseFlash *axwidget;
    FlashWidget *p;
    QShortcut *shortcut;
    bool enabled;

public Q_SLOTS:
    void timerTriggered();
    void clicked();
};

FlashWidgetPrivate::FlashWidgetPrivate(FlashWidget* p) :
        p(p),
        shortcut(NULL),
        enabled(false)
{
}

void FlashWidgetPrivate::timerTriggered()
{
    if (p->isFinished()) {
        timer.stop();
        p->finished();
    }
}

void FlashWidgetPrivate::clicked()
{
    if (p->isEnabled())
        Q_EMIT p->clicked();
}

FlashWidget::FlashWidget(QWidget* parent):
        QWidget(parent),
        d(new FlashWidgetPrivate(this))
{
    d->timer.setInterval(50);
    d->axwidget = new AxMouseFlash(this, d, "clicked");

    QGridLayout* l = new QGridLayout(this);
    setLayout(l);
    l->setContentsMargins(0,0,0,0);
    l->addWidget(d->axwidget,0,0);

    // does not have any effect :(
    d->axwidget->setProperty("WMode", "Transparent");
//    setPalette(palette());

    connect(&d->timer, SIGNAL(timeout()), d, SLOT(timerTriggered()));
}

FlashWidget::~FlashWidget()
{
    delete d;
}

void FlashWidget::setShortcut(const QKeySequence &key)
{
    if (d->shortcut)
        delete d->shortcut;
    d->shortcut = NULL;
    if (!key.isEmpty()) {
        d->shortcut = new QShortcut(key, this);
        connect(d->shortcut, SIGNAL(activated()), d, SLOT(clicked()));
    }
}

bool FlashWidget::loadMovie(const QString &filename)
{
    // The ActiveX control does not like relative paths
    QString absPath( QFileInfo(filename).absoluteFilePath() );
    //qCDebug(APEX_RS) << "Loading " << absPath;
    d->axwidget->dynamicCall ("LoadMovie(int,const QString&)",0, absPath);
    d->axwidget->dynamicCall ("StopPlay()");
    d->axwidget->dynamicCall ("ReWind()");
    return readyState()==4;
}

int FlashWidget::readyState() const
{
    return d->axwidget->dynamicCall( "ReadyState()").toInt();
}

bool FlashWidget::play()
{
    d->axwidget->dynamicCall ("Play()");
    d->timer.start();
        return readyState()==4;
}

void FlashWidget::stop()
{
    d->axwidget->dynamicCall ("Stop()");
}

void FlashWidget::nextFrame()
{
    d->axwidget->dynamicCall ("Foward()");
}

void FlashWidget::gotoFrame(int frame)
{
    d->axwidget->dynamicCall ("GoToFrame(int)", frame);
}

bool FlashWidget::isPlaying() const
{
    return d->axwidget->dynamicCall("IsPlaying()").toBool();
}

bool FlashWidget::isFinished() const
{
    int totalFrames = d->axwidget->property("TotalFrames").toInt();
    /*qCDebug(APEX_RS) << "Framenum=" << d->axwidget->property("FrameNum").toInt()
            << ", TotalFrames=" << totalFrames
            << ", ReadyState=" << readyState()
            << ", IsPlaying=" << isPlaying();*/
    return totalFrames > 0 && !isPlaying();
}

void FlashWidget::setEnabled(bool e)
{
    d->enabled=e;
    QWidget::setEnabled(e);
}

bool FlashWidget::initialized() const
{
    return ! d->axwidget->isNull();
}

void FlashWidget::setAutoFillBackground(bool s)
{
    QWidget::setAutoFillBackground(s);
    d->axwidget->setAutoFillBackground(s);
}

void FlashWidget::setPalette(const QPalette& p)
{
    QWidget::setPalette(p);
    d->axwidget->setPalette(p);

    qCDebug(APEX_RS) << p.color(backgroundRole()).name();

    d->axwidget->setProperty(
            "BGColor",
            p.color(QPalette::Active, QPalette::Window).name()
            );

}

#include "flashwidget.moc"
