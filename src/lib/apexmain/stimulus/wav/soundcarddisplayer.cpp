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
 
#include "soundcarddisplayer.h"
#include "soundcarddrivers.h"
#include "soundcard/soundcardfactory.h"

#include <iostream>
#include <QString>

using namespace streamapp;

namespace apex {
  namespace stimulus {

    void sf_ShowCardsForDriver( const gt_eDeviceType ac_tType )
    {
      std::string sErr;
      tStringVector saCards( SoundCardFactory::GetDriverNames( ac_tType, sErr ) );
      const tStringVector::size_type nCards = saCards.size();
      if( nCards )
      {
        std::cout << fSndEnumToString< std::string >( ac_tType ) << " cards: " << std::endl;
        for( tStringVector::size_type i = 0 ; i < saCards.size() ; ++i )
        {
          QString temp( saCards.at( i ).c_str() );
          std::cout << temp.toLocal8Bit().constData() << std::endl;
        }
      }
    }

    void SoundCardDisplayer::ShowSoundCards()
    {
        //there's no need for #ifdefs here, that's
        //done in SoundCardFacotry already
      sf_ShowCardsForDriver( ASIO );
      sf_ShowCardsForDriver( PORTAUDIO );
      sf_ShowCardsForDriver( JACK );
      sf_ShowCardsForDriver( COREAUDIO );
    }

  }
}
