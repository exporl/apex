#include "htmlapi.h"

namespace apex
{

HtmlAPI::HtmlAPI()
{
}

HtmlAPI::~HtmlAPI()
{
}

void HtmlAPI::startExperiment()
{
    emit startExperimentRequested();
}

void HtmlAPI::newAnswer(const QString &text)
{
    emit answered(text);
}
}
