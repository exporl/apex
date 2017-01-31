#include <QDateTime>
#include <QFile>
#include <QDomDocument>
#include <QDebug>
#include <QStringList>

template<typename T>
bool compare(T l, T r)
{
    return l == r;
}

template<>
bool compare(double l, double r)
{
    return qFuzzyCompare(l, r);
}

template<>
bool compare(float l, float r)
{
    return qFuzzyCompare(l, r);
}

template<typename T>
class Value
{
    public:

        Value() : set(false) {}
        Value(const T& v) : set(true), val(v) {}

        bool isSet() const
        {
            return set;
        }

        operator T() const
        {
            return val;
        }

        bool operator==(const Value& other) const
        {
            if (set!=other.set)
                return false;
            if (set && ! compare(val, other.val))
                return false;
            return true;
        }

        bool operator!=(const Value& other) const
        {
            return !(*this == other);
        }

        Value& operator=(const Value& other)
        {
            if (other.set)
            {
                val = other.val;
                set = true;
            }
            else
                set = false;

            return *this;
        }

    private:

        bool set;
        T val;
};

struct DeviceResult
{
    Value<int> bufferUnderruns;

    bool operator==(const DeviceResult& other) const
    {
        return bufferUnderruns == other.bufferUnderruns;
    }
};

struct TrialResult
{
    QString id;
    Value<QString> answer;
    Value<QString> correctAnswer;
    Value<QString> correctInterval;
    Value<QString> answerInterval;
    //Value<QString> stimulus;
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


    bool operator==(const TrialResult& other) const
    {
        QString errStr = QString(" -> trial (%1) -> ").arg(id);
        const char* errMsg = qPrintable(errStr);

        bool ret = true;




        if (correctInterval.isSet()) {
            if (correctInterval != other.correctAnswer) {
                qDebug() << errMsg << "correctInterval != correctAnswer";
                ret =  false;
            }
        } else if (other.correctInterval.isSet()) {
                if (other.correctInterval != correctAnswer) {
                    qDebug() << errMsg << "correctInterval != correctAnswer";
                    ret =  false;
                }
                if (other.answerInterval != answer) {
                    qDebug() << errMsg << "answer != answerInterval";
                    ret =  false;
                }
        } else {
            if (correctAnswer != other.correctAnswer)
            {
                qDebug() << errMsg << "correctAnswer";
                ret =  false;
            }
            if (answer != other.answer)
            {
                qDebug() << errMsg << "answer:" << answer << other.answer;
                ret = false;
            }
        }

        QStringList a(stimuli);
        QStringList b(other.stimuli);
        a.sort();
        b.sort();
        if (a != b)
        {
            qDebug() << errMsg << "stimulus";
            ret =  false;
        }

        if (correct != other.correct)
        {
            qDebug() << errMsg << "correct";
            ret =  false;
        }

        /*if (!(responseTime.isSet() && other.responseTime.isSet()))
        {
            qDebug() << errMsg << "responseTime";
            ret =  false;
        }*/

        if (screenResults != other.screenResults)
        {
            qDebug() << errMsg << "screenResults";
            ret =  false;
        }

        if (deviceResults != other.deviceResults)
        {
            qDebug() << errMsg << "deviceResults";
            ret =  false;
        }


        /*if (stepSize != other.stepSize)
        {
            qDebug() << errMsg << "stepSize";
            ret =  false;
        }*/

        if (parameter != other.parameter)
        {
            qDebug() << errMsg << "parameter";
            ret =  false;
        }


        // Reversals in trunk are off by one (reported one trial too late)
        /*if (reversals != other.reversals)
        {
            qDebug() << errMsg << "reversals";
            ret =  false;
        }

        if (reversal != other.reversal)
        {
            qDebug() << errMsg << "reversal";
            ret =  false;
        }*/

        if (saturation != other.saturation)
        {
            qDebug() << errMsg << "saturation";
            ret =  false;
        }

        if (presentations != other.presentations)
        {
            qDebug() << errMsg << "presentations";
            ret =  false;
        }

        if (skip != other.skip)
        {
            qDebug() << errMsg << "skip";
            ret =  false;
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

struct Results
{
    Value<QDateTime> startDate;
    Value<QDateTime> endDate;
    Value<int> duration;
    QList<TrialResult> trialResults;

    bool operator==(const Results& other) const
    {
        if (!(startDate.isSet() && other.startDate.isSet()))
        {
            qDebug() << "-> startDate";
            return false;
        }

        if (!(endDate.isSet() && other.endDate.isSet()))
        {
            qDebug() << "-> endDate";
            return false;
        }

        if (!(duration.isSet() && other.duration.isSet()))
        {
            qDebug() << "-> duration";
            return false;
        }

        if (trialResults != other.trialResults)
        {
            qDebug() << "-> trialResults";
            return false;
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

        StringException(const QString& m) : msg(m) {}

        QString message() const {return msg;}

    private:

        QString msg;
};

QDomElement getRootElement(const QString& fileName)
{
    QFile file(fileName);

    if (!file.open(QFile::ReadOnly))
        throw StringException("Cannot open file");

    QDomDocument doc;

    if (!doc.setContent(&file))
        throw StringException("Parse error");

    return doc.documentElement();
}

void unknownTag(const QString& tag)
{
    throw StringException(QString("Unknown tag %1").arg(tag));
}

bool stringToBool(const QString& s)
{
    if (s.compare("true", Qt::CaseInsensitive) == 0)
        return true;
    else if (s.compare("false", Qt::CaseInsensitive) == 0)
        return false;
    else
        throw StringException(QString("Invalid boolean value \"%1\"").arg(s));
}

void parseGeneral(QDomElement general, Results* results)
{
    QDomElement child;

    for (child = general.firstChildElement();
         !child.isNull();
         child = child.nextSiblingElement())
    {
        QString tag = child.tagName();

        if (tag == "startdate")
            results->startDate = QDateTime::fromString(child.text());
        else if (tag == "enddate")
            results->endDate = QDateTime::fromString(child.text());
        else if (tag == "duration")
            results->duration = child.text().toInt();
        else
            unknownTag(tag);
    }
}

Results parseRefactoryResults(const QString& file)
{
    Results ret;

    QDomElement root = getRootElement(file);
    QDomElement child;

    for (child = root.firstChildElement();
         !child.isNull();
         child = child.nextSiblingElement())
    {
        QString tag = child.tagName();

        if (tag == "general")
            parseGeneral(child, &ret);
        else if (tag == "trial")
        {
            QString id = child.attribute("id");
            TrialResult t;
            t.id = id;

            for (QDomElement trialChild = child.firstChildElement();
                 !trialChild.isNull();
                 trialChild = trialChild.nextSiblingElement())
            {
                QString tag = trialChild.tagName();

                if (tag == "procedure")
                {
                    for (QDomElement procChild = trialChild.firstChildElement();
                         !procChild.isNull();
                         procChild = procChild.nextSiblingElement())
                    {
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
                            t.stimuli.push_back( procChild.text() );
                        else if (tag == "standard")
                            t.stimuli.push_back( procChild.text() );
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
                }
                else if (tag == "screenresult")
                {
                    for (QDomElement element = trialChild.firstChildElement();
                         !element.isNull();
                         element = element.nextSiblingElement())
                    {
                        if (element.tagName() != "element")
                            unknownTag(element.tagName());

                        t.screenResults[element.attribute("id")]
                            = element.text();
                    }
                }
                else if (tag == "output")
                {
                    for (QDomElement outputChild = trialChild.firstChildElement();
                         !outputChild.isNull();
                         outputChild = outputChild.nextSiblingElement())
                    {
                        QString tag = outputChild.tagName();

                        if (tag == "device")
                        {
                            QString devId = outputChild.attribute("id");

                            for (QDomElement devChild = outputChild.firstChildElement();
                                 !devChild.isNull();
                                 devChild = devChild.nextSiblingElement())
                            {
                                QString tag = devChild.tagName();

                                if (tag == "buffer")
                                {
                                    t.deviceResults[devId]
                                        .bufferUnderruns = devChild.text().toInt();
                                }
                                else
                                    unknownTag(tag);
                            }
                        }
                    }
                }
                else if (tag == "responsetime")
                {
                    t.responseTime = trialChild.text().toInt();
                }
                else if (tag == "randomgenerators")
                {
                    //TODO
                }
                else
                    unknownTag(tag);
            }

            ret.trialResults.push_back(t);
        }
    }

    return ret;
}

Results parseTrunkResults(const QString& file)
{
    Results ret;

    QDomElement root = getRootElement(file);
    QDomElement child;

    for (child = root.firstChildElement();
         !child.isNull();
         child = child.nextSiblingElement())
    {
        QString tag = child.tagName();

        if (tag == "general")
            parseGeneral(child, &ret);
        else if (tag == "trial")
        {
            QString id = child.attribute("id");
            TrialResult t;
            t.id = id;

            for (QDomElement trialChild = child.firstChildElement();
                 !trialChild.isNull();
                 trialChild = trialChild.nextSiblingElement())
            {
                QString tag = trialChild.tagName();

                if (tag == "procedure")
                {
                    for (QDomElement procChild = trialChild.firstChildElement();
                         !procChild.isNull();
                         procChild = procChild.nextSiblingElement())
                    {
                        QString tag = procChild.tagName();


                        if (tag == "correct_answer")
                            t.correctAnswer = procChild.text();
                        else if (tag == "answer")
                            t.answer = procChild.text();
                        else if (tag == "stimulus")
                            t.stimuli.push_back( procChild.text() );
                        else if (tag == "standard")
                            t.stimuli.push_back( procChild.text() );
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
                }
                else if (tag == "screenresult")
                {
                    for (QDomElement element = trialChild.firstChildElement();
                         !element.isNull();
                         element = element.nextSiblingElement())
                    {
                        if (element.tagName() != "element")
                            unknownTag(element.tagName());

                        t.screenResults[element.attribute("id")]
                        = element.text();
                    }
                }
                else if (tag == "corrector")
                {
                    for (QDomElement correctorChild = trialChild.firstChildElement();
                         !correctorChild.isNull();
                         correctorChild = correctorChild.nextSiblingElement())
                    {
                        QString tag = correctorChild.tagName();

                        if (tag == "result")
                        {
                            t.correct =
                                stringToBool(correctorChild.text());
                        }
                        else if (tag == "answer")
                            t.answer = correctorChild.text();
                        else if (tag == "correctanswer")
                            t.correctAnswer = correctorChild.text();
                        else
                            unknownTag(tag);
                    }
                }
                else if (tag == "output")
                {
                    for (QDomElement outputChild = trialChild.firstChildElement();
                         !outputChild.isNull();
                         outputChild = outputChild.nextSiblingElement())
                    {
                        QString tag = outputChild.tagName();

                        if (tag == "device")
                        {
                            QString devId = outputChild.attribute("id");

                            for (QDomElement devChild = outputChild.firstChildElement();
                                 !devChild.isNull();
                                 devChild = devChild.nextSiblingElement())
                            {
                                QString tag = devChild.tagName();

                                if (tag == "buffer")
                                {
                                    t.deviceResults[devId]
                                    .bufferUnderruns = devChild.text().toInt();
                                }
                                else
                                    unknownTag(tag);
                            }
                        }
                    }
                }
                else if (tag == "responsetime")
                {
                    t.responseTime = trialChild.text().toInt();
                }
                else if (tag == "randomgenerators")
                {
                    //TODO
                }
                else
                    unknownTag(tag);
            }
            ret.trialResults.push_back(t);
        }
    }

    return ret;
}

int main(int argc, char* argv[])
{
    if (argc != 3)
    {
        qDebug() << "Usage:" << argv[0] << "<trunk results> <refactory results>";
        return 1;
    }

    Results trunkResults;
    Results refactoryResults;

    try
    {
        trunkResults = parseTrunkResults(argv[1]);
    }
    catch (const StringException& e)
    {
        qWarning("Error parsing %s: %s", argv[1], qPrintable(e.message()));
        return 1;
    }

    try
    {
        refactoryResults = parseRefactoryResults(argv[2]);
    }
    catch (const StringException& e)
    {
        qWarning("Error parsing %s: %s", argv[2], qPrintable(e.message()));
        return 1;
    }

    if (trunkResults == refactoryResults)
        return 0;
    else
        return 1;
}
