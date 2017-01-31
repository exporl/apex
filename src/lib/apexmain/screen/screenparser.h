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

#ifndef _EXPORL_SRC_LIB_APEXMAIN_SCREEN_SCREENPARSER_H_
#define _EXPORL_SRC_LIB_APEXMAIN_SCREEN_SCREENPARSER_H_

#include "apexdata/fileprefix.h"

#include "apexmain/fileprefixconvertor.h"

#include "apextools/apextools.h"
#include "apextools/apextypedefs.h"
#include "apextools/global.h"

#include "apextools/status/errorlogger.h"

#include "apextools/xml/xercesinclude.h"

#include <QCoreApplication>
#include <QFont>
#include <QString>

class QWidget;
class QObject;

namespace XERCES_CPP_NAMESPACE
{
  class DOMElement;
};

namespace apex
{
  namespace data
  {
    class Screen;
    class ScreenElement;
    class ScreenLayoutElement;
    class ButtonGroup;
    class ScreensData;
    class ParameterManagerData;
  }

  namespace gui
  {
    using data::ScreenElement;
    using data::ScreenLayoutElement;



    /**
     * The ScreenParser class is a class that knows how to parse the
     * data about a screen in an Apex Experiment file into an instance
     * of the \ref Screen class.  It needs a pointer to a ScreensData
     * instance, and it will store newly created screens in that
     * object.
     */
    class APEX_EXPORT ScreenParser
      : public ErrorLogger
    {
        Q_DECLARE_TR_FUNCTIONS(ScreenParser)

    public:
        static QString f_CheckPath( const QString& ac_sBasePath, const QString& ac_sRelativePath );

        static QString f_CheckPath(data::FilePrefix p, const QString& ac_sRelativePath);

    public:
      /**
       * Create a new ScreenParser, that will store its created
       * screens in the given \ref ScreensData instance.
       */
      ScreenParser( data::ScreensData* screens,
                    data::ParameterManagerData* pmd=0);

      /**
       * Set the base path for relative paths.
       * @param ac_sPath the path
       */
      void mp_SetBasePath( const data::FilePrefix& ac_sPath )
      {
          m_sPath = ac_sPath;
            // FIXME: makedirend
      }


      const QString GetBasePath( ) const {
          return  FilePrefixConvertor::convert( m_sPath );
      }

      /**
       * Create a Screen.
       * @param a_pElement the DOMElement
       * @return the newly created screen ( dynamically allocated with new ), or 0 for error (call GetError() for description)
       */
        data::Screen* createScreen( XERCES_CPP_NAMESPACE::DOMElement* a_pElement );

      /**
       * Create a test Screen.
       * Creates a Screen with a GridLayout and two buttons.
       * @return the screen ( dynamically allocated with new )
       */
      data::Screen* createTestScreen();

    private:
      /**
       * Create a ScreenElement with the given parent.
       * @return a Button.
       */
      ScreenElement* createTestScreenElement( ScreenElement* parent);

      ScreenLayoutElement* createLayout(
          XERCES_CPP_NAMESPACE::DOMElement* e, ScreenElement* parent, ScreenElementMap& elements );
      ScreenElement* createElement(
        XERCES_CPP_NAMESPACE::DOMElement* e, ScreenElement* parent, ScreenElementMap& elements );
      ScreenElement* createNonLayoutElement(
        XERCES_CPP_NAMESPACE::DOMElement* e, ScreenElement* parent, ScreenElementMap& elements );

      /**
       * Struct for parsing feedback media paths
       */
      struct mt_FeedBackPaths
      {
        QString m_sPositive;
        QString m_sNegative;
        QString m_sHighLight;
        QString m_sDisabled;
      };

      /**
       * Parse mt_FeedBackPaths.
       * Must be deleted after using it.
       * @param a_pElement the DOMElement
       * @param ac_sElementID the ID of the element the paths are parsed for
       * @return the struct, or 0 when one of the paths doesn't exist
       */
      mt_FeedBackPaths* parseFeedBackPaths( XERCES_CPP_NAMESPACE::DOMElement* a_pElement, const QString& ac_sElementID );

      /**
       * parse columnstretch or rowstretch into tStretchList
       */
      data::tStretchList parseStretchList(QString d);

      /**
       * Create a ButtonGroup.
       * @param a_pElement the DOMElement
       * @param idToElementMap Used for input validation
       * @return the newly created ButtonGroup, or 0 for error (call GetError() for description)
       */
      data::ButtonGroup* createButtonGroup( XERCES_CPP_NAMESPACE::DOMElement* a_pElement, const ScreenElementMap& p_idToElementMap );

      /**
       * Shortcut for adding 'Unknown Tag' error
       * @param ac_sSource the method
       * @param ac_sTag the unknown tag
       */
      void addUnknownTag( const QString& ac_sSource, const QString& ac_sTag );

      //! Check whether the given string is a valid color
      bool checkColor (const QString& value,
                       const QString& element="") const;

    private:
      data::ScreensData* screens;
      data::ParameterManagerData* parameterManagerData;
      data::FilePrefix   m_sPath;
    };
  }
}

#endif
