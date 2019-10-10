#include "numerickeypadwidget.h"

#include <QKeyEvent>
#include <QPushButton>

NumericKeypadWidget::NumericKeypadWidget(QWidget *parent) : QWidget(parent)
{
    ui.setupUi(this);

    connect(ui.button_0, &QPushButton::clicked, [&]() { addNumber(0); });
    connect(ui.button_1, &QPushButton::clicked, [&]() { addNumber(1); });
    connect(ui.button_2, &QPushButton::clicked, [&]() { addNumber(2); });
    connect(ui.button_3, &QPushButton::clicked, [&]() { addNumber(3); });
    connect(ui.button_4, &QPushButton::clicked, [&]() { addNumber(4); });
    connect(ui.button_5, &QPushButton::clicked, [&]() { addNumber(5); });
    connect(ui.button_6, &QPushButton::clicked, [&]() { addNumber(6); });
    connect(ui.button_7, &QPushButton::clicked, [&]() { addNumber(7); });
    connect(ui.button_8, &QPushButton::clicked, [&]() { addNumber(8); });
    connect(ui.button_9, &QPushButton::clicked, [&]() { addNumber(9); });
    connect(ui.button_back, &QPushButton::clicked, [&]() { removeLast(); });
    connect(ui.button_ok, &QPushButton::clicked, this,
            &NumericKeypadWidget::submitTriplet);

    updateWidgets();
}

QString NumericKeypadWidget::getTriplet() const
{
    return QString::number(triplet.at(0)) + QString::number(triplet.at(1)) +
           QString::number(triplet.at(2));
}

void NumericKeypadWidget::addNumber(unsigned number)
{
    if (triplet.size() >= 3)
        return;

    triplet.push_back(number);
    updateWidgets();
}

void NumericKeypadWidget::removeLast()
{
    if (triplet.empty())
        return;

    triplet.pop_back();
    updateWidgets();
}

void NumericKeypadWidget::updateWidgets()
{
    ui.label_1->clear();
    ui.label_2->clear();
    ui.label_3->clear();
    if (triplet.size() >= 1)
        ui.label_1->setText(QString::number(triplet.at(0)));
    if (triplet.size() >= 2)
        ui.label_2->setText(QString::number(triplet.at(1)));
    if (triplet.size() >= 3)
        ui.label_3->setText(QString::number(triplet.at(2)));
    ui.button_ok->setEnabled(triplet.size() == 3);
    ui.button_back->setEnabled(!triplet.empty());
}

void NumericKeypadWidget::clear()
{
    triplet.clear();
    updateWidgets();
}

void NumericKeypadWidget::keyPressEvent(QKeyEvent *event)
{
    switch (event->key()) {
    case Qt::Key_Backspace:
        removeLast();
        break;
    case Qt::Key_0:
        addNumber(0);
        break;
    case Qt::Key_1:
        addNumber(1);
        break;
    case Qt::Key_2:
        addNumber(2);
        break;
    case Qt::Key_3:
        addNumber(3);
        break;
    case Qt::Key_4:
        addNumber(4);
        break;
    case Qt::Key_5:
        addNumber(5);
        break;
    case Qt::Key_6:
        addNumber(6);
        break;
    case Qt::Key_7:
        addNumber(7);
        break;
    case Qt::Key_8:
        addNumber(8);
        break;
    case Qt::Key_9:
        addNumber(9);
        break;
    case Qt::Key_Return:
        submitTriplet();
        break;
    case Qt::Key_Enter:
        submitTriplet();
        break;
    }

    QWidget::keyPressEvent(event);
}

void NumericKeypadWidget::submitTriplet()
{
    if (triplet.size() == 3)
        emit tripletSubmitted();
}

void NumericKeypadWidget::setFont(const QFont &font)
{
    // TODO: figure out how to let changes to font propagate to all children
    ui.button_1->setFont(font);
    ui.button_2->setFont(font);
    ui.button_3->setFont(font);
    ui.button_4->setFont(font);
    ui.button_5->setFont(font);
    ui.button_6->setFont(font);
    ui.button_7->setFont(font);
    ui.button_8->setFont(font);
    ui.button_9->setFont(font);
    ui.button_0->setFont(font);
    ui.button_back->setFont(font);
    ui.button_ok->setFont(font);
    ui.label_1->setFont(font);
    ui.label_2->setFont(font);
    ui.label_3->setFont(font);

    QWidget::setFont(font);
}