/******************************************************************************
 * Copyright (C) 2008  Tom Francart <tom.francart@med.kuleuven.be>            *
 *                                                                            *
 * This file is part of APEX 3.                                               *
 *                                                                            *
 * APEX 3 is free software: you can redistribute it and/or modify             *
 * it under the terms of the GNU General Public License as published by       *
 * the Free Software Foundation, either version 2 of the License, or          *
 * (at your option) any later version.                                        *
 *                                                                            *
 * APEX 3 is distributed in the hope that it will be useful,                  *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of             *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the              *
 * GNU General Public License for more details.                               *
 *                                                                            *
 * You should have received a copy of the GNU General Public License          *
 * along with APEX 3.  If not, see <http://www.gnu.org/licenses/>.            *
 *****************************************************************************/
 
#ifndef __QWTPARTIALCURVE_H__
#define __QWTPARTIALCURVE_H__

#include <qwt_plot_curve.h>
#include "containers/array.h"
using namespace streamapp;

namespace guicore
{

    /**
      * QwtPartialCurve
      *   draws items on a QwtPlotCurve at the specified samples only.
      *   Uses an array of booleans to specify which samples should
      *   be drawn. By default, nothing is drawn.
      *   Note that this should not be used to draw large ranges,
      *   since this will be quite slow because only one item
      *   is drawn at a time. This also affects the global speed.
      *   To draw a range, just set the X axis.
      *   @see QwtPlot
      **************************************************************** */
  class QwtPartialCurve : public QwtPlotCurve
  {
  public:
      /**
        * Constructor.
        * @param ac_nSamples the total number of samples
        */
    QwtPartialCurve( const unsigned ac_nSamples );

      /**
        * Constructor overload.
        * @see QwtPlotCurve
        * @param title the curve's title
        * @param ac_nSamples the total number of samples
        */
    QwtPartialCurve( const QwtText& title, const unsigned ac_nSamples  );

      /**
        * Constructor overload.
        * @see QwtPlotCurve
        * @param title the curve's title
        * @param ac_nSamples the total number of samples
        */
    QwtPartialCurve( const QString& title, const unsigned ac_nSamples  );

      /**
        * Destructor.
        */
    virtual ~QwtPartialCurve();

      /**
        * Array of booleans specifying where to draw.
        */
    typedef ArrayStorage<bool> mt_DrawdSamples;

      /**
        * Set the array for partial drawing.
        * All samples that are true will be drawed, the rest is ignored.
        * @param ac_Array the array
        */
    void mp_SetPartialArray( const mt_DrawdSamples& ac_Array );

      /**
        * Get the current partial array.
        * @return const reference to the mt_DrawdSamples
        */
    INLINE const mt_DrawdSamples& mf_GetPartialArray() const
    {
      return m_Partial;
    }

  protected:
      /**
        * Override drawCurve.
        * Draws only at specified samples.
        * @see QwtPlotCurve::drawCurve
        */
    virtual void drawCurve( QPainter *p, int style, const QwtScaleMap &xMap, const QwtScaleMap &yMap, int from, int to ) const;

      /**
        * Override drawSymbols.
        * Draws only at specified samples.
        * @see QwtPlotCurve::drawSymbols
        */
    virtual void drawSymbols( QPainter *p, const QwtSymbol &, const QwtScaleMap &xMap, const QwtScaleMap &yMap, int from, int to ) const;

  private:
    mt_DrawdSamples m_Partial;

    QwtPartialCurve( const QwtPartialCurve &c );
    QwtPartialCurve& operator = ( const QwtPartialCurve &c );
  };

}

#endif //#ifndef __QWTPARTIALCURVE_H__
