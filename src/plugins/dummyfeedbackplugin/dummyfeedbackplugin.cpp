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

#include <QDebug>
#include <QLoggingCategory>
#include <QObject>
#include <QStringList>

Q_DECLARE_LOGGING_CATEGORY(APEX_DUMMYFEEDBACKPLUGIN)
Q_LOGGING_CATEGORY(APEX_DUMMYFEEDBACKPLUGIN, "apex.dummyfeedbackplugin")

class dummyfeedbackpluginCreator :
    public QObject,
    public PluginFeedbackCreator
{
    Q_OBJECT
    Q_INTERFACES (PluginFeedbackCreator)
#if QT_VERSION >= 0x050000
    Q_PLUGIN_METADATA (IID "apex.dummyfeedbackplugin")
#endif
public:
    virtual QStringList availablePlugins() const;

    virtual PluginFeedbackInterface *createPluginFeedback
        (const QString &name,
         const FeedbackPluginParameters& params) const;
};

#if QT_VERSION < 0x050000
Q_EXPORT_PLUGIN2 (dummyfeedbackplugin, dummyfeedbackpluginCreator)
#endif


class dummyfeedbackplugin : public QObject, public PluginFeedbackInterface
{
        Q_OBJECT
public:
    dummyfeedbackplugin(const FeedbackPluginParameters& params);

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

};

QStringList dummyfeedbackpluginCreator::availablePlugins() const
{
    return QStringList() << "dummyfeedbackplugin";
}

PluginFeedbackInterface * dummyfeedbackpluginCreator::createPluginFeedback
    (const QString &name, const FeedbackPluginParameters& params) const
{
    if (name == "dummyfeedbackplugin")
        return new dummyfeedbackplugin(params);

    return NULL;
}


void dummyfeedbackplugin::showMessage(const QString& message)
{
    qCDebug(APEX_DUMMYFEEDBACKPLUGIN) << "dummyfeedbackplugin: " << message;
}

dummyfeedbackplugin::dummyfeedbackplugin(const FeedbackPluginParameters& params)
{
    showMessage("constructor");
    qCDebug(APEX_DUMMYFEEDBACKPLUGIN, "Available parameters: ");
    QPair<QString,QString> p;
    Q_FOREACH( p, params) {
        qCDebug(APEX_DUMMYFEEDBACKPLUGIN) << "(" << p.first << "," << p.second << ")";
    }
}


void dummyfeedbackplugin::highLight(const QString& element)
{
    showMessage("Highlighting " + element);
}

void dummyfeedbackplugin::showPositive()
{
    showMessage("showPositive");
}

void dummyfeedbackplugin::showNegative()
{
    showMessage("showNegative");
}

void dummyfeedbackplugin::clear()
{
    showMessage("clear");
}

QString dummyfeedbackplugin::errorString()
{
    return QString();
}

#include "dummyfeedbackplugin.moc"

