#ifndef _EXPORL_SRC_LIB_APEXMAIN_EXPERIMENTCONTROL_H_
#define _EXPORL_SRC_LIB_APEXMAIN_EXPERIMENTCONTROL_H_

#include "apextools/status/errorlogger.h"

#include <QDate>
#include <QObject>

class QStateMachine;

namespace apex
{

namespace data
{
class ExperimentData;
}

namespace gui
{
class ApexMainWindow;
}

class ExperimentIo;
class ExperimentRunDelegate;
class ExperimentControlPrivate;

/**
 * @class ExperimentControl
 * @author Job Noorman <jobnoorman@gmail.com>
 *
 * @brief Class to run an experiment.
 *
 * This class controls the execution of an experiment.
 * Use loadExperiment(data::ExperimentData*) to set the experiment to
 * run and start() to run it. After a call to start(), isRunning() will
 * return true until experimentDone() has been emitted.
 */
class APEX_EXPORT ExperimentControl : public QObject, public ErrorLogger
{
        Q_OBJECT

        friend class ExperimentControlPrivate;

    public:

        enum Flag
        {
            NoFlags         = 0,
            AutoStart       = 1 << 0,
            NoResults       = 1 << 1,
            Deterministic   = 1 << 2
        };

        Q_DECLARE_FLAGS(Flags, Flag);

        ExperimentControl(Flags flags = NoFlags);

        ~ExperimentControl();

        void loadExperiment(ExperimentRunDelegate* runDelegate);

        /**
         * Starts the loaded experiment.
         */
        void start();

        /**
         * Returns true if the experiment is running, ie start() has been
         * called and experimentDone() has not been emitted yet.
         */
        bool isRunning() const;

        /**
         * Returns the time the experiment started. I.e. the time the first
         * trial was started.
         */
        QDateTime startTime() const;

        ExperimentIo* io() const;
        const QStateMachine* machine() const;

    Q_SIGNALS:

        /**
         * Emitted when the experiment is done.
         */
        void experimentDone();
        void savedResults(QString filename);

        void errorMessage(const QString& source, const QString& message);

    private:

        ExperimentControlPrivate* const d;
};

Q_DECLARE_OPERATORS_FOR_FLAGS(ExperimentControl::Flags)

}

#endif
