#include <QString>
#include <QWidget>
#include <QPushButton>

class FlashWidgetPrivate;

/**
 * FlashWidget wraps a Flash ActiveX object in a QWidget using QAxWidget
 */
class FlashWidget: public QWidget
{
    Q_OBJECT
    friend class FlashWidgetPrivate;
public:
    FlashWidget(QWidget* parent=0);
    ~FlashWidget();

    /**
     * Returns true if flash is working OK
     */
    bool initialized() const;

    bool loadMovie(const QString &filename);
    bool play();
    void stop();
    void nextFrame();
    void gotoFrame(int frame);
    bool isPlaying() const;
    bool isFinished() const;

    /**
     * If enabled the widget will emit mouseRelease signals
     */
    void setEnabled(bool e);

    /**
     * Ready states are defined in
     * http://www.adobe.com/support/flash/publishexport/scriptingwithflash/scriptingwithflash_03.html
     *
     * 0=Loading, 1=Uninitialized, 2=Loaded, 3=Interactive, 4=Complete.
     */
    int readyState() const;

    void setAutoFillBackground(bool s);
    void setPalette(const QPalette& p);
    void setShortcut(const QKeySequence &key);

Q_SIGNALS:
    void clicked();

    /**
     * Emitted when the movie has finished (after play)
     */
    void finished();

private:
    FlashWidgetPrivate* d;
};


