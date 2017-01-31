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
 
#include "qwtpartialcurve.h"

using namespace guicore;

QwtPartialCurve::QwtPartialCurve( const unsigned ac_nSamples ) :
    QwtPlotCurve(),
  m_Partial( ac_nSamples, true )
{
}

QwtPartialCurve::QwtPartialCurve( const QwtText& title, const unsigned ac_nSamples   ) :
    QwtPlotCurve( title ),
  m_Partial( ac_nSamples, true )
{
}

QwtPartialCurve::QwtPartialCurve( const QString& title, const unsigned ac_nSamples   ) :
    QwtPlotCurve( title ),
  m_Partial( ac_nSamples, true )
{
}

QwtPartialCurve::~QwtPartialCurve()
{
}

void QwtPartialCurve::mp_SetPartialArray( const mt_DrawdSamples& ac_Array )
{
  m_Partial.mp_CopyFrom( ac_Array );
}

void QwtPartialCurve::drawCurve( QPainter* painter, int style, const QwtScaleMap &xMap, const QwtScaleMap &yMap, int from, int to ) const
{
  for( int i = from ; i < to ; ++i )
  {
    if( m_Partial( i ) )
      QwtPlotCurve::drawCurve( painter, style, xMap, yMap, i, i );
  }
}

void QwtPartialCurve::drawSymbols( QPainter* painter, const QwtSymbol& sym, const QwtScaleMap &xMap, const QwtScaleMap &yMap, int from, int to ) const
{
  for( int i = from ; i < to ; ++i )
  {
    if( m_Partial( i ) )
      QwtPlotCurve::drawSymbols( painter, sym, xMap, yMap, i, i );
  }
}
