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
 
#include "qwtplot.h"
#include "scrollzoomer.h"
#include "qwt_scale_div.h"
#include "qwt_double_interval.h"
#include "qwt_scale_engine.h"
#include "qwt_scale_draw.h"
#include "appcore/qt_utils.h"

using namespace guicore;

QwtPlotter::QwtPlotter( QwtPlot& a_Plot, const unsigned ac_nChannels, const unsigned ac_nBufferSize ) :
  m_Curves( ac_nChannels, true ),
  m_dAxis ( ac_nBufferSize ),
  m_Plot  ( a_Plot ),
  mc_nChan( ac_nChannels ),
  mc_nSize( ac_nBufferSize ),
  mv_bNoDeepCopy( false )
{
  mp_InitAxis();
  mp_InitCurves();
}

QwtPlotter::~QwtPlotter()
{
}

void QwtPlotter::mp_Plot( const Stream& ac_Data )
{
  mp_Plot( ac_Data, mc_nSize );
}

void QwtPlotter::mp_Plot( const Stream& ac_Data, const unsigned ac_nSamples )
{
  const unsigned nChanCanPlot = s_min( mc_nChan, ac_Data.mf_nGetChannelCount() );
  assert( ac_nSamples <= ac_Data.mf_nGetBufferSize() );
  assert( ac_nSamples <= mc_nSize );
  const double*  pAxis = m_dAxis.mf_pGetArray();
  double**       pSrc  = ac_Data.mf_pGetArray();

  for( unsigned i = 0 ; i < nChanCanPlot ; ++i )
  {
    if( mv_bNoDeepCopy )
      m_Curves( i )->setRawData( pAxis, pSrc[ i ], ac_nSamples );
    else
      m_Curves( i )->setData( pAxis, pSrc[ i ], ac_nSamples );
  }
}

void QwtPlotter::mp_Plot( const StreamChannel& ac_Data, const unsigned ac_nCurve )
{
  mp_Plot( ac_Data, mc_nSize, ac_nCurve );
}

void QwtPlotter::mp_Plot( const StreamChannel& ac_Data, const unsigned ac_nSamples, const unsigned ac_nCurve )
{
  assert( mc_nChan > 0 );
  assert( ac_nSamples <= ac_Data.mf_nGetBufferSize() );
  assert( ac_nSamples <= mc_nSize );
  assert( ac_nCurve < mc_nChan );
  const double*  pAxis = m_dAxis.mf_pGetArray();
  double*        pSrc  = ac_Data.mf_pGetArray();

  if( mv_bNoDeepCopy )
    m_Curves( ac_nCurve )->setRawData( pAxis, pSrc, ac_nSamples );
  else
    m_Curves( ac_nCurve )->setData( pAxis, pSrc, ac_nSamples );
}

void QwtPlotter::mp_InstallCurve( QwtPlotCurve* a_pCurve, const unsigned ac_nWhere )
{
  assert( a_pCurve );
  delete m_Curves( ac_nWhere );
  m_Curves.mp_Set( ac_nWhere, a_pCurve );
  a_pCurve->attach( &m_Plot );
}

void QwtPlotter::mp_InitCurves()
{
  for( unsigned i = 0 ; i < m_Curves.mf_nGetBufferSize() ; ++i )
  {
    m_Curves.mp_Set( i, new QwtPlotCurve( "blah" ) ); //FIXME
    m_Curves( i )->attach( &m_Plot );
  }
}

void QwtPlotter::mp_InitAxis()
{
  for( unsigned i = 0 ; i < m_dAxis.mf_nGetBufferSize() ; ++i )
    m_dAxis.mp_Set( i, (double) i );
}

namespace guicore
{
    /**
      * FrequencyScaleDraw
      *   used to override the axis labels to a frequency scale
      ********************************************************* */
  class FrequencyScaleDraw : public QwtScaleDraw
  {
  public:
    FrequencyScaleDraw( const double ac_dFrequency, const unsigned ac_nSamples ) :
      mc_dFrequency( ac_dFrequency ),
      mc_dSamples( (double) ac_nSamples )
    {}
    virtual ~FrequencyScaleDraw()
    {}

    virtual QwtText label( double a_dSample ) const
    {
      const double c_dFreq = ( a_dSample / mc_dSamples ) * mc_dFrequency;
      return qn( c_dFreq );
    }

    const double& mf_dFrequency() const
    { return mc_dFrequency; }

  private:
    const double mc_dFrequency;
    const double mc_dSamples;

    FrequencyScaleDraw( const FrequencyScaleDraw& );
    FrequencyScaleDraw& operator = ( const FrequencyScaleDraw& );
  };

    /**
      * AxisScaleDraw
      *   used to override the axis labels with a scalefactor
      ******************************************************* */
  class AxisScaleDraw : public QwtScaleDraw
  {
  public:
    AxisScaleDraw( const double ac_dScale ) :
      mc_dScale( ac_dScale )
    {}
    virtual ~AxisScaleDraw()
    {}

    virtual QwtText label( double a_dSample ) const
    {
      const double c_dVal = a_dSample * mc_dScale;
      return qn( c_dVal );
    }

    const double& mf_dScale() const
    { return mc_dScale; }

  private:
    const double mc_dScale;

    AxisScaleDraw( const AxisScaleDraw& );
    AxisScaleDraw& operator = ( const AxisScaleDraw& );
  };
}

myQwtPlot::myQwtPlot( QWidget* a_pParent, const QString& ac_sName, const unsigned ac_nSamples, const unsigned ac_nChannels ) :
    QwtPlot( ac_sName, a_pParent ),
    QwtPlotter( *this, ac_nChannels, ac_nSamples ),
  m_pZoomer( 0 )
{
  QwtPlot::setAxisScale( QwtPlot::xBottom, 0.0, (double) ac_nSamples );
  QwtPlot::setFrameStyle( QFrame::NoFrame );
  QwtPlot::setLineWidth( 0 );
  QwtPlot::setCanvasLineWidth( 2 );
  QwtPlot::setAutoReplot();
  QwtPlot::setTitle( ac_sName );
  QwtPlot::setCanvasBackground( QColor( 29, 100, 141 ) ); //nice blue
  const QRect r = QwtPlot::geometry();
}

myQwtPlot::~myQwtPlot( )
{
}

void myQwtPlot::mp_SetFrequencyScaleDraw( const double ac_dFrequency )
{
  QwtPlot::setAxisScaleDraw( QwtPlot::xBottom, new FrequencyScaleDraw( ac_dFrequency, mc_nSize ) );
  const QRect r = QwtPlot::geometry();
}

void myQwtPlot::mp_SetYAxisScale( const double ac_dScale )
{
  QwtPlot::setAxisScaleDraw( QwtPlot::yLeft, new AxisScaleDraw( ac_dScale ) );
}

void myQwtPlot::mp_PlotSetupFinished( const bool ac_bCreateZoomer )
{
  if( m_pZoomer )
  {
    disconnect( this, SLOT( mp_PlotRescaled() ) );
    delete m_pZoomer;
  }

  QwtPlot::replot();

  if( ac_bCreateZoomer )
  {
    m_pZoomer = new ScrollZoomer( QwtPlot::canvas() );
    m_pZoomer->setRubberBandPen( QPen( Qt::yellow, 0, Qt::DotLine ) );
    QwtPlot::setAxisScale( QwtPlot::xBottom, 0.0, (double) QwtPlotter::mc_nSize );
    connect( m_pZoomer, SIGNAL( ms_Rescaled() ), SLOT( mp_PlotRescaled() ) );
  }
}

void myQwtPlot::mp_PlotRescaled()
{
  QwtPlot::replot();
}
