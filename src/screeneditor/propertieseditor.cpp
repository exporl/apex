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

#include "propertieseditor.h"

#include "flashfilesdb.h"
#include "imagefilesdb.h"
#include "screenelementeditordelegate.h"

#include <QApplication>
#include <QComboBox>
#include <QFileDialog>
#include <QHBoxLayout>
#include <QIntValidator>
#include <QLineEdit>
#include <QSpinBox>
#include <QToolButton>

#include <assert.h>

namespace apex
{
  namespace editor
  {
    class ObjectPropertiesItemDelegate
      : public QItemDelegate
    {
      Q_OBJECT
    public:
      ObjectPropertiesItemDelegate( ObjectPropertiesModel* model, QObject* parent );
      QWidget* createEditor ( QWidget * parent, const QStyleOptionViewItem & option, const QModelIndex & index ) const;
      void setEditorData ( QWidget * editor, const QModelIndex & index ) const;
      void setModelData( QWidget * editor, QAbstractItemModel * model, const QModelIndex & index ) const;
      QSize sizeHint() const;
    public slots:
      void editingFinished();
      void valueChanged();
    private:
      ObjectPropertiesModel* model;
      bool invaluechanged;
      mutable int rowBeingEdited;
    };

    class ObjectPropertiesModel
      : public QAbstractTableModel
    {
      Q_OBJECT

      friend class ObjectPropertiesItemDelegate;
      ScreenElementEditorDelegate* rep;
    public:
      ObjectPropertiesModel( QObject* parent, ScreenElementEditorDelegate* r = 0 );

      void setEditorDelegate( ScreenElementEditorDelegate* nrep );
      int rowCount( const QModelIndex& parent ) const;
      int columnCount( const QModelIndex& parent ) const;
      QVariant data( const QModelIndex& index, int role = Qt::DisplayRole ) const;
      QVariant headerData( int section, Qt::Orientation orientation, int role = Qt::DisplayRole ) const;
      Qt::ItemFlags flags(const QModelIndex &index) const;
      bool setData(const QModelIndex &index, const QVariant &value,
                   int role = Qt::EditRole);
    signals:
      void modified();
    };

    class MediaSelector
      : public QWidget
    {
      Q_OBJECT
    protected:
      QComboBox* cb;
    public:
      MediaSelector( QWidget* parent );

      QString getValue() const;
      bool selectFile( const QString& s );
    private slots:
      void showDialog();
    signals:
      void valueChanged();
    private:
      virtual QString getOpenFileName() = 0;
      virtual void addMediumToDB( const QString& s ) = 0;
    };

    MediaSelector::MediaSelector( QWidget* parent )
      : QWidget( parent )
    {
      QHBoxLayout* l = new QHBoxLayout();
      l->setMargin( 0 );

      cb = new QComboBox();
      cb->setFrame( false );
      cb->setEditable(false);
      l->addWidget( cb );

      QToolButton* button = new QToolButton();
      button->setIcon(QIconSet( QLatin1String("fileopen") ) );
      button->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::MinimumExpanding);
      button->setFixedWidth(20);
      l->addWidget(button);

      connect(button, SIGNAL(clicked()), this, SLOT(showDialog()));
      connect(cb, SIGNAL(activated(int)), this, SIGNAL(valueChanged()));

      setLayout( l );
    }

    QString MediaSelector::getValue() const
    {
      int index = cb->currentIndex();
      if ( index == -1 ) return QString();
      QString s = cb->itemData( index ).toString();
      return s;
    }

    void MediaSelector::showDialog()
    {
      QString s = getOpenFileName();
      if ( s.isEmpty() ) return;
      else {
        if ( selectFile( s ) )
          emit valueChanged();
      }
    }

    bool MediaSelector::selectFile( const QString& s )
    {
      int i = cb->findData( s );
      if ( i == -1 )
      {
        QFileInfo fi( s );
        if ( fi.isFile() )
        {
          addMediumToDB( s );
          cb->addItem( s, s );
          cb->setCurrentIndex( cb->count() - 1 );
          return true;
        }
        else
        {
          return false;
        }
      }
      else
      {
        cb->setCurrentIndex( i );
        return true;
      }
    }

    class FlashFileSelector
      : public MediaSelector
    {
      Q_OBJECT
    public:
      FlashFileSelector( QWidget* parent );
      void populateCombo();
      QString getOpenFileName();
      void addMediumToDB( const QString& s );
    };

    void FlashFileSelector::addMediumToDB( const QString& s )
    {
      FlashFilesDB& fdb = FlashFilesDB::instance();
      fdb.files.push_back( s );
    };

    QString FlashFileSelector::getOpenFileName()
    {
      return QFileDialog::getOpenFileName(
        this, tr( "Select Flash File" ),
        QString(), "Flash files (*.swf)" );
    }

    FlashFileSelector::FlashFileSelector( QWidget* parent )
      : MediaSelector( parent )
    {
      populateCombo();
    }

    void FlashFileSelector::populateCombo()
    {
      cb->addItem( tr( "Not Set" ), QString() );
      FlashFilesDB& fdb = FlashFilesDB::instance();
      for ( unsigned i = 0; i < fdb.files.size(); ++i )
        cb->addItem( fdb.files[i], fdb.files[i] );
    }

    class ImageFileSelector
      : public MediaSelector
    {
      Q_OBJECT
    public:
      ImageFileSelector( QWidget* parent );
      void populateCombo();
      QString getOpenFileName();
      void addMediumToDB( const QString& s );
    };

    void ImageFileSelector::addMediumToDB( const QString& s )
    {
      ImageFilesDB& fdb = ImageFilesDB::instance();
      fdb.files.push_back( s );
    };

    QString ImageFileSelector::getOpenFileName()
    {
      return QFileDialog::getOpenFileName(
        this, tr( "Select Image File" ),
        QString(), "Image files (*.jpg *.jpeg *.png *.gif)" );
    }

    ImageFileSelector::ImageFileSelector( QWidget* parent )
      : MediaSelector( parent )
    {
      populateCombo();
    }

    void ImageFileSelector::populateCombo()
    {
      cb->addItem( tr( "Not Set" ), QString() );
      ImageFilesDB& fdb = ImageFilesDB::instance();
      for ( unsigned i = 0; i < fdb.files.size(); ++i )
        cb->addItem( fdb.files[i], fdb.files[i] );
    }

    void ObjectPropertiesItemDelegate::valueChanged()
    {
      invaluechanged = true;
      assert( dynamic_cast<QWidget*>( sender() ) );
      QWidget* editor = static_cast<QWidget*>( sender() );
      emit commitData( editor );
      setEditorData( editor, model->index( rowBeingEdited, 1 ) );
      editor->setFocus();
      invaluechanged = false;
    }

    void ObjectPropertiesItemDelegate::editingFinished()
    {
      // this is an obscure fix for a strange bug: it happens if for
      // example you change the qspinbox value and valueChanged is
      // called.  Further down someone opens a modal window, which calls
      // the qt event loop.  The qt event loop notices the editor has
      // lost focus, makes it call editingFinished(), so we end up here,
      // and closeEditor() calls deleteLater() on it, and then the event
      // loop executes the deleteLater(), so the widget is deleted.  now
      // when we then return from the valueChanged method everything
      // crashes because the editor has been deleted already..
      //
      if ( invaluechanged == true )
        return;
      assert( dynamic_cast<QWidget*>( sender() ) );
      QWidget* editor = static_cast<QWidget*>( sender() );
      emit closeEditor( editor );
    }

    void ObjectPropertiesItemDelegate::setEditorData(
      QWidget* editor, const QModelIndex& index ) const
    {
      if ( index.parent().isValid() )
        return;
      if ( index.column() != 1 )
        return;
      int row = index.row();

      const QAbstractItemModel* m = index.model();
      const ObjectPropertiesModel* model = dynamic_cast<const ObjectPropertiesModel*>( m );
      assert( model );

      ScreenElementEditorDelegate* rep = model->rep;
      int type = rep->getPropertyType( row );
      QVariant v = rep->getPropertyData( row, Qt::DisplayRole );
      QLineEdit* e = 0;
      QSpinBox* sb = 0;
      QComboBox* cb = 0;
      FlashFileSelector* ffs = 0;
      ImageFileSelector* ifs = 0;
      int val = 0;
      switch( type )
      {
      case FontSizePropertyType:
      case IntPropertyType:
      case PositiveIntPropertyType:
        val = v.toInt();
        assert( dynamic_cast<QSpinBox*>( editor ) );
        sb = static_cast<QSpinBox*>( editor );
        sb->setValue( val );
        return;
      case Double01PropertyType:
      case StringPropertyType:
      case InputMaskPropertyType:
        e = dynamic_cast<QLineEdit*>( editor );
        assert( e );
        e->setText( v.toString() );
        return;
      case ShortcutPropertyType:
        e = dynamic_cast<QLineEdit*>( editor );
        assert( e );
        e->setText( v.value<QKeySequence>().toString() );
        return;
      case ArcLayoutTypePropertyType:
      case TwoPartLayoutDirectionPropertyType:
        cb = dynamic_cast<QComboBox*>( editor );
        assert( cb );
        for ( int i = 0; i < cb->count(); ++i )
        {
          if ( cb->itemText( i ) == v.toString() )
          {
            cb->setCurrentIndex( i );
          }
        };
        return;
      case BooleanPropertyType:
        assert( v.type() == QVariant::Bool );
        cb = dynamic_cast<QComboBox*>( editor );
        assert( cb );
        for ( int i = 0; i < cb->count(); ++i )
        {
          if ( cb->itemText( i ) == v.toString() )
          {
            cb->setCurrentIndex( i );
          }
        }
        return;
      case FlashPropertyType:
        assert( v.type() == QVariant::String );
        ffs = dynamic_cast<FlashFileSelector*>( editor );
        ffs->selectFile( v.toString() );
        return;
      case ImagePropertyType:
        assert( v.type() == QVariant::String );
        ifs = dynamic_cast<ImageFileSelector*>( editor );
        ifs->selectFile( v.toString() );
        return;
      default:
        return;
      }
    }

    void ObjectPropertiesItemDelegate::setModelData(
      QWidget * editor, QAbstractItemModel * m, const QModelIndex & index ) const
    {
      if ( index.parent().isValid() )
        return;
      if ( index.column() != 1 )
        return;
      int row = index.row();

      assert( dynamic_cast<ObjectPropertiesModel*>( m ) );
      ObjectPropertiesModel* model = static_cast<ObjectPropertiesModel*>( m );

      ScreenElementEditorDelegate* rep = model->rep;
      int type = rep->getPropertyType( row );

      // not allowed to declare vars in a switch...
      QLineEdit* e = 0;
      QSpinBox* sb = 0;
      QComboBox* cb = 0;
      FlashFileSelector* ffs = 0;
      ImageFileSelector* ifs = 0;
      int val;
      QString text;
      QKeySequence seq;
      bool b;
      double dval;

      switch( type )
      {
      case FontSizePropertyType:
      case IntPropertyType:
      case PositiveIntPropertyType:
        assert( dynamic_cast<QSpinBox*>( editor ) );
        sb = static_cast<QSpinBox*>( editor );
        assert( sb );
        val = sb->value();
        rep->setProperty( row, val );
        break;
      case Double01PropertyType:
        e = dynamic_cast<QLineEdit*>( editor );
        assert( e );
        text = e->text();
        bool ok;
        dval = text.toDouble( &ok );
        if ( !ok ) break;
        rep->setProperty( row, dval );
        break;
      case StringPropertyType:
      case InputMaskPropertyType:
        e = dynamic_cast<QLineEdit*>( editor );
        assert( e );
        text = e->text();
        rep->setProperty( row, text );
        break;
      case ShortcutPropertyType:
        e = dynamic_cast<QLineEdit*>( editor );
        assert( e );
        text = e->text();
        seq = QKeySequence( text );
        rep->setProperty( row, qVariantFromValue( seq ) );
        break;
      case ArcLayoutTypePropertyType:
      case TwoPartLayoutDirectionPropertyType:
        cb = dynamic_cast<QComboBox*>( editor );
        assert( cb );
        text = cb->currentText();
        rep->setProperty( row, text );
        break;
      case BooleanPropertyType:
        cb = dynamic_cast<QComboBox*>( editor );
        assert( cb );
        text = cb->currentText();
        b = QVariant( text ).toBool();
        rep->setProperty( row, b );
        break;
      case FlashPropertyType:
        ffs = dynamic_cast<FlashFileSelector*>( editor );
        assert( ffs );
        text = ffs->getValue();
        qDebug( "setting flash prop text: %s", qPrintable(text));
        rep->setProperty( row, text );
        break;
      case ImagePropertyType:
        ifs = dynamic_cast<ImageFileSelector*>( editor );
        assert( ifs );
        text = ifs->getValue();
        qDebug( "setting image prop text: %s", qPrintable(text));
        rep->setProperty( row, text );
        break;
      default:
        break;
      }
      emit model->dataChanged( index, index );
      emit model->modified();
    }

    QWidget* ObjectPropertiesItemDelegate::createEditor(
      QWidget* parent,
      const QStyleOptionViewItem& /*option*/,
      const QModelIndex& index ) const
    {
      if ( index.parent().isValid() )
        return 0;
      if ( index.column() != 1 )
        return 0;
      int row = index.row();
      rowBeingEdited = row;

      const QAbstractItemModel* m = index.model();
      const ObjectPropertiesModel* model = dynamic_cast<const ObjectPropertiesModel*>( m );
      assert( model );

      ScreenElementEditorDelegate* rep = model->rep;
      int type = rep->getPropertyType( row );
      QLineEdit* lineedit;
      QSpinBox* spinbox;
      QComboBox* comboBox;
      FlashFileSelector* flashFileSelector;
      ImageFileSelector* ifs;
      switch( type )
      {
      case FontSizePropertyType:
        spinbox = new QSpinBox( parent );
        spinbox->setMinimum( -1 );
        spinbox->setMaximum( 200 );
        spinbox->setFrame( false );
        connect( spinbox, SIGNAL( editingFinished () ),
                 this, SLOT( editingFinished() ) );
        connect( spinbox, SIGNAL( valueChanged( int ) ),
                 this, SLOT( valueChanged() ) );
        return spinbox;
      case IntPropertyType:
        spinbox = new QSpinBox( parent );
        spinbox->setMinimum( -1000 );
        spinbox->setMaximum( 1000 );
        spinbox->setFrame( false );
        connect( spinbox, SIGNAL( editingFinished () ),
                 this, SLOT( editingFinished() ) );
        connect( spinbox, SIGNAL( valueChanged( int ) ),
                 this, SLOT( valueChanged() ) );
        return spinbox;
      case PositiveIntPropertyType:
        spinbox = new QSpinBox( parent );
        spinbox->setMinimum( 0 );
        spinbox->setMaximum( 1000 );
        spinbox->setFrame( false );
        connect( spinbox, SIGNAL( editingFinished () ),
                 this, SLOT( editingFinished() ) );
        connect( spinbox, SIGNAL( valueChanged( int ) ),
                 this, SLOT( valueChanged() ) );
        return spinbox;
      case Double01PropertyType:
        lineedit = new QLineEdit( parent );
        lineedit->setValidator( new QDoubleValidator( 0., 1., 2, lineedit ) );
        connect( lineedit, SIGNAL( editingFinished () ),
                 this, SLOT( editingFinished() ) );
        return lineedit;
      case StringPropertyType:
        lineedit = new QLineEdit( parent );
        connect( lineedit, SIGNAL( editingFinished () ),
                 this, SLOT( editingFinished() ) );
        connect( lineedit, SIGNAL( textEdited( const QString& ) ),
                 this, SLOT( valueChanged() ) );
        return lineedit;
      case InputMaskPropertyType:
        lineedit = new QLineEdit( parent );
        connect( lineedit, SIGNAL( editingFinished () ),
                 this, SLOT( editingFinished() ) );
        return lineedit;
      case ShortcutPropertyType:
        lineedit = new QLineEdit( parent );
        connect( lineedit, SIGNAL( editingFinished () ),
                 this, SLOT( editingFinished() ) );
        connect( lineedit, SIGNAL( textEdited( const QString& ) ),
                 this, SLOT( valueChanged() ) );
        return lineedit;
      case ArcLayoutTypePropertyType:
        comboBox = new QComboBox( parent );
        comboBox->addItem( "Upper" );
        comboBox->addItem( "Lower" );
        comboBox->addItem( "Left" );
        comboBox->addItem( "Right" );
        comboBox->addItem( "Full" );
        comboBox->setFrame( false );
        comboBox->setInsertPolicy( QComboBox::NoInsert );
        connect( comboBox, SIGNAL( activated( int ) ),
                 this, SLOT( valueChanged() ) );
        return comboBox;
      case TwoPartLayoutDirectionPropertyType:
        comboBox = new QComboBox( parent );
        comboBox->addItem( "Horizontal" );
        comboBox->addItem( "Vertical" );
        comboBox->setFrame( false );
        comboBox->setInsertPolicy( QComboBox::NoInsert );
        connect( comboBox, SIGNAL( activated( int ) ),
                 this, SLOT( valueChanged() ) );
        return comboBox;
      case BooleanPropertyType:
        comboBox = new QComboBox( parent );
        comboBox->addItem( QVariant( true ).toString() );
        comboBox->addItem( QVariant( false ).toString() );
        comboBox->setFrame( false );
        comboBox->setInsertPolicy( QComboBox::NoInsert );
        connect( comboBox, SIGNAL( activated( int ) ),
                 this, SLOT( valueChanged() ) );
        return comboBox;
      case FlashPropertyType:
        flashFileSelector = new FlashFileSelector( parent );
        connect( flashFileSelector, SIGNAL( valueChanged() ),
                 this, SLOT( valueChanged() ) );
        return flashFileSelector;
      case ImagePropertyType:
        ifs = new ImageFileSelector( parent );
        connect( ifs, SIGNAL( valueChanged() ),
                 this, SLOT( valueChanged() ) );
        return ifs;
      default:
        return 0;
      }
    }

    Qt::ItemFlags ObjectPropertiesModel::flags( const QModelIndex& index ) const
    {
      if (!index.isValid())
        return Qt::ItemIsEnabled;

      if ( index.column() == 1 )
        return QAbstractTableModel::flags(index) | Qt::ItemIsEditable;
      else return QAbstractTableModel::flags( index );
    }

    bool ObjectPropertiesModel::setData( const QModelIndex& index, const QVariant& v,
                                         int role )
    {
      if ( index.isValid() && role == Qt::EditRole )
      {
        bool t = rep->setProperty( index.row(), v );
        if ( t )
        {
          emit dataChanged( index, index );
          emit modified();
          return true;
        }
        return false;
      }
      return false;
    }

    ObjectPropertiesModel::ObjectPropertiesModel( QObject* parent, ScreenElementEditorDelegate* r )
      : QAbstractTableModel( parent ), rep( r )
    {
    }

    int ObjectPropertiesModel::rowCount( const QModelIndex& ) const
    {
      if ( rep )
      {
        return rep->getPropertyCount();
      }
      else
      {
        return 1;
      }
    }

    int ObjectPropertiesModel::columnCount( const QModelIndex& ) const
    {
      return 2;
    }

    QVariant ObjectPropertiesModel::data(
      const QModelIndex& index, int role ) const
    {
      if ( rep )
      {
        int row = index.row();
        int column = index.column();
        switch( column )
        {
        case 0:
          if ( role == Qt::DisplayRole )
            return rep->getPropertyName( row );
          else return QVariant();
        case 1:
          return rep->getPropertyData( row, role );
        default:
          return QVariant();
        };
      } else {
        switch( role )
        {
        case Qt::DisplayRole: return tr( "<No object selected>" );
        case Qt::TextColorRole:
          return QApplication::palette().color( QPalette::Disabled, QPalette::WindowText );
        default:
          return QVariant();
        };
      }
      return QVariant();
    }

    QVariant ObjectPropertiesModel::headerData(
      int section, Qt::Orientation orientation,
      int role ) const
    {
      if ( orientation == Qt::Horizontal )
      {
        if ( role == Qt::DisplayRole )
          switch( section )
          {
          case 0: return tr( "Name" );
          case 1: return tr( "Value" );
          default: return QVariant();
          }
        else return QVariant();
      }
      else return QVariant();
    }

    PropertiesEditor::PropertiesEditor( QWidget* parent )
      : QTreeView( parent ),
        selectedRep( 0 ),
        model( new ObjectPropertiesModel( this ) )
    {
      connect( model, SIGNAL( modified() ), this, SIGNAL( modified() ) );
      setModel( model );
      setItemDelegate( new ObjectPropertiesItemDelegate( model, this ) );
      setItemsExpandable( false );
      setRootIsDecorated( false );
      setEditTriggers( QAbstractItemView::AllEditTriggers );
      setSizePolicy( QSizePolicy( QSizePolicy::Preferred, QSizePolicy::Expanding ) );
      setMinimumSize( 150, 100 );
      setMaximumWidth( 250 );
    }

    PropertiesEditor::~PropertiesEditor()
    {
    }

    void PropertiesEditor::selectWidget( ScreenElementEditorDelegate* rep )
    {
      selectedRep = rep;
      model->setEditorDelegate( rep );
    }

    void ObjectPropertiesModel::setEditorDelegate( ScreenElementEditorDelegate* nrep )
    {
      rep = nrep;
      reset();
    }

    ObjectPropertiesItemDelegate::ObjectPropertiesItemDelegate( ObjectPropertiesModel* m, QObject* parent )
      : QItemDelegate( parent ),
        model( m ), invaluechanged( false )
    {
    }

    QSize ObjectPropertiesItemDelegate::sizeHint() const
    {
      return QSize( 180, 200 );
    }

  }
}
#include "propertieseditor.moc"
