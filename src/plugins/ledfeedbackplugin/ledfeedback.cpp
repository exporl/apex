/******************************************************************************
 * Copyright (C) 2008  Tom Francart <tom.francart@med.kuleuven.be>            *
 *                                                                            *
 * This file is part of APEX 3.                                               *
 *                                                                            *
 * APEX 3 is free software: you can redistribute it and/or modify             *
 * it under the terms of the GNU General Public License as published by       *
 * the Free Software Foundation, either version 2 of the License, or          *
 * (at your option) any later version.                                        *
 *                                                                            *
 * APEX 3 is distributed in the hope that it will be useful,                  *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of             *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the              *
 * GNU General Public License for more details.                               *
 *                                                                            *
 * You should have received a copy of the GNU General Public License          *
 * along with APEX 3.  If not, see <http://www.gnu.org/licenses/>.            *
 *****************************************************************************/

#include "apexmain/feedback/pluginfeedbackinterface.h"

#include "apextools/global.h"

#include "ledcontroller.h"

#include <QDebug>
#include <QObject>
#include <QStringList>

#include <exception>
#include <memory>

class LedFeedbackCreator :
    public QObject,
    public PluginFeedbackCreator
{
    Q_OBJECT
    Q_INTERFACES (PluginFeedbackCreator)
    Q_PLUGIN_METADATA(IID "apex.ledfeedback")
public:
    virtual QStringList availablePlugins() const;

    virtual PluginFeedbackInterface *createPluginFeedback(const QString &name,
                                const FeedbackPluginParameters& params) const;
};

class LedFeedback : public QObject, public PluginFeedbackInterface
{
        Q_OBJECT
public:
    LedFeedback(const FeedbackPluginParameters& params);

    void highLight(const QString& element);

    //! Indicate that the response was correct (thumbs up)
    void showPositive();

    //! Indicate that the response was false (thumbs down)
    void showNegative();

    //! Remove all feedback (highlight & positive/negative)
    void clear();

    //! Get error string (error condition if non-empty)
    virtual QString errorString();

private:
    void showMessage(const QString& message);

    QScopedPointer<LedController> controller;
    QHash<QString, int> mapping;
    QString eString;

};

QStringList LedFeedbackCreator::availablePlugins() const
{
    return QStringList() << "ledfeedback";
}

PluginFeedbackInterface * LedFeedbackCreator::createPluginFeedback
    (const QString &name, const FeedbackPluginParameters& params) const
{
    if (name == "ledfeedback")
        return new LedFeedback(params);

    return NULL;
}


void LedFeedback::showMessage(const QString& message)
{
    qCDebug(APEX_LEDFEEDBACKPLUGIN) << "LedFeedback: " << message;
}

LedFeedback::LedFeedback(const FeedbackPluginParameters& params)
{
    showMessage("constructor");

    QPair<QString,QString> p;
    Q_FOREACH( p, params) {
        bool success;
        mapping[p.first] = p.second.toInt(&success);
        if (!success)
        {
            eString = "LedFeedback: could not parse parameter " + p.first;
        }
    }

    controller.reset( new LedController );
    QString err( controller->errorMessage() );
    if (! err.isEmpty()) {
        eString = err;
        return;
    }

}


void LedFeedback::highLight(const QString& element)
{
    showMessage("Highlighting " + element);
    if ( mapping.contains(element) ) {
        controller->setLed(mapping[element]);
    } else {
        qCDebug(APEX_LEDFEEDBACKPLUGIN) << "LedFeedback:: error cannot find response " << element;
    }
}

void LedFeedback::showPositive()
{
    showMessage("showPositive");
}

void LedFeedback::showNegative()
{
    showMessage("showNegative");
}

void LedFeedback::clear()
{
    showMessage("clear");
    controller->clear();
}

QString LedFeedback::errorString()
{
    QString result(eString);
    eString = QString();
    return result;
}

#include "ledfeedback.moc"

