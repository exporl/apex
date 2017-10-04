#ifndef _EXPORL_SRC_LIB_APEXDATA_PROCEDURE_TRIAL_H_
#define _EXPORL_SRC_LIB_APEXDATA_PROCEDURE_TRIAL_H_

#include <QMap>
#include <QObject>
#include <QSharedDataPointer>
#include <QVariant>

#include "apextools/global.h"

class QString;
class QVariant;

namespace apex
{

namespace data
{

struct TrialPrivate;

/**
 * Configuration of the next trial to be presented
 */
class APEXDATA_EXPORT Trial : public QObject
{
    Q_OBJECT
public:
    Trial(QObject *parent = 0);
    Trial(const Trial &other);
    virtual ~Trial();

    virtual Trial &operator=(const Trial &other);

    typedef QList<Trial> List;

public slots:

    /** Add a screen to the list of screens to be shown
    * Every subsequently added stimulus will be associated with this screen
    * @param id the id of the screen
    * @param acceptresponse if true, the response of this screen will be
    * reported in the next processResult call
    * @param timeout if timeout >0, the screen wil disappear after timeout s.
    * If acceptresponse is true, timeout must be <=0
    */
    virtual void addScreen(const QString &id, bool acceptResponse,
                           double timeout);

    /**
     * Add a stimulus to be played during the last added screen
     *
     * @param id id of the stimulus to be added
     * @param parameters parameters to be set before the stimulus is played
     * @param highlightElement id of screenelement to be highlighted during
     *                         presentation of this stimulus
     * @param waitafter time in s to wait after the stimulus was played
     */
    virtual void addStimulus(const QString &id,
                             const QMap<QString, QVariant> &parameters,
                             const QString &highlightedElement,
                             double waitAfter);

    virtual void setId(const QString &id);
    virtual void setAnswer(const QString &answer);

    virtual bool isValid() const;

    virtual int screenCount() const;
    virtual QString screen(int screenIndex) const;
    virtual bool acceptResponse(int screenIndex) const;
    virtual double timeout(int screenIndex) const;

    virtual int stimulusCount(int screenIndex) const;
    virtual QString stimulus(int screenIndex, int stimulusIndex) const;
    virtual QMap<QString, QVariant> parameters(int screenIndex,
                                               int stimulusIndex) const;
    virtual QString highlightedElement(int screenIndex,
                                       int stimulusIndex) const;
    virtual bool doWaitAfter(int screenIndex, int stimulusIndex) const;
    virtual double waitAfter(int screenIndex, int stimulusIndex);
    virtual QString id() const;
    virtual QString answer() const;

    /**
     * Clear all data
     */
    virtual void reset();

private:
    QSharedDataPointer<TrialPrivate> d;
};

} // ns data
} // ns apex

#endif
