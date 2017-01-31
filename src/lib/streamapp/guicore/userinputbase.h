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
 
#ifndef __USERINPUTBASE_H__
#define __USERINPUTBASE_H__

#include <qglobal.h>
#if QT_VERSION < 0x040000
#include <qdialog.h>
#else
#include <QDialog>
#endif

class QPushButton;
class QWidget;

namespace guicore
{
  class CustomLayout;

    /**
      * UserInputBase
      *   base class for customizable interaction dialogs.
      *   Provides 3 buttons and a CustomLayout.
      *   Example that asks user to enter a number between 0 and 10:
      *   @code
      *   class AskNumber : public UserInputBase
      *   {
      *   public:
      *     AskNumber()
      *     {
      *       setCaption( "AskNumber" );
      *       m_Label = new QLabel( "Enter a number between 0 and 10", this );
      *       m_Edit  = new LineEdit( this );
      *       m_Edit->setValidator( new QIntValidator( 0, 10, m_Edit ) );
      *       layout->mp_AddItem( m_Label, new tItemLayout( 0.0, 0.0, 1.0, 0.3 ) );
      *       layout->mp_AddItem( m_Edit, new tItemLayout( 0.4, 0.35, 0.2, 0.2 ) );
      *       m_Edit->setFocus();
      *       m_Edit->setText( qn( 0 ) );
      *     }
      *
      *     QLabel* m_Label;
      *     LineEdit* m_Edit;
      *   };
      *
      *   void f_SomeFunc()
      *   {
      *     AskNumber ask;
      *     if( ask.exec() == AskNumber::Accepted )
      *     {
      *       int number = ask.m_Edit->text().toInt();
      *       //do something interesting with the entered number
      *     }
      *   }
      *   @endcode
      ***************************************************************************** */
  class UserInputBase : public QDialog
  {
    Q_OBJECT
  public:
      /**
        * Constructor.
        * @param ac_bUseRetry true to show the retry button
        * @param a_pParent the parent widget, 0 for desktop level
        */
    UserInputBase( const bool ac_bUseRetry = false, QWidget* a_pParent = 0 );
    ~UserInputBase();

    QPushButton* buttonOk;
    QPushButton* buttonCancel;
    QPushButton* buttonRetry;
    CustomLayout* layout;

      /**
        * Disable the cancel button.
        * Positions the Ok button in the middle.
        * @param ac_bSet true for disable
        */
    void mp_SetNoCancel( const bool ac_bSet );

  public slots:
      /**
        * Called when retrybutton is clicked, exec() will return 3
        */
    void ms_Retry();

  protected:
    void resizeEvent( QResizeEvent* );
  };

}

#endif //#ifndef __USERINPUTBASE_H__
