#ifndef _EXPORL_SRC_LIB_APEXDATA_STIMULUS_STIMULUSPARAMETERS_H_
#define _EXPORL_SRC_LIB_APEXDATA_STIMULUS_STIMULUSPARAMETERS_H_


#include "apextools/global.h"
#include <QMap>
#include <QString>
#include <QVariant>

namespace apex
{

namespace data
{

class APEXDATA_EXPORT StimulusParameters
{

    public:
        QVariant value(const QString& id) const;
        void insert(const QString& id, const QVariant& value);
        bool contains(const QString& id) const;
        const QMap<QString,QVariant>& map() const;
        bool isEmpty() const;

        bool operator==(const StimulusParameters& other) const;

    private:
        QMap<QString,QVariant> values;


};

}
}


#endif

