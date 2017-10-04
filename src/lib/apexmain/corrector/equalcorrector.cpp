#include "equalcorrector.h"

namespace apex
{

const QVariant EqualCorrector::compare(const QVariant &a,
                                       const QVariant &b) const
{
    return a == b;
}

EqualCorrector::~EqualCorrector()
{
}
}
