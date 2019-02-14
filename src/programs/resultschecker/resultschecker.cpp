#include "apextools/global.h"

#include <QDateTime>
#include <QDebug>
#include <QDomDocument>
#include <QFile>
#include <QLoggingCategory>
#include <QStringList>
#include <QXmlStreamReader>

Q_DECLARE_LOGGING_CATEGORY(APEX_RESULTSCHECKER)
Q_LOGGING_CATEGORY(APEX_RESULTSCHECKER, "apex.resultschecker")

template <typename T>
bool compare(T l, T r)
{
    return l == r;
}

template <>
bool compare(double l, double r)
{
    return qFuzzyCompare(l + 1, r + 1);
}

template <>
bool compare(float l, float r)
{
    return qFuzzyCompare(l + 1, r + 1);
}

QStringList trainingProcedureNames()
{
    QStringList result;

    result << "apex:trainingProcedure";
    result << "apex:trainingProcedureType";

    return result;
}

template <typename T>
class Value
{
public:
    Value() : set(false)
    {
    }
    Value(const T &v) : set(true), val(v)
    {
    }

    bool isSet() const
    {
        return set;
    }

    operator T() const
    {
        return val;
    }

    bool operator==(const Value &other) const
    {
        if (set != other.set)
            return false;
        if (set && !compare(val, other.val))
            return false;
        return true;
    }

    bool operator!=(const Value &other) const
    {
        return !(*this == other);
    }

    Value &operator=(const Value &other)
    {
        if (other.set) {
            val = other.val;
            set = true;
        } else
            set = false;

        return *this;
    }

private:
    bool set;
    T val;
};

struct DeviceResult {
    Value<int> bufferUnderruns;
    Value<int> clippedChannel;

    bool operator==(const DeviceResult &other) const
    {
        return bufferUnderruns == other.bufferUnderruns &&
               clippedChannel == other.clippedChannel;
    }
};

struct TrialResult {
    QString procedureType;
    QString id;
    Value<QString> answer;
    Value<QString> correctAnswer;
    Value<QString> correct_answer;
    Value<QString> correctInterval;
    Value<QString> answerInterval;
    // Value<QString> stimulus;
    Value<bool> correct;
    Value<int> responseTime;
    QMap<QString, QString> screenResults;
    QMap<QString, DeviceResult> deviceResults;
    QStringList stimuli;
    Value<double> stepSize;
    Value<double> parameter;
    Value<int> reversals;
    Value<bool> reversal;
    Value<bool> saturation;
    Value<bool> skip;
    Value<int> presentations;
    QString startTime;

    bool operator==(const TrialResult &other) const
    {
        bool isTrainingProcedure =
            trainingProcedureNames().contains(other.procedureType) ||
            trainingProcedureNames().contains(procedureType);

        QString errMsg = QString(" -> trial (%1) -> ").arg(id);

        bool ret = true;

        if (correctInterval.isSet()) {
            if (correctInterval != other.correct_answer) {
                qCDebug(APEX_RESULTSCHECKER)
                    << errMsg << "correctInterval != correct_answer";
                ret = false;
            }
        } else if (other.correctInterval.isSet()) {
            if (other.correctInterval != correct_answer) {
                qCDebug(APEX_RESULTSCHECKER)
                    << errMsg << "correctInterval != correct_answer";
                ret = false;
            }
        }

        QStringList a(stimuli);
        QStringList b(other.stimuli);
        a.sort();
        b.sort();
        if (a != b) {
            qCDebug(APEX_RESULTSCHECKER) << errMsg << "stimulus";
            ret = false;
        }

        if (!isTrainingProcedure && correct != other.correct) {
            qCDebug(APEX_RESULTSCHECKER) << errMsg << "correct";
            ret = false;
        }

        /*if (!(responseTime.isSet() && other.responseTime.isSet()))
        {
            qCDebug(APEX_RESULTSCHECKER) << errMsg << "responseTime";
            ret =  false;
        }*/

        if (!isTrainingProcedure && screenResults != other.screenResults) {
            qCDebug(APEX_RESULTSCHECKER) << errMsg << "screenResults";
            ret = false;
        }

        if (deviceResults != other.deviceResults) {
            qCDebug(APEX_RESULTSCHECKER) << errMsg << "deviceResults";
            ret = false;
        }

        /*if (stepSize != other.stepSize)
        {
            qCDebug(APEX_RESULTSCHECKER) << errMsg << "stepSize";
            ret =  false;
        }*/

        if (parameter != other.parameter) {
            qCDebug(APEX_RESULTSCHECKER) << errMsg << "parameter";
            ret = false;
        }

        // Reversals in trunk are off by one (reported one trial too late)
        /*if (reversals != other.reversals)
        {
            qCDebug(APEX_RESULTSCHECKER) << errMsg << "reversals";
            ret =  false;
        }

        if (reversal != other.reversal)
        {
            qCDebug(APEX_RESULTSCHECKER) << errMsg << "reversal";
            ret =  false;
        }*/

        if (saturation != other.saturation) {
            qCDebug(APEX_RESULTSCHECKER) << errMsg << "saturation";
            ret = false;
        }

        if (presentations != other.presentations) {
            qCDebug(APEX_RESULTSCHECKER) << errMsg << "presentations";
            ret = false;
        }

        if (skip != other.skip) {
            qCDebug(APEX_RESULTSCHECKER) << errMsg << "skip";
            ret = false;
        }

        return ret;

        //         return answer == other.answer &&
        //                correctAnswer == other.correctAnswer &&
        //                stimulus == other.stimulus &&
        //                correct == other.correct &&
        //                responseTime.isSet() && other.responseTime.isSet() &&
        //                screenResults == other.screenResults &&
        //                deviceResults == other.deviceResults;
    }
};

struct Results {
    Value<QDateTime> startDate;
    Value<QDateTime> endDate;
    Value<int> duration;
    QList<TrialResult> trialResults;
    QString procedureType;

    bool operator==(const Results &other) const
    {
        if (!(startDate.isSet() && other.startDate.isSet())) {
            qCDebug(APEX_RESULTSCHECKER) << "-> startDate";
            return false;
        }

        if (!(endDate.isSet() && other.endDate.isSet())) {
            qCDebug(APEX_RESULTSCHECKER) << "-> endDate";
            return false;
        }

        if (!(duration.isSet() && other.duration.isSet())) {
            qCDebug(APEX_RESULTSCHECKER) << "-> duration";
            return false;
        }

        bool isTrainingProcedure =
            trainingProcedureNames().contains(other.procedureType) ||
            trainingProcedureNames().contains(procedureType);

        if (isTrainingProcedure) {
            if (std::abs(trialResults.size() - other.trialResults.size()) ==
                1) {

                QList<TrialResult>::const_iterator it1;
                QList<TrialResult>::const_iterator it2;

                if (trialResults.size() > other.trialResults.size()) {
                    it1 = trialResults.begin();
                    it2 = other.trialResults.begin();
                } else {
                    it1 = other.trialResults.begin();
                    it2 = trialResults.begin();
                }
                ++it1;

                for (; it1 != trialResults.end(); ++it1, ++it2) {
                    if (!(*it1 == *it2)) {
                        qCDebug(APEX_RESULTSCHECKER) << "-> trialResults";
                        return false;
                    }
                }
            } else {
                qCDebug(APEX_RESULTSCHECKER) << "-> trialResults size";
                return false;
            }
        } else {
            if (trialResults != other.trialResults) {
                qCDebug(APEX_RESULTSCHECKER) << "-> trialResults";
                return false;
            }
        }

        return true;
        //         return startDate.isSet() && other.startDate.isSet() &&
        //                endDate.isSet() && other.endDate.isSet() &&
        //                duration.isSet() && other.duration.isSet() &&
        //                trialResults == other.trialResults;
    }
};

class StringException
{
public:
    StringException(const QString &m) : msg(m)
    {
    }

    QString message() const
    {
        return msg;
    }

private:
    QString msg;
};

QDomElement getRootElement(const QString &fileName)
{
    QFile file(fileName);

    if (!file.open(QFile::ReadOnly))
        throw StringException("Cannot open file");

    QDomDocument doc;

    if (!doc.setContent(&file))
        throw StringException("Parse error");

    return doc.documentElement();
}

void unknownTag(const QString &tag)
{
    throw StringException(QString("Unknown tag %1").arg(tag));
}

bool stringToBool(const QString &s)
{
    if (s.compare("true", Qt::CaseInsensitive) == 0)
        return true;
    else if (s.compare("false", Qt::CaseInsensitive) == 0)
        return false;
    else
        throw StringException(QString("Invalid boolean value \"%1\"").arg(s));
}

void parseGeneral(QDomElement general, Results *results)
{
    QDomElement child;

    for (child = general.firstChildElement(); !child.isNull();
         child = child.nextSiblingElement()) {
        QString tag = child.tagName();

        if (tag == "startdate") {
            results->startDate = QDateTime::fromString(child.text());
        } else if (tag == "enddate") {
            results->endDate = QDateTime::fromString(child.text());
        } else if (tag == "duration") {
            results->duration = child.text().toInt();
        } else if (tag == "apex_version" || tag == "apex_version_git") {
            // ignored
        } else if (tag == "jscript") {
            // ignored
        } else if (tag == "hostname") {
            // ignored
        } else if (tag == "serial") {
            // ignored
        } else if (tag == "device_id") {
            // ignored
        } else if (tag == "stopped_by") {
            // ignored
        } else {
            unknownTag(tag);
        }
    }
}

Results parseRefactoryResults(const QString &file)
{
    Results ret;

    QDomElement root = getRootElement(file);
    QDomElement child;

    QString experimentFile = root.attribute("experiment_file");
    experimentFile.remove("file:");
    QFile f(experimentFile);
    if (f.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QXmlStreamReader xmlReader;
        xmlReader.setDevice(&f);
        xmlReader.readNext();

        while (!xmlReader.atEnd()) {
            if (xmlReader.isStartElement()) {
                if (xmlReader.name() == "procedure") {
                    QXmlStreamAttributes attributes = xmlReader.attributes();

                    ret.procedureType.append(attributes.value("xsi:type"));
                    break;
                }
            }
            xmlReader.readNext();
        }
    } else {
        qCDebug(APEX_RESULTSCHECKER) << "Couldn't open experiment file";
    }

    for (child = root.firstChildElement(); !child.isNull();
         child = child.nextSiblingElement()) {
        QString tag = child.tagName();

        if (tag == "general")
            parseGeneral(child, &ret);
        else if (tag == "trial") {
            QString id = child.attribute("id");
            TrialResult t;
            t.id = id;
            t.procedureType = ret.procedureType;

            for (QDomElement trialChild = child.firstChildElement();
                 !trialChild.isNull();
                 trialChild = trialChild.nextSiblingElement()) {
                QString tag = trialChild.tagName();

                if (tag == "procedure") {
                    for (QDomElement procChild = trialChild.firstChildElement();
                         !procChild.isNull();
                         procChild = procChild.nextSiblingElement()) {
                        QString tag = procChild.tagName();

                        if (tag == "correct_answer")
                            t.correctAnswer = procChild.text();
                        else if (tag == "correct_interval")
                            t.correctInterval = procChild.text();
                        else if (tag == "answer_interval")
                            t.answerInterval = procChild.text();
                        else if (tag == "answer")
                            t.answer = procChild.text();
                        else if (tag == "stimulus")
                            t.stimuli.push_back(procChild.text());
                        else if (tag == "standard")
                            t.stimuli.push_back(procChild.text());
                        else if (tag == "correct")
                            t.correct = stringToBool(procChild.text());
                        else if (tag == "parameter")
                            t.parameter = procChild.text().toDouble();
                        else if (tag == "stepsize")
                            t.stepSize = procChild.text().toDouble();
                        else if (tag == "reversals")
                            t.reversals = procChild.text().toInt();
                        else if (tag == "reversal")
                            t.reversal = true;
                        else if (tag == "saturation")
                            t.saturation = stringToBool(procChild.text());
                        else if (tag == "presentations")
                            t.presentations = procChild.text().toInt();
                        else if (tag == "skip")
                            t.skip = true;
                        else
                            unknownTag(tag);
                    }
                } else if (tag == "screenresult") {
                    for (QDomElement element = trialChild.firstChildElement();
                         !element.isNull();
                         element = element.nextSiblingElement()) {
                        if (element.tagName() != "element")
                            unknownTag(element.tagName());

                        t.screenResults[element.attribute("id")] =
                            element.text();
                    }
                } else if (tag == "output") {
                    for (QDomElement outputChild =
                             trialChild.firstChildElement();
                         !outputChild.isNull();
                         outputChild = outputChild.nextSiblingElement()) {
                        QString tag = outputChild.tagName();

                        if (tag == "device") {
                            QString devId = outputChild.attribute("id");

                            for (QDomElement devChild =
                                     outputChild.firstChildElement();
                                 !devChild.isNull();
                                 devChild = devChild.nextSiblingElement()) {
                                QString tag = devChild.tagName();

                                if (tag == "buffer") {
                                    QString underruns = "underruns";
                                    bool ok = false;

                                    if (devChild.hasAttribute(underruns))
                                        t.deviceResults[devId].bufferUnderruns =
                                            devChild.attribute(underruns).toInt(
                                                &ok);

                                    if (!ok)
                                        unknownTag(tag);
                                } else if (tag == "clipped") {
                                    QString channel = "channel";
                                    bool ok = false;

                                    if (devChild.hasAttribute(channel))
                                        t.deviceResults[devId].clippedChannel =
                                            devChild.attribute(channel).toInt(
                                                &ok);

                                    if (!ok)
                                        unknownTag(tag);
                                } else
                                    unknownTag(tag);
                            }
                        }
                    }
                } else if (tag == "responsetime") {
                    t.responseTime = trialChild.text().toInt();
                } else if (tag == "randomgenerators") {
                    // TODO
                } else if (tag == "trial_start_time") {
                    t.startTime = trialChild.text();
                } else
                    unknownTag(tag);
            }

            ret.trialResults.push_back(t);
        }
    }

    return ret;
}

Results parseTrunkResults(const QString &file)
{
    Results ret;

    QDomElement root = getRootElement(file);
    QDomElement child;

    for (child = root.firstChildElement(); !child.isNull();
         child = child.nextSiblingElement()) {
        QString tag = child.tagName();

        if (tag == "general")
            parseGeneral(child, &ret);
        else if (tag == "trial") {
            QString id = child.attribute("id");
            TrialResult t;
            t.id = id;

            for (QDomElement trialChild = child.firstChildElement();
                 !trialChild.isNull();
                 trialChild = trialChild.nextSiblingElement()) {
                QString tag = trialChild.tagName();

                if (tag == "procedure") {
                    for (QDomElement procChild = trialChild.firstChildElement();
                         !procChild.isNull();
                         procChild = procChild.nextSiblingElement()) {
                        QString tag = procChild.tagName();

                        if (tag == "correct_answer")
                            t.correct_answer = procChild.text();
                        else if (tag == "answer")
                            t.answer = procChild.text();
                        else if (tag == "stimulus")
                            t.stimuli.push_back(procChild.text());
                        else if (tag == "standard")
                            t.stimuli.push_back(procChild.text());
                        else if (tag == "correct")
                            t.correct = stringToBool(procChild.text());
                        else if (tag == "parameter")
                            t.parameter = procChild.text().toDouble();
                        else if (tag == "stepsize")
                            t.stepSize = procChild.text().toDouble();
                        else if (tag == "reversals")
                            t.reversals = procChild.text().toInt();
                        else if (tag == "reversal")
                            t.reversal = true;
                        else if (tag == "saturation")
                            t.saturation = stringToBool(procChild.text());
                        else if (tag == "presentations")
                            t.presentations = procChild.text().toInt();
                        else if (tag == "skip")
                            t.skip = true;
                        else if (tag == "trial_start_time")
                            t.startTime = procChild.text();
                        else
                            unknownTag(tag);
                    }
                } else if (tag == "screenresult") {
                    for (QDomElement element = trialChild.firstChildElement();
                         !element.isNull();
                         element = element.nextSiblingElement()) {
                        if (element.tagName() != "element")
                            unknownTag(element.tagName());

                        t.screenResults[element.attribute("id")] =
                            element.text();
                    }
                } else if (tag == "corrector") {
                    for (QDomElement correctorChild =
                             trialChild.firstChildElement();
                         !correctorChild.isNull();
                         correctorChild = correctorChild.nextSiblingElement()) {
                        QString tag = correctorChild.tagName();

                        if (tag == "result") {
                            t.correct = stringToBool(correctorChild.text());
                        } else if (tag == "answer")
                            t.answer = correctorChild.text();
                        else if (tag == "correctanswer")
                            t.correctAnswer = correctorChild.text();
                        else
                            unknownTag(tag);
                    }
                } else if (tag == "output") {
                    for (QDomElement outputChild =
                             trialChild.firstChildElement();
                         !outputChild.isNull();
                         outputChild = outputChild.nextSiblingElement()) {
                        QString tag = outputChild.tagName();

                        if (tag == "device") {
                            QString devId = outputChild.attribute("id");

                            for (QDomElement devChild =
                                     outputChild.firstChildElement();
                                 !devChild.isNull();
                                 devChild = devChild.nextSiblingElement()) {
                                QString tag = devChild.tagName();

                                if (tag == "buffer") {
                                    QString underruns = "underruns";
                                    bool ok = false;

                                    if (devChild.hasAttribute(underruns))
                                        t.deviceResults[devId].bufferUnderruns =
                                            devChild.attribute(underruns).toInt(
                                                &ok);

                                    if (!ok)
                                        unknownTag(tag);
                                } else if (tag == "clipped") {
                                    QString channel = "channel";
                                    bool ok = false;

                                    if (devChild.hasAttribute(channel))
                                        t.deviceResults[devId].clippedChannel =
                                            devChild.attribute(channel).toInt(
                                                &ok);

                                    if (!ok)
                                        unknownTag(tag);
                                } else
                                    unknownTag(tag);
                            }
                        }
                    }
                } else if (tag == "responsetime") {
                    t.responseTime = trialChild.text().toInt();
                } else if (tag == "randomgenerators") {
                    // TODO
                } else if (tag == "trial_start_time") {
                    t.startTime = trialChild.text();
                } else
                    unknownTag(tag);
            }
            ret.trialResults.push_back(t);
        }
    }

    return ret;
}

int main(int argc, char *argv[])
{
    if (argc != 3) {
        qCDebug(APEX_RESULTSCHECKER) << "Usage:" << argv[0]
                                     << "<trunk results> <refactory results>";
        return 1;
    }

    Results trunkResults;
    Results refactoryResults;

    try {
        trunkResults = parseTrunkResults(argv[1]);
    } catch (const StringException &e) {
        qCWarning(APEX_RESULTSCHECKER, "Error parsing %s: %s", argv[1],
                  qPrintable(e.message()));
        return 1;
    }

    try {
        refactoryResults = parseRefactoryResults(argv[2]);
    } catch (const StringException &e) {
        qCWarning(APEX_RESULTSCHECKER, "Error parsing %s: %s", argv[2],
                  qPrintable(e.message()));
        return 1;
    }

    if (trunkResults == refactoryResults)
        return 0;
    else
        return 1;
}
