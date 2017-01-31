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
 

#include <QObject>
#include <QStringList>
#include <QDebug>

#include "feedback/pluginfeedbackinterface.h"

class dummyfeedbackpluginCreator :
    public QObject,
    public PluginFeedbackCreator
{
    Q_OBJECT
    Q_INTERFACES (PluginFeedbackCreator)
public:
    virtual QStringList availablePlugins() const;

    virtual PluginFeedbackInterface *createPluginFeedback
        (const QString &name,
         const FeedbackPluginParameters& params) const;
};

Q_EXPORT_PLUGIN2 (dummyfeedbackplugin, dummyfeedbackpluginCreator)

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
    qDebug() << "dummyfeedbackplugin: " << message;
}

dummyfeedbackplugin::dummyfeedbackplugin(const FeedbackPluginParameters& params)
{
    showMessage("constructor");
    qDebug("Available parameters: ");
    QPair<QString,QString> p;
    Q_FOREACH( p, params) {
        qDebug() << "(" << p.first << "," << p.second << ")";
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

