#include "stimulusparameters.h"


using namespace apex::data;



QVariant StimulusParameters::value(const QString& id) const
{
    return values.value(id);
}

void StimulusParameters::insert(const QString& id, const QVariant& value)
{
    values[id]=value;
}

bool StimulusParameters::contains(const QString &id) const
{
    return values.contains(id);
}

bool StimulusParameters::operator==(const StimulusParameters& other) const
{
    return values==other.values;
}

bool StimulusParameters::isEmpty() const
{
    return values.isEmpty();
}

const QMap<QString,QVariant>& StimulusParameters::map() const
{
    return values;
}

