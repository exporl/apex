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
 
#include "statuswindow.h"
#include "statusitem.h"
#include "statusreporter.h"

#include <QLayout>
#include <QAbstractListModel>
#include <QPainter>
#include <QTreeView>
#include <QDateTime>
#include <QHeaderView>
#include <QAction>
#include <QApplication>         // clipboard
#include <QClipboard>
#include <QShortcut>
#include <QMenu>
#include <QCloseEvent>

#include <iostream>

using namespace apex;

//ApexMessageWindowItem
static const QColor sf_SetColor( const StatusItem::Level a_Type )
{
    switch ( a_Type )
    {
        case StatusItem::ERROR:
            return Qt::red;
        case StatusItem::WARNING:
            return QColor( 0, 0x88, 0 ); //less flashy than Qt::green
        case StatusItem::MESSAGE:
            return Qt::gray;
        default:
            return Qt::black;
    }
}


class StatusWindow::Model
            : public QAbstractTableModel
{
    struct Error;
    QList<Error> errors;
protected:
    int rowCount( const QModelIndex& parent = QModelIndex() ) const;
    QVariant data ( const QModelIndex & index, int role = Qt::DisplayRole ) const;
    int columnCount( const QModelIndex & parent = QModelIndex() ) const;
    QVariant headerData( int section, Qt::Orientation orientation, int role = Qt::DisplayRole ) const;

public:
    void addError( const StatusItem* error );
    Error getError(int index) const;
    int getNumErrors() const;
    void clear();
    QModelIndex GetLastItemIndex() const
    {
        return index( errors.size() - 1, 0 );
    }
};

struct StatusWindow::Model::Error
{
    QString time;
    QString errorSource;
    QString errorMessage;
    QColor color;
    Error( const QString& t,
           const QString& s,
           const QString& m,
           const QColor& c )
            : time( t ), errorSource( s ), errorMessage( m ), color( c )
    {
    };
    Error( const Error& e )
            : time( e.time ), errorSource( e.errorSource ),
            errorMessage( e.errorMessage ),
            color( e.color )
    {
    };
    Error()
    {
    };
    ~Error()
    {
    };
    QString toString() {
        return time + "\t" + errorSource + "\t" + errorMessage;
    }
};

void StatusWindow::Model::addError( const StatusItem* error )
{
    beginInsertRows( QModelIndex(), rowCount(), rowCount() );
    errors.push_back(
        Error(
            error->dateTime().time().toString(),
            error->source(),
            error->message(),
            sf_SetColor( error->level() ) ) );
    endInsertRows();
};

StatusWindow::Model::Error StatusWindow::Model::getError(int index) const {
    return errors.at(index);
}

int StatusWindow::Model::getNumErrors() const {
    return errors.size();
}

void StatusWindow::Model::clear() {
    return errors.clear();
}


int StatusWindow::Model::rowCount( const QModelIndex& /*parent*/ ) const
{
//    Q_ASSERT( !parent.isValid() );
    return errors.size();
};

int StatusWindow::Model::columnCount( const QModelIndex& /*parent*/ ) const
{
//    Q_ASSERT( !parent.isValid() );
    return 3;
};

QVariant StatusWindow::Model::headerData( int section, Qt::Orientation orientation, int role ) const
{
    if ( role == Qt::DisplayRole )
    {
        if ( orientation == Qt::Horizontal )
            switch ( section ) {
                case 0: return tr( "Time" );
                case 1: return tr( "Source" );
                case 2: return tr( "Message" );
                default: return QVariant();
            }
        else return QVariant();
    } else return QVariant();
};

QVariant StatusWindow::Model::data( const QModelIndex& index, int role ) const
{
    int row = index.row();
    if ( row < 0 )
        return QVariant();
    
    if ( row >= rowCount() )
        return QVariant();

    if ( role == Qt::DisplayRole ) {
        switch ( index.column() ) {
            case 0: return errors[row].time;
            case 1: return errors[row].errorSource;
            case 2: return errors[row].errorMessage;
            default: return QVariant();
        }
    }
    
    if ( role == Qt::TextColorRole ) {
        return errors[row].color;
    }

    if (role == Qt::ToolTipRole) {
        return     errors[row].errorMessage;
    }

    if (role == Qt::TextAlignmentRole) {
        return Qt::AlignTop;
    }
    
    return QVariant();
};

//ApexMessageWindow
StatusWindow::StatusWindow(StatusItem::Levels reportLevels) :
        QFrame( 0 ),
        m_Model( new Model() ),
        m_List( new QTreeView( this ) ),
        m_Layout( new QHBoxLayout( this/*, 11, 6, "ApexMessageWindowLayout"*/ ) )
{
    m_List->setModel( m_Model );
    m_List->setUniformRowHeights( false );
    m_List->setItemsExpandable( false );
    m_List->setIndentation( 0 );
    m_List->setRootIsDecorated( false );
    m_List->setSelectionMode(QAbstractItemView::ExtendedSelection);
    m_List->header()->resizeSection( 0, 70 );
    m_List->header()->resizeSection( 1, 250 );
    m_List->header()->resizeSection( 2, 470 );

    // context menu
    QAction* copyAction = new QAction(tr("Copy all messages to clipboard"), m_List);
    m_List->addAction( copyAction );
    connect(copyAction, SIGNAL(triggered()), this, SLOT(copyAllToClipboard()));
    QShortcut* shortcut = new QShortcut(QKeySequence("Ctrl+C"), m_List);
    connect(shortcut, SIGNAL( activated()), this, SLOT(copyAllToClipboard()));

    QAction* copyThisAction = new QAction(tr("Copy selected messages to clipboard"), m_List);
    m_List->addAction( copyThisAction );
    connect(copyThisAction, SIGNAL(triggered()), this, SLOT(copySelectedToClipboard()));

    QAction* clearAction = new QAction(tr("Clear message window"), m_List);
    m_List->addAction( clearAction );
    connect(clearAction, SIGNAL(triggered()), this, SLOT(clearList()));

    QAction* levelsAction = new QAction(tr("Report levels"), m_List);
    m_List->addAction(levelsAction);
    QMenu* levelsMenu = new QMenu("bla", m_List);
    levelsAction->setMenu(levelsMenu);

    errorsAction = new QAction(tr("Errors"), m_List);
    errorsAction->setCheckable(true);
    errorsAction->setChecked(reportLevels & StatusItem::ERROR);
    levelsMenu->addAction(errorsAction);
    connect(errorsAction, SIGNAL(triggered(bool)), this, SLOT(updateLevels()));

    warningsAction = new QAction(tr("Warnings"), m_List);
    warningsAction->setCheckable(true);
    warningsAction->setChecked(reportLevels & StatusItem::WARNING);
    levelsMenu->addAction(warningsAction);
    connect(warningsAction, SIGNAL(triggered(bool)), this, SLOT(updateLevels()));

    messagesAction = new QAction(tr("Messages"), m_List);
    messagesAction->setCheckable(true);
    messagesAction->setChecked(reportLevels & StatusItem::MESSAGE);
    levelsMenu->addAction(messagesAction);
    connect(messagesAction, SIGNAL(triggered(bool)), this, SLOT(updateLevels()));

    m_List->setContextMenuPolicy( Qt::ActionsContextMenu );

    m_Layout->addWidget( m_List );
    resize( QSize( 800, 400 ) );
    //QFrame::show();
    setWindowTitle( tr("Messages - APEX" ));
}

StatusWindow::~StatusWindow()
{
    delete m_Model;
}

const QTreeView* StatusWindow::treeView() const
{
    return m_List;
}

void StatusWindow::addItem( const StatusItem* const a_cpItem )
{
    m_Model->addError( a_cpItem );
    //m_List->scrollTo( m_Model->GetLastItemIndex() );  //scrollToBottom doesn't work: it skips one or two lines
    m_List->scrollToBottom();
    m_List->resizeColumnToContents(2);

//     if (!isVisible())
//         show();
}

void StatusWindow::clearList()
{
    m_Model->clear();
}

void StatusWindow::closeEvent(QCloseEvent* e)
{
    hide();
    e->accept();
}

void StatusWindow::copyAllToClipboard() {
    QString buffer;
    
    for (int i=0; i<m_Model->getNumErrors(); ++i) {
        buffer += m_Model->getError(i).toString() + "\n";
    }

    //qDebug("Copy: ");
    //qDebug(qPrintable(buffer));
    
    QApplication::clipboard()->setText(buffer);
}

void StatusWindow::copySelectedToClipboard()
{
    QStringList result;
    const QItemSelectionModel * const selection = m_List->selectionModel();
    for (int i = 0; i < m_Model->getNumErrors(); ++i)
        if (selection->isSelected (m_Model->index (i, 0)))
            result.append (m_Model->getError(i).toString());

    QApplication::clipboard()->setText (result.join ("\n"));
}

void StatusWindow::updateLevels()
{
    StatusItem::Levels levels;

    if (errorsAction->isChecked())
        levels |= StatusItem::ERROR;
    if (warningsAction->isChecked())
        levels |= StatusItem::WARNING;
    if (messagesAction->isChecked())
        levels |= StatusItem::MESSAGE;

    emit levelsChanged(levels);
}


