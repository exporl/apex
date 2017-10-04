/******************************************************************************
 * Copyright (C) 2007-2013  Michael Hofmann <mh21@mh21.de>                    *
 *                                                                            *
 * This program is free software; you can redistribute it and/or modify       *
 * it under the terms of the GNU General Public License as published by       *
 * the Free Software Foundation; either version 3 of the License, or          *
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

/** @file
 * Abstract CI interface declaration.
 */

#ifndef _EXPORL_COH_LIB_COH_COH_H_
#define _EXPORL_COH_LIB_COH_COH_H_

#include "common/global.h"

#include <QByteArray>
#include <QList>
#include <QObject>
#include <QString>
#include <QVariant>

#include <QtGlobal>

#ifdef COH_MAKEDLL
#define COH_EXPORT Q_DECL_EXPORT
#else
#define COH_EXPORT Q_DECL_IMPORT
#endif

// Q_DECLARE_LOGGING_CATEGORY
extern COH_EXPORT const QLoggingCategory &EXPORL_COH();

namespace coh
{

class CohCommand;
class CohCommandVisitor;
class CohMetaData;
class CohBiphasicStimulus;
class CohCodacsStimulus;
class CohNullStimulus;
class CohSequence;

class COH_EXPORT Coh : public QObject
{
    Q_OBJECT
    Q_ENUMS(CommandType Electrode Status TriggerMode Type
                AmplitudeModulationMode CommandProperty)
    Q_FLAGS(CommandTypes CommandProperties)
public:
    enum CommandType {
        BiphasicStimulus = 0x0001,
        CodacsStimulus = 0x0002,
        NullStimulus = 0x0004,
        Sequence = 0x0008,
        AllStimuli =
            BiphasicStimulus | CodacsStimulus | NullStimulus | Sequence,
        MetaData = 0x0010,
        AllCommands = 0xFFFF,
    };
    Q_DECLARE_FLAGS(CommandTypes, CommandType)

    enum CommandProperty {
        Trigger = 0x0001,
        Period = 0x0002,
        ActiveElectrode = 0x0004,
        ReferenceElectrode = 0x0008,
        CurrentLevel = 0x0010,
        PhaseWidth = 0x0020,
        PhaseGap = 0x0040,
        Amplitude = 0x0080,
        ChannelMagnitude = 0x0100,
        ChannelIndex = 0x0200,
        NoProperties = 0,
        NullProperties = Trigger | Period,
        BiphasicProperties = Trigger | Period | ActiveElectrode |
                             ReferenceElectrode | CurrentLevel | PhaseWidth |
                             PhaseGap,
        CodacsProperties = Trigger | Period | Amplitude
    };
    Q_DECLARE_FLAGS(CommandProperties, CommandProperty)

    enum Type {
        Cic3Implant,
        Cic4Implant,
    };

    enum AmplitudeModulationMode {
        CurrentUnit,
        Ampere,
        Rectangular,
        CustomAmplitudeModulation,
    };

    /** CI trigger type. Represents the different types  of start for capturing
     * and streaming.
     *
     * THESE VALUES NEED TO BE KEPT SYNCHRONIZED WITH NIC3SLAVE!
     *
     * @ingroup ci
     */
    enum TriggerMode {
        /// Immediate start of stimulation after the command has been received
        Immediate = 0,
        /// Waits for an external trigger before starting the stimulation
        External = 1,
        /// Starts immediately and generates an external trigger signal
        Bilateral = 2
    };

    /** CI streaming status. Represents the different states of the CI
     * interface.
     *
     * THESE VALUES NEED TO BE KEPT SYNCHRONIZED WITH NIC3SLAVE!
     *
     * @ingroup ci
     */
    enum Status {
        /// Unknown operating state, most likely an error occurred
        Unknown = 0,
        /// Streaming to the speech processor in progress.
        Streaming = 1,
        /// Hardware is idle and waiting for further commands
        Idle = 2,
        /// An error related to safety was encountered, hardware is stopped
        SafetyError = 3,
        /// A general error was encountered, hardware is stopped
        Error = 4,
        /// Streaming completed successfully, hardware can now be stopped
        Stopped = 5,
        /// Hardware waiting for an external trigger signal to begin streaming
        Waiting = 6,
        /// Hardware ran out of sequence data whilst streaming, is stopped
        Underflow = 7
    };

    /** CI electrode. Represents the different electrodes that are available for
     * a CI.
     *
     * @ingroup ci
     */
    enum Electrode {
        /// Intra-cochlear electrode 0 (virtual)
        CI_ELECTRODE_IC0 = 0,
        /// Intra-cochlear electrode 1
        CI_ELECTRODE_IC1 = 1,
        /// Intra-cochlear electrode 2
        CI_ELECTRODE_IC2 = 2,
        /// Intra-cochlear electrode 3
        CI_ELECTRODE_IC3 = 3,
        /// Intra-cochlear electrode 4
        CI_ELECTRODE_IC4 = 4,
        /// Intra-cochlear electrode 5
        CI_ELECTRODE_IC5 = 5,
        /// Intra-cochlear electrode 6
        CI_ELECTRODE_IC6 = 6,
        /// Intra-cochlear electrode 7
        CI_ELECTRODE_IC7 = 7,
        /// Intra-cochlear electrode 8
        CI_ELECTRODE_IC8 = 8,
        /// Intra-cochlear electrode 9
        CI_ELECTRODE_IC9 = 9,
        /// Intra-cochlear electrode 10
        CI_ELECTRODE_IC10 = 10,
        /// Intra-cochlear electrode 11
        CI_ELECTRODE_IC11 = 11,
        /// Intra-cochlear electrode 12
        CI_ELECTRODE_IC12 = 12,
        /// Intra-cochlear electrode 13
        CI_ELECTRODE_IC13 = 13,
        /// Intra-cochlear electrode 14
        CI_ELECTRODE_IC14 = 14,
        /// Intra-cochlear electrode 15
        CI_ELECTRODE_IC15 = 15,
        /// Intra-cochlear electrode 16
        CI_ELECTRODE_IC16 = 16,
        /// Intra-cochlear electrode 17
        CI_ELECTRODE_IC17 = 17,
        /// Intra-cochlear electrode 18
        CI_ELECTRODE_IC18 = 18,
        /// Intra-cochlear electrode 19
        CI_ELECTRODE_IC19 = 19,
        /// Intra-cochlear electrode 20
        CI_ELECTRODE_IC20 = 20,
        /// Intra-cochlear electrode 21
        CI_ELECTRODE_IC21 = 21,
        /// Intra-cochlear electrode 22
        CI_ELECTRODE_IC22 = 22,
        /// Intra-cochlear electrode 23
        CI_ELECTRODE_IC23 = 23,
        /// Intra-cochlear electrode 24
        CI_ELECTRODE_IC24 = 24,
        /// Intra-cochlear electrode 25
        CI_ELECTRODE_IC25 = 25,
        /// Intra-cochlear electrode 26
        CI_ELECTRODE_IC26 = 26,
        /// Intra-cochlear electrode 27
        CI_ELECTRODE_IC27 = 27,
        /// Intra-cochlear electrode 28
        CI_ELECTRODE_IC28 = 28,
        /// Intra-cochlear electrode 29
        CI_ELECTRODE_IC29 = 29,
        /// Intra-cochlear electrode 30
        CI_ELECTRODE_IC30 = 30,
        /// Intra-cochlear electrode 31 (virtual)
        CI_ELECTRODE_IC31 = 31,

        /// Extra-cochlear electrode 1
        CI_ELECTRODE_EC1 = -1,
        /// Extra-cochlear electrode 2
        CI_ELECTRODE_EC2 = -2,
        /// Extra-cochlear electrodes 1 and 2 (internally connected)
        CI_ELECTRODE_EC1_2 = -3,
        /// Electrode for the Vss supply voltage
        CI_ELECTRODE_VSS = -4,
        /// Electrode for the Vdda supply voltage
        CI_ELECTRODE_VDDA = -5,
        /// Electrode for the Vddd supply voltage
        CI_ELECTRODE_VDDD = -6,
        /// Electrode for the Vref supply voltage
        CI_ELECTRODE_VREF = -7,
        /// Electrode for the Vio supply voltage
        CI_ELECTRODE_VIO = -8,
        /// Electrode for the Vdd supply voltage
        CI_ELECTRODE_VDD = -9
    };
};

Q_DECLARE_OPERATORS_FOR_FLAGS(Coh::CommandTypes)
Q_DECLARE_OPERATORS_FOR_FLAGS(Coh::CommandProperties)

/** Visitor for classes derived from CohCommand. Implement the abstract
 * methods to be able to distinguish between the different CI command types
 * while walking a CohSequence tree with CohCommand#accept().
 *
 * @see CohCommand
 *
 * @ingroup ci
 */
class COH_EXPORT CohCommandVisitor
{
public:
    /** Virtual destructor to make the compiler happy. */
    virtual ~CohCommandVisitor()
    {
    }

    /** Reimplement to handle an instance of CohNullStimulus.
     *
     * @param command visited command of type CohNullStimulus
     */
    virtual void visit(CohNullStimulus *command) = 0;

    /** Reimplement to handle an instance of CohBiphasicStimulus.
     *
     * @param command visited command of type CohBiphasicStimulus
     */
    virtual void visit(CohBiphasicStimulus *command) = 0;

    /** Reimplement to handle an instance of CohCodacsStimulus.
     *
     * @param command visited command of type CohCodacsStimulus
     */
    virtual void visit(CohCodacsStimulus *command) = 0;

    /** Reimplement to handle an instance of CohSequence.
     *
     * @param command visited command of type CohSequence
     */
    virtual void visit(CohSequence *command) = 0;

    /** Reimplement to handle an instance of CohMetaData.
     *
     * @param command visited command of type CohMetaData
     */
    virtual void visit(CohMetaData *command) = 0;
};

/** Base class for CI commands that can be visited with
 * CohCommandVisitor.
 *
 * @see CohCommandVisitor
 *
 * @ingroup ci
 */
class COH_EXPORT CohCommand
{
public:
    /** Virtual destructor to make the compiler happy. */
    virtual ~CohCommand();

    /** Reimplement for derived classes to call the appropriate method of the
     * passed CohCommandVisitor.
     *
     * @param visitor visitor
     */
    virtual void accept(CohCommandVisitor *visitor) = 0;

    Coh::CommandProperties availableProperties() const;
    bool availableProperties(Coh::CommandProperties tested) const;
    Coh::CommandProperties requiredProperties() const;
    Coh::CommandProperties missingProperties() const;
    bool isValid() const;

protected:
    CohCommand();
    CohCommand(Coh::CommandProperties properties,
               Coh::CommandProperties required);

protected:
    Coh::CommandProperties properties;
    Coh::CommandProperties required;
};

// The following classes do not use private data to reduce the amount of
// allocations needed if a lot of stimuli need to be constructed

/** Just meta data. Contains additional key=value information that is not sent
 * to the CI.
 *
 * While you can store arbitrary types, dependent classes might be more
 * restrictive in what they accept.
 *
 * @ingroup ci
 */
class COH_EXPORT CohMetaData : public CohCommand
{
public:
    CohMetaData(const QString &key, const QVariant &value);
    CohMetaData(const QString &key, const QString &type, const QString &value);

    QString key() const;
    void setKey(const QString &value);

    QVariant value() const;
    QString valueType() const;
    void setValue(const QVariant &value);
    void setValue(const QString &type, const QString &value);

    /** Calls CohCommandVisitor#visit(CohMetaData*).
     *
     * @param visitor visitor
     */
    virtual void accept(CohCommandVisitor *visitor);

protected:
    QString first;
    QVariant second;
};

class COH_EXPORT CohStimulus : public CohCommand
{
public:
    /** Returns whether or not a trigger signal should be generated when the CI
     * stimulus is delivered.
     *
     * @return true if a trigger signal should be generated when the CI
     * stimulus is delivered.
     */
    bool hasTrigger() const;

    /** Sets whether or not a trigger signal should be generated when the CI
     * stimulus is delivered.
     *
     * @param  trigger true if a trigger signal should be generated when the CI
     * stimulus is delivered.
     */
    void setTrigger(bool trigger);

    /** Returns the distance to the following CI command.
     *
     * @return distance to the following CI command in us
     */
    double period() const;

    /** Sets the distance to the following CI command.
     *
     * @param interval distance to the following CI command in us
     */
    void setPeriod(double interval);

    // This is for compatibility with older XML files that were exported with
    // unrounded periods, but only with one digit precision
    static void setRoundToRfCycles(bool round);

protected:
    CohStimulus(Coh::CommandProperties properties,
                Coh::CommandProperties required, double interval, bool trigger);

    bool trigger;
    double interval;
    static bool roundToRfCycles;
};

/** CI null stimulus command. Delivers a stimulus to the CI, but does not do
 * actual stimulation of electrodes. This type of stimulus command is useful for
 * powerup sequences or silence periods during which the CI should still be
 * powered up.
 *
 * @ingroup ci
 */
class COH_EXPORT CohNullStimulus : public CohStimulus
{
public:
    /** Creates a new CI null stimulus command.
     *
     * @param interval distance to the following CI command in us, defaults to
     * 200 us
     * @param trigger true if a trigger signal should be generated when the CI
     * stimulus is delivered, defaults to false
     */
    CohNullStimulus(double interval, bool trigger);

    /** Calls CohCommandVisitor#visit(CohNullStimulus*).
     *
     * @param visitor visitor
     */
    virtual void accept(CohCommandVisitor *visitor);

    static CohNullStimulus *incompleteStimulus();

private:
    CohNullStimulus();
};

/** CI biphasic stimulus command. Delivers a two phase, charge balanced stimulus
 * between the two specified electrodes. The stimulus has a leading cathodic
 * phase.
 *
 * @ingroup ci
 */
class COH_EXPORT CohBiphasicStimulus : public CohStimulus
{
public:
    /** Creates a new CI biphasic stimulus command.
     *
     * @param active active electrode
     * @param reference reference electrode
     * @param level current level between 0 and 255
     * @param width width of each of the two phases in us
     * @param gap width of the gap between the two phases in us
     * @param interval distance to the following CI command in us, defaults to
     * 1000 us
     * @param trigger true if a trigger signal should be generated when the CI
     * stimulus is delivered, defaults to false
     */
    CohBiphasicStimulus(Coh::Electrode active, Coh::Electrode reference,
                        unsigned level, double width, double gap,
                        double interval, bool trigger);

    /** Returns the active electrode for the biphasic stimulus.
     *
     * @return active electrode
     */
    Coh::Electrode activeElectrode() const;

    /** Sets the active electrode for the biphasic stimulus.
     *
     * @param active active electrode
     */
    void setActiveElectrode(Coh::Electrode active);

    /** Returns the reference electrode for the biphasic stimulus.
     *
     * @return reference electrode
     */
    Coh::Electrode referenceElectrode() const;

    /** Sets the reference electrode for the biphasic stimulus.
     *
     * @param reference reference electrode
     */
    void setReferenceElectrode(Coh::Electrode reference);

    /** Returns the current level for the biphasic stimulus.
     *
     * @return current level between 0 and 255
     */
    unsigned currentLevel() const;

    /** Sets the current level for the biphasic stimulus.
     *
     * @param level current level between 0 and 255
     */
    void setCurrentLevel(unsigned level);

    /** Returns the width of each of the two phases.
     *
     * @return width of each of the two phases in us
     */
    double phaseWidth() const;

    /** Sets the width of each of the two phases.
     *
     * @param width width of each of the two phases in us
     */
    void setPhaseWidth(double width);

    /** Returns the width of the gap between the two phases.
     *
     * @return width of the gap between the two phases in us
     */
    double phaseGap() const;

    /** Sets the width of the gap between the two phases.
     *
     * @param gap width of the gap between the two phases in us
     */
    void setPhaseGap(double gap);

    unsigned channelIndex() const;
    void setChannelIndex(unsigned channel);

    double channelMagnitude() const;
    void setChannelMagnitude(double magnitude);

    /** Calls CohCommandVisitor#visit(CohBiphasicStimulus*).
     *
     * @param visitor visitor
     */
    virtual void accept(CohCommandVisitor *visitor);

    static CohBiphasicStimulus *incompleteStimulus();

private:
    CohBiphasicStimulus();

    Coh::Electrode active;
    Coh::Electrode reference;
    unsigned level;
    double width;
    double gap;
    unsigned channel;
    double magnitude;
};

/** CI Codacs stimulus command. Delivers a HR frame with a certain amplitude
 * suitable for Codacs middle-ear implant devices. Does not derive from
 * CohBiphasicStimulus to provide type safety.
 *
 * @ingroup ci
 */
class COH_EXPORT CohCodacsStimulus : public CohStimulus
{
public:
    /** Creates a new Codacs stimulus command.
     *
     * @param level current level between -2048 and 2047
     * @param interval distance to the following CI command in us
     * @param trigger true if a trigger signal should be generated when the
     * Codacs stimulus is delivered
     */
    CohCodacsStimulus(int level, double interval, bool trigger);

    /** Returns the amplitude for the Codacs stimulus.
     *
     * @return amplitude between -2048 and 2047
     */
    int amplitude() const;

    /** Returns the amplitude for the Codacs stimulus after ulaw expansion.
     *
     * @return amplitude between -32768 and 32767
     */
    int expandedAmplitude() const;

    /** Sets the amplitude for the Codacs stimulus.
     *
     * @param level current level between -2048 and 2047
     */
    void setAmplitude(int level);

    static double defaultPeriod();

    // 5CPC HR encoding
    Coh::Electrode activeElectrode() const;
    unsigned currentLevel() const;

    static Coh::Electrode referenceElectrode();
    static double phaseWidth();
    static double phaseGap();

    /** Calls CohCommandVisitor#visit(CohCodacsStimulus*).
     *
     * @param visitor visitor
     */
    virtual void accept(CohCommandVisitor *visitor);

    static CohCodacsStimulus *incompleteStimulus();

private:
    CohCodacsStimulus();

private:
    int level;
};

/** CI stimulus sequence. Sequences deliver stimulus commands to the CI and can
 * be nested. If you add a CohCommand to the sequence, it is freed when
 * the sequence is destroyed.
 *
 * The current implementation subclasses QList, but disables the implicit
 * sharing.
 *
 * @ingroup ci
 */
class COH_EXPORT CohSequence : public QList<CohCommand *>, public CohCommand
{
public:
    /** Creates a new empty sequence. You can specify the number of times the
     * sequence is repeated.
     *
     * @param repeats number of times the sequence is repeated
     */
    CohSequence(unsigned repeats);

    /** Frees all CI commands in the sequence. To prevent this, remove the
     * commands prior to destruction.
     */
    ~CohSequence();

    /** Returns the number of times the sequence is repeated.
     *
     * @return number of times the sequence is repeated
     */
    unsigned numberRepeats() const;

    /** Sets the number of times the sequence is repeated.
     *
     * @param repeats number of times the sequence is repeated
     */
    void setNumberRepeats(unsigned repeats);

    /** Calls CohCommandVisitor#visit(CohSequence*).
     *
     * @param visitor visitor
     */
    virtual void accept(CohCommandVisitor *visitor);

private:
    unsigned repeats;
    Q_DISABLE_COPY(CohSequence)
};

// currents in micro amps
COH_EXPORT double cohCurrentForCurrentLevel(unsigned currentLevel,
                                            Coh::Type implant);
// currents in micro amps
COH_EXPORT unsigned cohCurrentLevelForCurrent(double current,
                                              Coh::Type implant);

// period in us, output in RF cycles, negative periods get set to 0
COH_EXPORT unsigned roundedCohCycles(double period);
// period in us, output in us, negative periods get set to 0
COH_EXPORT double roundedCohPeriod(double period);

} // namespace coh

#endif
