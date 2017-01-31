/******************************************************************************
 * Copyright (C) 2007  Tom Francart <tom.francart@med.kuleuven.be>            *
 *                                                                            *
 * This program is free software; you can redistribute it and/or modify       *
 * it under the terms of the GNU General Public License as published by       *
 * the Free Software Foundation; either version 2 of the License, or          *
 * (at your option) any later version.                                        *
 *                                                                            *
 * This program is distributed in the hope that it will be useful,            *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of             *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the              *
 * GNU General Public License for more details.                               *
 *                                                                            *
 * You should have received a copy of the GNU General Public License along    *
 * with this program; if not, write to the Free Software Foundation, Inc.,    *
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.                *
 ******************************************************************************/

#include "plugincontrollerinterface.h"
#include <QStringList>


class DemoControllerCreator :
    public QObject,
    public PluginControllerCreator
{
    Q_OBJECT
    Q_INTERFACES (PluginControllerCreator)
public:
    virtual QStringList availablePlugins() const;

    virtual PluginControllerInterface *createController
            (const QString &name ) const;
};

Q_EXPORT_PLUGIN2 (democontroller, DemoControllerCreator)

class DemoController:
    public QObject,
    public PluginControllerInterface
{
    Q_OBJECT
public:
    DemoController ();
    ~DemoController();

    virtual void resetParameters();

    virtual bool isValidParameter (const QString &type, int channel) const;
    virtual bool setParameter (const QString &type, int channel,
            const QString &value);

    virtual bool prepare ();


private:
    int myParam;
};


// DemoController ==============================================================

DemoController::DemoController ():
        myParam(-1)
{
    qDebug("Initializing democontroller");
}


DemoController::~DemoController()
{
   qDebug("Deleting democontroller");
}

void DemoController::resetParameters()
{
   myParam=-1;
}

bool DemoController::isValidParameter (const QString &type, int channel) const
{
    if (type == "demoparameter" && channel == -1)
        return true;
    
    return false;
}

bool DemoController::setParameter (const QString &type, int channel,
        const QString &value)
{
    if (type == "demoparameter" && channel == -1) {
        myParam=value.toInt();
        qDebug("DemoController: setting parameter to value %i", myParam);
        return true;
    }

    return false;
}

bool DemoController::prepare ()
{
    qDebug("DemoController: the fancy stuf happens here...");
    return true;
}


// DemoControllerCreator =======================================================

QStringList DemoControllerCreator::availablePlugins() const
{
    return QStringList() << "democontroller";
}

PluginControllerInterface *DemoControllerCreator::createController
       (const QString &name) const
{
    try {
        if (name == "democontroller")
            return new DemoController ();
    } catch (...) {
        // Exceptions silently ignored
    }

    return NULL;
}

#include "democontroller.moc"
