/******************************************************************************
 * Copyright (C) 2008  Tom Francart <tom.francart@med.kuleuven.be>            *
 *                                                                            *
 * This file is part of APEX 4.                                               *
 *                                                                            *
 * APEX 4 is free software: you can redistribute it and/or modify             *
 * it under the terms of the GNU General Public License as published by       *
 * the Free Software Foundation, either version 2 of the License, or          *
 * (at your option) any later version.                                        *
 *                                                                            *
 * APEX 4 is distributed in the hope that it will be useful,                  *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of             *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the              *
 * GNU General Public License for more details.                               *
 *                                                                            *
 * You should have received a copy of the GNU General Public License          *
 * along with APEX 4.  If not, see <http://www.gnu.org/licenses/>.            *
 *****************************************************************************/

#include "apexdata/screen/parameterlistelement.h"

#include "apexdata/stimulus/stimulusparameters.h"

#include "parameters/parametermanager.h"

#include "runner/experimentrundelegate.h"

#include "screen/screenrundelegate.h"

#include "stimulus/stimulus.h"

#include "parameterlistrundelegate.h"

#include <QAbstractTableModel>
#include <QHeaderView>
#include <QList>

namespace
{
struct ModelData {
    QString id;
    QString name;
    QString value;
};
}

namespace apex
{
namespace rundelegates
{

class ParameterListRunDelegate::Model : public QAbstractTableModel,
                                        public QList<ModelData>
{
public:
    int rowCount(const QModelIndex &) const;
    int columnCount(const QModelIndex &) const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    QVariant headerData(int section, Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const;
    void dataChanged();

    int rowIndex(const QString &id);
};

int ParameterListRunDelegate::Model::rowCount(const QModelIndex &index) const
{
    if (index.isValid())
        return 0;
    else
        return count();
}
int ParameterListRunDelegate::Model::columnCount(const QModelIndex &index) const
{
    if (index.isValid())
        return 0;
    else
        return 2;
}

void ParameterListRunDelegate::Model::dataChanged()
{
    beginResetModel();
    endResetModel();
}

int ParameterListRunDelegate::Model::rowIndex(const QString &id)
{
    for (int i = 0; i < length(); ++i)
        if (at(i).id == id)
            return i;
    return -1;
}

QVariant ParameterListRunDelegate::Model::data(const QModelIndex &index,
                                               int role) const
{
    if (role != Qt::DisplayRole)
        return QVariant();
    switch (index.column()) {
    case 0:
        return at(index.row()).name;
    case 1:
        return at(index.row()).value;
    default:
        Q_ASSERT(false);
        return QVariant();
    }
}

QVariant ParameterListRunDelegate::Model::headerData(
    int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole)
        return QVariant();
    if (orientation != Qt::Horizontal)
        return QVariant();
    switch (section) {
    case 0:
        return tr("Name");
    case 1:
        return tr("Value");
    default:
        return QVariant();
    }
}

/* class LabelDelegate : public QItemDelegate
{


    public:
        LabelDelegate(QObject *parent = 0);

        QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem
&option,
                              const QModelIndex &index) const;

        void setEditorData(QWidget *editor, const QModelIndex &index) const;
        void setModelData(QWidget *editor, QAbstractItemModel *model,
                          const QModelIndex &index) const;

        void updateEditorGeometry(QWidget *editor,
                                  const QStyleOptionViewItem &option, const
QModelIndex &index) const;
};

QWidget *LabelDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem
&option,
                      const QModelIndex &index) const {


                      }

                      void LabelDelegate::setEditorData(QWidget *editor, const
QModelIndex &index) const;
                      void LabelDelegate::setModelData(QWidget *editor,
QAbstractItemModel *model,
                          const QModelIndex &index) const;

                      void LabelDelegate::updateEditorGeometry(QWidget *editor,
                                  const QStyleOptionViewItem &option, const
QModelIndex &index) const;

*/

ParameterListRunDelegate::ParameterListRunDelegate(
    ExperimentRunDelegate *p_exprd, const ParameterListElement *e,
    QWidget *parent)
    : QTableView(parent),
      ScreenElementRunDelegate(p_exprd, e),
      element(e),
      model(new Model)
{
    QTableView::setObjectName(element->getID());
    setStyleSheet(element->getStyle());
    setModel(model);
    verticalHeader()->hide();
    horizontalHeader()->setSectionsClickable(false);
    resizeColumnsToContents();

    for (ParameterListElement::ParameterListT::const_iterator it =
             element->getParameterList().begin();
         it != element->getParameterList().end(); ++it) {
        ModelData d;

        d.name = (*it).GetName();
        d.id = (*it).id;
        model->push_back(d);
    }
    model->dataChanged();
    resizeColumnsToContents();

    ParameterManager *mgr = p_exprd->GetParameterManager();
    connect(mgr, SIGNAL(parameterChanged(QString, QVariant)), this,
            SLOT(updateParameter(QString, QVariant)));

    // FIXME: set initial parameter values
}

ParameterListRunDelegate::~ParameterListRunDelegate()
{
}

QWidget *ParameterListRunDelegate::getWidget()
{
    return this;
}

// void ParameterListRunDelegate::newStimulus( stimulus::Stimulus* stimulus )
void ParameterListRunDelegate::updateParameter(const QString &id,
                                               const QVariant &value)
{
    QString newValue(value.toString());

    for (int i = 0; i < model->length(); ++i) {
        if (model->at(i).id == id) {

            QString expression(element->getParameterList().at(i).expression);
            if (!expression.isEmpty()) {
                newValue = QString::number(
                    ParseExpression(expression, value.toDouble()));
            } else {
                newValue = value.toString();
            }

            model->operator[](i).value = newValue;
        }
    }

    model->dataChanged();
    resizeColumnsToContents();
}

const ScreenElement *ParameterListRunDelegate::getScreenElement() const
{
    return element;
}

/*void ParameterListRunDelegate::connectSlots( gui::ScreenRunDelegate* d )
{
    connect( d, SIGNAL( newStimulus( stimulus::Stimulus* ) ),
             this, SLOT( newStimulus( stimulus::Stimulus* ) ) );
}*/
}
}
