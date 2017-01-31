#ifndef _EXPORL_SRC_LIB_APEXMAIN_CORRECTOR_EQUALCORRECTOR_H_
#define _EXPORL_SRC_LIB_APEXMAIN_CORRECTOR_EQUALCORRECTOR_H_

#include "apextools/global.h"

#include "corrector.h"

namespace apex {

class APEX_EXPORT EqualCorrector : public Corrector
{
public:
    virtual const QVariant compare(const QVariant& a, const QVariant& b) const;
    virtual ~EqualCorrector();
};

}   // ns apex

#endif
