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
 
#ifndef __MYQWTPLOT_H_
#define __MYQWTPLOT_H_

#include "qwt_plot.h"
#include "qwt_plot_curve.h"
#include "streamappdefines.h"
using namespace streamapp;

class ScrollZoomer;

namespace guicore
{

    /**
      * QwtPlotter
      *   plots Stream objects to a QwtPlot.
      *   Used to wrap the QwtPlot calls that require passing a plain array.
      *   @see myQwtPlot
      ********************************************************************** */
  class QwtPlotter
  {
  public:
      /**
        * Constructor.
        * @param a_Plot the plot
        * @param ac_nChannels the number of curves to use
        * @param ac_nBufferSize the number of samples
        */
    QwtPlotter( QwtPlot& a_Plot, const unsigned ac_nChannels, const unsigned ac_nBufferSize );

      /**
        * Destructor.
        */
    ~QwtPlotter();

      /**
        * Plot min( ac_nChannels, ac_Data.mc_nChannels ) of ac_nBufferSize samples.
        * @param ac_Data the data to plot
        */
    void mp_Plot( const Stream& ac_Data );

      /**
        * Plot min( ac_nChannels, ac_Data.mc_nChannels ) of ac_nSamples samples.
        * @param ac_Data the data to plot
        * @param ac_nSamples number of samples to take from ac_Data
        */
    void mp_Plot( const Stream& ac_Data, const unsigned ac_nSamples );

      /**
        * Plot one channel of ac_nBufferSize samples on the specified curve.
        */
    void mp_Plot( const StreamChannel& ac_Data, const unsigned ac_nCurve = 0 );

      /**
        * Plot one channel of ac_nSamples samples on the specified curve.
        * @param ac_Data the data to plot
        * @param ac_nSamples number of samples to take from ac_Data
        * @param ac_nCurve the curve to plot on
        */
    void mp_Plot( const StreamChannel& ac_Data, const unsigned ac_nSamples, const unsigned ac_nCurve );

      /**
        * Make a copy of the samples or use the supplied buffer.
        * By default a copy of the data passed to mp_Plot() is used.
        * If you're sure the data stays valid however, i t greatly
        * improves speed by using the raw data buffers.
        * @param ac_bSet true for no copy
        */
    INLINE void mp_SetNoDeepCopy( const bool ac_bSet = false )
    {
      mv_bNoDeepCopy = ac_bSet;
    }

      /**
        * Curves.
        */
    typedef ArrayStorage<QwtPlotCurve*> mt_Curves;

      /**
        * Axis.
        */
    typedef ArrayStorage<double> mt_Axis;

      /**
        * Get all curves.
        */
    INLINE mt_Curves& mp_GetCurves()
    {
      return m_Curves;
    }

      /**
        * Get one curve.
        * @param ac_nCurve the curve to get
        */
    INLINE QwtPlotCurve* mp_GetCurve( const unsigned ac_nCurve )
    {
      return m_Curves( ac_nCurve );
    }

      /**
        * Install a new curve, deleting the old one.
        * @param a_pCurve the new QwtPlotCurve
        * @param ac_nWhere index of the new curve, must be in range
        */
    void mp_InstallCurve( QwtPlotCurve* a_pCurve, const unsigned ac_nWhere );

  private:
    void mp_InitCurves();
    void mp_InitAxis();
    mt_Curves       m_Curves;
    mt_Axis         m_dAxis;
    QwtPlot&        m_Plot;

  protected:
    const unsigned  mc_nChan;
    const unsigned  mc_nSize;
    bool            mv_bNoDeepCopy;

    QwtPlotter( const QwtPlotter& );
    QwtPlotter& operator = ( const QwtPlotter& );
   };

    /**
      * myQwtPlot
      *   defines how the QwtPlot should look and extends functionality.
      *   Uses QwtPlotter to draw Stream objects, supports zooming and changing
      *   the scale labels.
      ************************************************************************* */
  class myQwtPlot : public QwtPlot, public QwtPlotter
  {
    Q_OBJECT
  public:
      /**
        * Constructor.
        * @param a_pParent the parent widget or 0 for desktoplevel
        * @param ac_sName the plot's title
        * @param ac_nSamples the number of samples
        * @param ac_nChannels the number of curves to use
        */
    myQwtPlot( QWidget* a_pParent, const QString& ac_sName, const unsigned ac_nSamples, const unsigned ac_nChannels = 1 );

      /**
        * Destructor.
        */
    ~myQwtPlot();

      /**
        * Draw frequency labels on the X axis.
        * @param ac_dFrequency the frequency of the last sample
        */
    void mp_SetFrequencyScaleDraw( const double ac_dFrequency );

      /**
        * Set the Y axis scale.
        * When zooming this will be overridden when zoomed.
        * @param ac_dScale the +/- maximum
        */
    void mp_SetYAxisScale( const double ac_dScale );

      /**
        * Call this method after everything is done and you want a zoomer.
        * Due to buggy behaviour (see below) the zooming will only work ok
        * if zoomer is created after setting axes scales.
        * @warning Calling QwtPlot::setAxisScale() while QwtPlot::autoReplot() is FALSE
        * leaves the axis in an 'intermediate' state.
        * In this case, to prevent buggy behaviour, your must call
        * QwtPlot::replot() before calling QwtPlotPicker::scaleRect(),
        * QwtPlotZoomer::scaleRect(), QwtPlotPicker::QwtPlotPicker() or
        * QwtPlotZoomer::QwtPlotZoomer().
        * @param ac_bCreateZoomer true for zooming capabilities
        */
    void mp_PlotSetupFinished( const bool ac_bCreateZoomer = false );

  private slots:
      /**
        * Due to other, unexplained buggy behaviour, we need to replot after zooming,
        * or the data will not match the y-axis. So we let ScrollZoomer emit a signal
        * after zooming, catch it here and do a replot.
        */
    void mp_PlotRescaled();

  private:
    ScrollZoomer* m_pZoomer;

    myQwtPlot( const myQwtPlot& );
    myQwtPlot& operator = ( const myQwtPlot& );
  };

}

#endif //#ifndef __MYQWTPLOT_H_
