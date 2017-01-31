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
 
#include "parameterlistrundelegate.h"

#include "screen/parameterlistelement.h"
#include "screen/screenrundelegate.h"
#include "stimulus/stimulusparameters.h"
#include "stimulus/stimulus.h"

#include "runner/experimentrundelegate.h"
#include "parameters/parametermanager.h"

#include <QList>
#include <QAbstractTableModel>
#include <QHeaderView>

#include <assert.h>

namespace
{
    struct ModelData
    {
        QString name;
        QString value;
    };
}

namespace apex
{
namespace rundelegates
{

class ParameterListRunDelegate::Model
    : public QAbstractTableModel, public QList<ModelData>
{
public:
    int rowCount( const QModelIndex& ) const;
    int columnCount( const QModelIndex& ) const;
    QVariant data (
        const QModelIndex & index, int role = Qt::DisplayRole ) const;
    QVariant headerData(
        int section, Qt::Orientation orientation, int role = Qt::DisplayRole ) const;
    void dataChanged();
};

int ParameterListRunDelegate::Model::rowCount( const QModelIndex& index ) const {
    if ( index.isValid() )
        return 0;
    else
        return count();
}
int ParameterListRunDelegate::Model::columnCount( const QModelIndex& index ) const {
    if ( index.isValid() )
        return 0;
    else
        return 2;
}

void ParameterListRunDelegate::Model::dataChanged() {
    reset();
}

QVariant ParameterListRunDelegate::Model::data(
    const QModelIndex& index, int role ) const
{
    if ( role != Qt::DisplayRole ) return QVariant();
    switch( index.column() )
    {
    case 0:
        return at( index.row() ).name;
    case 1:
        return at( index.row() ).value;
    default:
        assert( false );
        return QVariant();
    }
}

QVariant ParameterListRunDelegate::Model::headerData(
    int section, Qt::Orientation orientation, int role ) const
{
    if ( role != Qt::DisplayRole ) return QVariant();
    if ( orientation != Qt::Horizontal ) return QVariant();
    switch( section )
    {
    case 0:
        return tr( "Name" );
    case 1:
        return tr( "Value" );
    default:
        return QVariant();
    }
}


/* class LabelDelegate : public QItemDelegate
{
    Q_OBJECT

    public:
        LabelDelegate(QObject *parent = 0);

        QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option,
                              const QModelIndex &index) const;

        void setEditorData(QWidget *editor, const QModelIndex &index) const;
        void setModelData(QWidget *editor, QAbstractItemModel *model,
                          const QModelIndex &index) const;

        void updateEditorGeometry(QWidget *editor,
                                  const QStyleOptionViewItem &option, const QModelIndex &index) const;
};

QWidget *LabelDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option,
                      const QModelIndex &index) const {


                      }

                      void LabelDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const;
                      void LabelDelegate::setModelData(QWidget *editor, QAbstractItemModel *model,
                          const QModelIndex &index) const;

                      void LabelDelegate::updateEditorGeometry(QWidget *editor,
                                  const QStyleOptionViewItem &option, const QModelIndex &index) const;

*/

ParameterListRunDelegate::ParameterListRunDelegate(
        ExperimentRunDelegate* p_exprd,
    const ParameterListElement* e, QWidget* parent ) :
      QTableView( parent ),
      ScreenElementRunDelegate(p_exprd, e),
      element( e ),
      model( new Model )
{
    QTableView::setObjectName(element->getID());
    setStyleSheet(element->getStyle());
    setModel( model );
    verticalHeader()->hide();
    horizontalHeader()->setClickable( false );
    resizeColumnsToContents();
}

ParameterListRunDelegate::~ParameterListRunDelegate()
{
}

QWidget* ParameterListRunDelegate::getWidget()
{
    return this;
}

void ParameterListRunDelegate::newStimulus( stimulus::Stimulus* stimulus )
{
#ifdef PRINTPARAMETERLIST
    qDebug("ApexParameterlist::NewStimulus: Getting parameters from stimulus id=" + stimulus->GetID());
#endif
    model->clear();
    const data::StimulusParameters* varParam = stimulus->GetVarParameters();
    const data::StimulusParameters* fixParam = stimulus->GetFixParameters();

    for ( ParameterListElement::ParameterListT::const_iterator it=
              element->getParameterList().begin();
          it!=element->getParameterList().end(); ++it)
    {
        ModelData d;
        QString value;

        QVariant pmvalue( m_rd->GetParameterManager()->parameterValue((*it).id));
        if (pmvalue.isValid()){
            value=pmvalue.toString();
        } else if ( varParam->contains((*it).id))
        {
            value=varParam->value((*it).id).toString();
        } else if (fixParam->contains((*it).id)) {
            value=fixParam->value((*it).id).toString();
        }

        if (! (*it).expression.isEmpty()) {
            double newvalue=ParseExpression((*it).expression,value.toDouble());
            value=QString::number(newvalue);
        }

        d.name = (*it).GetName();
        d.value = value;
        model->push_back( d );
    }
    model->dataChanged();
    resizeColumnsToContents();
}

const ScreenElement* ParameterListRunDelegate::getScreenElement() const
{
    return element;
}

void ParameterListRunDelegate::connectSlots( gui::ScreenRunDelegate* d )
{
    connect( d, SIGNAL( newStimulus( stimulus::Stimulus* ) ),
             this, SLOT( newStimulus( stimulus::Stimulus* ) ) );
}

}
}

