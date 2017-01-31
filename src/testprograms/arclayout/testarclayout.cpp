/******************************************************************************
 * Copyright (C) 2007  Michael Hofmann <mh21@piware.de>                       *
 *                                                                            *
 * This program is free software; you can redistribute it and/or modify       *
 * it under the terms of the GNU General Public License as published by       *
 * the Free Software Foundation; either version 2 of the License, or          *
 * (at your option) any later version.                                        *
 *                                                                            *
 * This program is distributed in the hope that it will be useful,            *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of             *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the              *
 * GNU General Public License for more details.                               *
 *                                                                            *
 * You should have received a copy of the GNU General Public License along    *
 * with this program; if not, write to the Free Software Foundation, Inc.,    *
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.                *
 ******************************************************************************/

#include "gui/arclayout.h"

#include <QApplication>
#include <QDebug>
#include <QDialog>
#include <QHBoxLayout>
#include <QPushButton>
#include <QTextEdit>

class ArcLayoutTest : public QDialog
{
    Q_OBJECT
public:
    ArcLayoutTest();

public Q_SLOTS:
    void addButton();
    void removeButton();
    void topLayout();
    void rightLayout();
    void bottomLayout();
    void leftLayout();
    void fullLayout();

private:
    ArcLayout *arcLayout;
};

ArcLayoutTest::ArcLayoutTest()
{
    QVBoxLayout *mainLayout = new QVBoxLayout (this);
    mainLayout->setMargin (0);
    mainLayout->setSpacing (0);

    arcLayout = new ArcLayout (ArcLayout::ARC_FULL);
    mainLayout->addLayout (arcLayout);

    QPushButton * const button = new QPushButton ("+", this);
    button->setSizePolicy (QSizePolicy::Minimum, QSizePolicy::Minimum);
    arcLayout->addWidget (button);
    connect (button, SIGNAL (clicked()), this, SLOT (addButton()));

    mainLayout->addWidget (new QTextEdit (this));
}

void ArcLayoutTest::addButton()
{
    QPushButton * const button =
        new QPushButton (this);
    button->setSizePolicy (QSizePolicy::Minimum, QSizePolicy::Minimum);
    arcLayout->addWidget (button);

    switch (arcLayout->count()) {
    case 2:
        button->setText ("-");
        connect (button, SIGNAL (clicked()), this, SLOT (removeButton()));
        break;
    case 3:
        button->setText ("top");
        connect (button, SIGNAL (clicked()), this, SLOT (topLayout()));
        break;
    case 4:
        button->setText ("right");
        connect (button, SIGNAL (clicked()), this, SLOT (rightLayout()));
        break;
    case 5:
        button->setText ("bottom");
        connect (button, SIGNAL (clicked()), this, SLOT (bottomLayout()));
        break;
    case 6:
        button->setText ("left");
        connect (button, SIGNAL (clicked()), this, SLOT (leftLayout()));
        break;
    case 7:
        button->setText ("full");
        connect (button, SIGNAL (clicked()), this, SLOT (fullLayout()));
        break;
    default:
        button->setText (QString::number (arcLayout->count() + 1));
    }
}

void ArcLayoutTest::removeButton()
{
    if (arcLayout->count() > 1)
        arcLayout->itemAt (arcLayout->count() - 1)->widget()->deleteLater();
}

void ArcLayoutTest::topLayout()
{
    arcLayout->setArcType (ArcLayout::ARC_TOP);
}

void ArcLayoutTest::rightLayout()
{
    arcLayout->setArcType (ArcLayout::ARC_RIGHT);
}

void ArcLayoutTest::bottomLayout()
{
    arcLayout->setArcType (ArcLayout::ARC_BOTTOM);
}

void ArcLayoutTest::leftLayout()
{
    arcLayout->setArcType (ArcLayout::ARC_LEFT);
}

void ArcLayoutTest::fullLayout()
{
    arcLayout->setArcType (ArcLayout::ARC_FULL);
}



int main (int argc, char* argv[]) {
    QApplication app (argc, argv);

    ArcLayoutTest window;
    window.show();

    return app.exec();
}

#include "testarclayout.moc"
