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
 
#ifndef PLUGININTERFACEAPI_H
#define PLUGININTERFACEAPI_H

#include <QObject>
#include <QStringList>
#include <QVariant>

namespace apex 
{

class PluginProcedure;


/** API for plugin procedure
 * Is a proxy to PluginProcedure, but only exposes relevant methods
 */
class PluginProcedureAPI: public QObject 
{
    Q_OBJECT

public:
    PluginProcedureAPI(PluginProcedure* p);

public slots:
    // Functions to change something to Apex

    /** Create an entire output list containing stimuli and standards
     * Should be used whenever possible
     * Takes care of setting parameters from screen etc.
     */
    void CreateOutputList( QString p_trial, QString p_stimulus );

    void createTrial(const QString& trial, const QString& stimulus, const QString& screen);

    /** Add a stimulus to the output list, probably not to be used together with CreateOutputList
     * If used, the pluginprocedure should implement the same functionality as
     * ApexProcedure::CreateOutputList
     */
    void AddToOutputList(const QString& stimulus);

    /*void SetStimulusParameter(const QString& stimulus_id, const QString& parameter_id, const QString& value);*/
    void SetParameter(const QString& parameter_id, const QVariant& value);
    QVariant GetParameter (const QString& parameter_id);

    /**
     * Ask parametermanager to reset all parameters (that can be reset)
     * to their default value
     * If this function is not called, parameters will never be reset
     * between trials
     * Most procedures call ResetParameters in the beginning of NextTrial
     */
    void ResetParameters();

    /**
     * Register a new parameter with the manager.
     */
    void registerParameter(const QString& name); //TODO more parameters?

    void SetScreen(const QString screen);

    void SetProgress(const int value);
    void SetNumTrials(const int value);

    QString screenElementText(const QString& screenId,
                              const QString& elementId) const;

    /** Signal apex that the procedure has finished
     */
    void Finished();

    // Functions to get something from Apex

    /** Get list of trials for this procedure
     * will return a QList of ApexTrial objects
     */
    QObjectList GetTrials() const;
    float GetApexVersion() const;

    QStringList GetStimuli() const;
    QVariant GetFixedParameterValue(const QString stimulusID,
                                    const QString parameterID);

    // Misc functions
    /** Shows the user a messagebox with the specified messagebox
     */
    void MessageBox(const QString message) const;

    void AddWarning(const QString message) const;

    void Abort(const QString message) const;

private:
    apex::PluginProcedure* pluginProcedure;
};


}           // ns Apex

#endif
