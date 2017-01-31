#include "feedback.h"
#include "pluginfeedbackinterface.h"
#include "services/pluginloader.h"
#include "screen/screensdata.h"

#include <QDebug>

namespace apex {

class FeedbackPrivate
{
    public:

        QList<PluginFeedbackInterface*> plugins;

};


Feedback::Feedback(const apex::data::ScreensData* data):
    d(new FeedbackPrivate)
{
    const QList<data::FeedbackPluginPair>& l( data->feedbackPlugins());

    Q_FOREACH(data::FeedbackPluginPair p, l) {
        qDebug()<< "Loading feedback plugin " << p.first;
        loadPlugin(p.first, p.second);
    }
};

Feedback::~Feedback()
{
    Q_FOREACH( PluginFeedbackInterface* i, d->plugins) {
        delete i;
    }
    delete d;
}

void Feedback::loadPlugin(const QString& name,
                          const data::FeedbackPluginParameters& params)
{
    PluginFeedbackCreator* creator = PluginLoader::Get().
    createPluginCreator<PluginFeedbackCreator>(name);
    d->plugins.push_back (creator->createPluginFeedback (name, params));
    QString err( d->plugins.last()->errorString() );
    if (! err.isEmpty()) {
        throw ApexStringException( err );
    }
    return;
}

//! initialize all plugins
void Feedback::initialize()
{
    clear();
}

void Feedback::highLight(const QString& element)
{
    Q_FOREACH( PluginFeedbackInterface* i, d->plugins) {
        i->highLight(element);
    }
}

void Feedback::showPositive()
{
    Q_FOREACH( PluginFeedbackInterface* i, d->plugins) {
        i->showPositive();
    }
}

void Feedback::showNegative()
{
    Q_FOREACH( PluginFeedbackInterface* i, d->plugins) {
        i->showNegative();
    }
}

void Feedback::clear()
{
    Q_FOREACH( PluginFeedbackInterface* i, d->plugins) {
        i->clear();
    }
}

    

};              // ns apex
