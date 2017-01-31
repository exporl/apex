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
 
#include "parameterlisteditordelegate.h"

#include "screenwidget.h"
#include "functions.h"

#include "screen/parameterlistelement.h"
#include "screen/parameterdata.h"
#include "screen/screen.h"

#include "guicore/widgetattributes.h"

#include <QLabel>
#include <QVBoxLayout>
#include <QPushButton>
#include <QHBoxLayout>
#include <QTableView>
#include <QHeaderView>
#include <QMessageBox>

namespace apex
{
  namespace editor
  {
    class ParameterListModel
      : public QAbstractTableModel
    {
      Q_OBJECT

      ParameterListElement* element;
      ParameterListEditorDelegate* delegate;
    public:
      ParameterListModel( ParameterListEditorDelegate* del, ParameterListElement* el );
      int rowCount ( const QModelIndex & parent ) const;
      int columnCount ( const QModelIndex & parent ) const;
      QVariant data ( const QModelIndex & index, int role ) const;
      QVariant headerData ( int section, Qt::Orientation orientation, int role ) const;
      void reset();
      Qt::ItemFlags flags( const QModelIndex& index ) const;
      bool setData(const QModelIndex &index, const QVariant &value,
                   int role);
    };

    ParameterListEditorDelegate::ParameterListEditorDelegate(
      ParameterListElement* e, QWidget* parent, ScreenWidget* w )
      : QFrame( parent ),
        ScreenElementEditorDelegate( w ),
        table( 0 ),
        element( e ),
        model( 0 )
    {
      QVBoxLayout* layout = new QVBoxLayout( this );

      QLabel* label = new QLabel( tr( "Parameter List" ), this );
      layout->addWidget( label );
//      guicore::widgetattributes::gf_SetForeGroundColor( label, Qt::white );

      table = new QTableView( this );
      model = new ParameterListModel( this, element );
      table->setModel( model );
      layout->addWidget( table );
      table->verticalHeader()->hide();
      table->horizontalHeader()->resizeSection( 0, 85 );
      table->horizontalHeader()->resizeSection( 1, 100 );
      table->horizontalHeader()->resizeSection( 2, 150 );

      QHBoxLayout* buttonlayout = new QHBoxLayout();
      QPushButton* addButton = new QPushButton( "Add..", this );
      buttonlayout->addWidget( addButton );
      connect( addButton, SIGNAL( clicked() ),
               this, SLOT( addButtonClicked() ) );
      QPushButton* delButton = new QPushButton( "Remove..", this );
      buttonlayout->addWidget( delButton );
      connect( delButton, SIGNAL( clicked() ),
               this, SLOT( removeButtonClicked() ) );

      layout->addLayout( buttonlayout );
      setFrameShape( QFrame::Box );
    }

    ParameterListEditorDelegate::~ParameterListEditorDelegate()
    {
      screenWidget->delegateDeleted( this );
    }

    QFrame* ParameterListEditorDelegate::getWidget()
    {
      return this;
    }

    ScreenElement* ParameterListEditorDelegate::getScreenElement()
    {
      return element;
    }

    int ParameterListEditorDelegate::getPropertyCount()
    {
      return ScreenElementEditorDelegate::getPropertyCount();
    }

    QString ParameterListEditorDelegate::getPropertyName( int nr )
    {
      return ScreenElementEditorDelegate::getPropertyName( nr);
    }

    QVariant ParameterListEditorDelegate::getPropertyData( int nr, int role )
    {
      return ScreenElementEditorDelegate::getPropertyData( nr, role );
    }

    PropertyType ParameterListEditorDelegate::getPropertyType( int nr )
    {
      return ScreenElementEditorDelegate::getPropertyType( nr );
    }

    bool ParameterListEditorDelegate::setProperty( int nr, const QVariant& v )
    {
      return ScreenElementEditorDelegate::setProperty( nr, v );
    }

    void ParameterListEditorDelegate::mouseReleaseEvent( QMouseEvent* e )
    {
      handleMouseReleaseEvent( e, this );
    }

    int ParameterListModel::rowCount( const QModelIndex & parent ) const
    {
      if ( parent.isValid() ) return 0;
      else return element->getParameterList().size();
    }

    int ParameterListModel::columnCount( const QModelIndex & parent ) const
    {
      if ( parent.isValid() ) return 0;
      else return 3;
    }

    QVariant ParameterListModel::data( const QModelIndex & index, int role ) const
    {
      if ( index.parent().isValid() ) return QVariant();
      int row = index.row();
      int col = index.column();
      data::ParameterData d = element->getParameterList()[row];
      if ( role == Qt::DisplayRole || role == Qt::EditRole )
      {
        switch( col )
        {
        case 0: return d.id;
        case 1: return d.name;
        case 2: return d.expression;
        }
      }
      return QVariant();
    }

    QVariant ParameterListModel::headerData( int section, Qt::Orientation orientation, int role ) const
    {
      if ( orientation != Qt::Horizontal ) return QVariant();
      if ( role == Qt::DisplayRole )
      {
        switch( section )
        {
        case 0: return tr( "id" );
        case 1: return tr( "name" );
        case 2: return tr( "expression" );
        }
      }
      return QVariant();
    }

    ParameterListModel::ParameterListModel(
      ParameterListEditorDelegate* del, ParameterListElement* el )
      : QAbstractTableModel( del ), element( el ), delegate( del )
    {
    }

    void ParameterListEditorDelegate::removeButtonClicked()
    {
      QModelIndexList l = table->selectionModel()->selectedIndexes();
      if ( l.size() > 1 )
      {
        QMessageBox::warning( this, tr( "Select a single parameter to remove" ),
                              tr( "Please select only one parameter at a time." ),
                              QMessageBox::Ok, QMessageBox::NoButton, QMessageBox::NoButton );
        return;
      } else if ( l.size() == 0 ) {
        QMessageBox::warning( this, tr( "Select a parameter to remove" ),
                              tr( "Please select a parameter to be removed." ),
                              QMessageBox::Ok, QMessageBox::NoButton, QMessageBox::NoButton );
        return;
      } else {
        QModelIndex index = l[0];
        int row = index.row();
        element->removeParameter( row );
        model->reset();
      }
    }

    void ParameterListEditorDelegate::addButtonClicked()
    {
      QString parameterID;
      int i = 1;
      while ( true )
      {
        parameterID = "parameter" + QString::number( i++ );
        if ( !element->parameterIDTaken( parameterID ) )
          break;
      }
      element->addParameter(
        parameterID,
        "<No name set>" );
      model->reset();
    }

    void ParameterListModel::reset()
    {
      QAbstractTableModel::reset();
    }

    Qt::ItemFlags ParameterListModel::flags( const QModelIndex& index ) const
    {
      return QAbstractTableModel::flags( index ) | Qt::ItemIsEditable;
    }

    bool ParameterListModel::setData(const QModelIndex &index, const QVariant &value,
                                     int role)
    {
      if ( role != Qt::EditRole ) return false;
      if ( index.parent().isValid() ) return false;
      if ( value.type() != QVariant::String ) return false;
      QString s = value.toString();
      int row = index.row();
      int col = index.column();
      switch( col )
      {
      case 0: // id
        if ( element->setParameterID( row, s ) ) {
          reset();
          return true;
        }
      case 1: // name
        element->setParameterName( row, s );
        reset();
        return true;
      case 2: // expr
        if ( element->setParameterExpression( row, s ) ) {
          reset();
          return true;
        }
      }
      return false;
    }

  }
}

#include "parameterlisteditordelegate.moc"
