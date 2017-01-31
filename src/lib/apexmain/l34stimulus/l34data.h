#ifndef _EXPORL_SRC_LIB_APEXMAIN_L34STIMULUS_L34DATA_H_
#define _EXPORL_SRC_LIB_APEXMAIN_L34STIMULUS_L34DATA_H_

#include "apextools/exceptions.h"
#include "apextools/global.h"

#include <cmath>

namespace apex
{
namespace stimulus
{


class APEX_EXPORT L34Stimulus
{
public:
    qint16 channel;
    float magnitude;
    float period;
    qint16 mode;
    float phaseWidth;
    float phaseGap;

    // for aseq
    int activeElectrode;
    int returnElectrode;
    int currentLevel;


    L34Stimulus() :
        channel(0),
        magnitude(0),
        period(-1),
        mode(-1),
        phaseWidth(-1),
        phaseGap(-1),
        activeElectrode(-1),
        returnElectrode(0),
        currentLevel(-1)
    {
    }

    // Matthias: [Constructor for handling regular qic files]
    L34Stimulus( qint16 pChannel, float pMagnitude) :

        channel( pChannel ),
        magnitude( pMagnitude )
    {

        period      = -1;
        mode        = -1;
        phaseWidth  = -1;
        phaseGap    = -1;
        activeElectrode = -1;
        returnElectrode = 0;
        currentLevel = -1;
    }

    // Matthias: [Constructor for handling extended qic-file (.qicext extension)]
    L34Stimulus( qint16 pChannel,
        float pMagnitude,
        float pPeriod,
        qint16 pMode,
        float pPhaseWidth,
        float pPhaseGap ):

        channel( pChannel ),
        magnitude( pMagnitude ),
        period( pPeriod ),
        mode( pMode ),
        phaseWidth( pPhaseWidth ),
        phaseGap( pPhaseGap )

        {
            activeElectrode = -1;
            returnElectrode = 0;
            currentLevel = -1;
        }


        // Constructor for aseq
    L34Stimulus( qint16 pChannel,
                    float pMagnitude,
                    float pPeriod,
                    float pPhaseWidth,
                    float pPhaseGap,
                    int pActiveElectrode,
                    int pReturnElectrode,
                    int pCurrentLevel
                ):

            channel( pChannel ),
            magnitude( pMagnitude ),
            period( pPeriod ),
            phaseWidth( pPhaseWidth ),
            phaseGap( pPhaseGap ),
            activeElectrode ( pActiveElectrode ),
            returnElectrode ( pReturnElectrode),
            currentLevel ( pCurrentLevel )
    {
        mode=-1;
        if (!isQuantized(pPeriod))
            qCDebug(APEX_RS, "Warning: period not quantized: %1.10f",pPeriod);
        if (!isQuantized(pPhaseWidth))
            qCDebug(APEX_RS, "Warning: period not quantized: %1.4f",pPeriod);
        if (!isQuantized(pPhaseGap))
            qCDebug(APEX_RS, "Warning: period not quantized: %1.4f",pPeriod);

        /*if (channel<0)      // powerup
            channel=0;*/
    }


    bool operator== (const L34Stimulus& s) const
    {
        return s.channel==channel &&
                s.magnitude==magnitude &&
                s.period==period &&
                s.phaseWidth==phaseWidth &&
                s.phaseGap==phaseGap &&
                s.activeElectrode==activeElectrode &&
                s.returnElectrode==returnElectrode &&
                s.currentLevel==currentLevel &&
                s.mode==mode;
    }

    /**
     * The L34 only supports times quantized to 200ns (due to the internal
     * clock of 5MHz
     */
    bool isQuantized(float n) const
    {
        float cycles= std::floor( 5*n+0.5 );
        if (std::abs(cycles/5 - n) > 1e-4) {            // 23 bits are used for the base
            //qCDebug(APEX_RS, "Difference: %f",cycles/5-n);
            return false;
        } else
            return true;
    }
};

typedef QVector<L34Stimulus> L34Data;

}
}


#endif
