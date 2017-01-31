#ifndef _EXPORL_SRC_LIB_APEXMAIN_RESULT_WEBPAGEPROCESSOR_H_
#define _EXPORL_SRC_LIB_APEXMAIN_RESULT_WEBPAGEPROCESSOR_H_

#include <QObject>
#include <QWebPage>

class WebPageProcessor : public QWebPage
{
    Q_OBJECT
public:
    WebPageProcessor(QObject *parent =0);

protected:

    /**
     * Retrieves the console messages from the javascript scriptEngine and adds these messages
     * to the debug log and the message window.
     */
    void javaScriptConsoleMessage(const QString &message, int lineNumber, const QString &sourceID);

    ~WebPageProcessor();
};

#endif // _EXPORL_SRC_LIB_APEXMAIN_RESULT_WEBPAGEPROCESSOR_H_
