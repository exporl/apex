#include "webpageprocessor.h"
#include "services/errorhandler.h"

#include <QDebug>

static const char* ERROR_SOURCE = "WebPageProcessor";

WebPageProcessor::WebPageProcessor(QObject *parent) :
    QWebPage(parent)
{
}

void WebPageProcessor::javaScriptConsoleMessage(const QString &message, int lineNumber, const QString &sourceID)
{
    //Add the message to the debug log:
    qCDebug(APEX_RS) << ERROR_SOURCE << message << lineNumber << sourceID;
    //Add a warning to the message window:
    apex::ErrorHandler::Get().addWarning(ERROR_SOURCE, lineNumber + message);
}

WebPageProcessor::~WebPageProcessor()
{
    //Should there be something in the destructor?
}
