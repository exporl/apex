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
 
#include <QPushButton>
#include <QApplication>
#include <QGridLayout>
#include <QResizeEvent>

class PictureButtonWidget: public QPushButton {    
    public:
        PictureButtonWidget();
         void resizeEvent ( QResizeEvent * event );
};

PictureButtonWidget::PictureButtonWidget()
{
   QIcon i("1.png");
   setIcon(i);
   setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

   setFlat(true);
// setStyleSheet("border:0px; border-style:none; border-width:0px;");
}

void PictureButtonWidget::resizeEvent ( QResizeEvent * event ) {
    if (event)
        QPushButton::resizeEvent(event);
    
    QSize s = event->size();

    // adding the following makes it work, but the 40 is arbitrary...
    s.setWidth(s.width()-40);
    s.setHeight(s.height()-40);
    
    setIconSize(s);
}


int main(int argc, char** argv) {
    QApplication app(argc, argv);
    
    QWidget* main = new QWidget();
    QGridLayout* l = new QGridLayout(main);
    
    PictureButtonWidget* w = new PictureButtonWidget();
    l->addWidget(w);
    
    main->show();
    return app.exec();
    
}
