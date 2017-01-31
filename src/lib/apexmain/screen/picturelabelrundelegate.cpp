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

#include "apexdata/screen/picturelabelelement.h"

#include "apextools/apextools.h"

#include "gui/guidefines.h"

#include "fileprefixconvertor.h"
#include "picturelabelrundelegate.h"

#include <QPixmap>

#include <iostream>

namespace apex
{
namespace rundelegates
{

PictureLabelRunDelegate::PictureLabelRunDelegate(
        ExperimentRunDelegate* p_exprd,
    QWidget* parent, const PictureLabelElement* e ) :
        QLabel( parent ),
        ScreenElementRunDelegate(p_exprd, e),
        element( e ),
        pixmap( 0 ),
        pixmapDisabled( 0 )
{
    setAlignment (Qt::AlignCenter);

  pixmap = new QPixmap(ApexTools::addPrefix(element->getPicture(),
                       FilePrefixConvertor::convert(element->prefix())));
  if ( pixmap->isNull() )
      Q_ASSERT( element->getPicture().isEmpty() );
  else
      QLabel::setPixmap( *pixmap );


  if (!element->getPictureDisabled().isEmpty()) {
      pixmapDisabled =
              new QPixmap(ApexTools::addPrefix(element->getPictureDisabled(),
                    FilePrefixConvertor::convert(element->prefix())));
  }

}

PictureLabelRunDelegate::~PictureLabelRunDelegate()
{
    delete pixmap;
}

const ScreenElement* PictureLabelRunDelegate::getScreenElement() const
{
    return element;
}

}
}

QWidget* apex::rundelegates::PictureLabelRunDelegate::getWidget()
{
    return this;
}

void apex::rundelegates::PictureLabelRunDelegate::setEnabled( const bool enabled)
{
    if ( pixmapDisabled ) {
        if (enabled) {
            QLabel::setPixmap( *pixmap );
        } else {
            QLabel::setPixmap( *pixmapDisabled );
        }
    }
}

