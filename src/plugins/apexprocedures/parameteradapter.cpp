/******************************************************************************
 * Copyright (C) 2010  Job Noorman <jobnoorman@gmail.com>                     *
 *                                                                            *
 * This file is part of APEX 4.                                               *
 *                                                                            *
 * APEX 4 is free software: you can redistribute it and/or modify             *
 * it under the terms of the GNU General Public License as published by       *
 * the Free Software Foundation, either version 2 of the License, or          *
 * (at your option) any later version.                                        *
 *                                                                            *
 * APEX 4 is distributed in the hope that it will be useful,                  *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of             *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the              *
 * GNU General Public License for more details.                               *
 *                                                                            *
 * You should have received a copy of the GNU General Public License          *
 * along with APEX 4.  If not, see <http://www.gnu.org/licenses/>.            *
 *****************************************************************************/

#include "parameteradapter.h"

#include <QDebug>

namespace apex
{

struct ParameterAdapterPrivate {
    const data::AdaptiveProcedureData *parameters;

    data::adapting_parameter currentParameter;
    float currentStepsize;
    bool firstAnswer;
    bool lastAnswerCorrect;
    int nCorrect;
    int nWrong;
    int nReversals;

    /**
     * @brief The Direction enum, direction has the value INIT when no answer is
     * processed yet.
     * The direction is UP when the subject is providing correct answers.
     * The direction is DOWN when the subject is providing wrong answers.
     * REMARK: this is different from the terminology used in literature, where
     * UP is when the subject provides a wrong answer.
     */
    enum Direction { INIT, UP, DOWN } direction;

    bool lastReversal;
};

ParameterAdapter::ParameterAdapter(
    const data::AdaptiveProcedureData *parameters)
    : d(new ParameterAdapterPrivate())
{
    d->parameters = parameters;
    d->lastReversal = false;
    d->direction = ParameterAdapterPrivate::INIT;
    reset();
}

ParameterAdapter::~ParameterAdapter()
{
    delete d;
}

data::adapting_parameter ParameterAdapter::currentParameter() const
{
    return d->currentParameter;
}

float ParameterAdapter::currentStepsize() const
{
    return d->currentStepsize;
}

bool ParameterAdapter::lastReversal() const
{
    return d->lastReversal;
}

int ParameterAdapter::nReversals() const
{
    return d->nReversals;
}

void ParameterAdapter::updateParameter(const QVariant &answer, int trialNb,
                                       data::adapting_parameter usedParameter,
                                       bool keepStepSize)
{
    d->currentParameter = usedParameter;
    d->lastReversal = false;

    Q_ASSERT(answer.canConvert(QVariant::Bool));
    bool currentAnswerCorrect = answer.toBool();

    d->firstAnswer = false;
    d->lastAnswerCorrect = currentAnswerCorrect;

    // update the number of correct/wrong answers given in a row
    if (currentAnswerCorrect) {
        d->nCorrect++;
        d->nWrong = 0;
    } else {
        d->nWrong++;
        d->nCorrect = 0;
    }

    // check if we need to update the parameter
    bool doUpdate = true;
    bool increaseParameter;

    if (d->nCorrect == d->parameters->nDown()) {
        increaseParameter = !d->parameters->largerIsEasier();
        d->nCorrect = 0;

        // don't update reversals after the first answers
        if (d->direction == ParameterAdapterPrivate::DOWN) {
            d->lastReversal = true;
            d->nReversals++;
        }
        d->direction = ParameterAdapterPrivate::UP;
    } else if (d->nWrong == d->parameters->nUp()) {
        increaseParameter = d->parameters->largerIsEasier();
        d->nWrong = 0;

        // don't update reversals after the first answers
        if (d->direction == ParameterAdapterPrivate::UP) {
            d->lastReversal = true;
            d->nReversals++;
        }
        d->direction = ParameterAdapterPrivate::DOWN;
    } else
        doUpdate = false;

    if (!keepStepSize)
        updateStepsize(trialNb);

    if (doUpdate) {
        if (increaseParameter)
            d->currentParameter += d->currentStepsize;
        else
            d->currentParameter -= d->currentStepsize;
    }
}

int ParameterAdapter::numberOfReversals() const
{
    return d->nReversals;
}

void ParameterAdapter::reset()
{
    d->firstAnswer = true;
    d->currentParameter = d->parameters->startValue();
    d->nCorrect = 0;
    d->nWrong = 0;
    d->nReversals = 0;
    d->direction = ParameterAdapterPrivate::INIT;
    updateStepsize(0);
}

void ParameterAdapter::updateStepsize(int trialNb)
{
    int threshold = 0;

    switch (d->parameters->changeStepsizeAfter()) {
    case data::AdaptiveProcedureData::ChangeAfterReversals:

        threshold = d->nReversals;
        break;

    case data::AdaptiveProcedureData::ChangeAfterTrials:

        threshold = trialNb;
        break;

    default:

        qFatal("Unknown procedure step size adaptation");
        break;
    }

    QMap<int, float> stepsizes;
    // REMARK: The direction enum doesn't follow the terminology from the
    // literature (see the definition of Direction) while the name of the
    // stepsizes does. So, when the direction is UP we need to use the 'down
    // stepsizes' and vice versa.
    if (ParameterAdapterPrivate::UP == d->direction) {
        stepsizes = d->parameters->downStepsizes();
    } else if (ParameterAdapterPrivate::DOWN == d->direction) {
        stepsizes = d->parameters->upStepsizes();
    } else if (ParameterAdapterPrivate::INIT == d->direction) {
        stepsizes = d->parameters->upStepsizes();
    }

    Q_FOREACH (int startValue, stepsizes.keys()) {
        if (startValue <= threshold)
            d->currentStepsize = stepsizes[startValue];
        else
            break; // keys are always sorted in a QMap
    }

    qCDebug(APEX_RS) << "Updated stepsize for trial" << trialNb << "to"
                     << d->currentStepsize;
}

} // ns apex
