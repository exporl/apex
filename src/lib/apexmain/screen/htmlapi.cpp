#include "htmlapi.h"

namespace apex
{

HtmlAPI::HtmlAPI()
{
}

HtmlAPI::~HtmlAPI()
{
}

void HtmlAPI::newAnswer(const QString &text)
{
    emit answered(text);
}
}
