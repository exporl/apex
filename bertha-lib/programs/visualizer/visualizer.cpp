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

#include "bertha/xmlloader.h"

#include "common/exception.h"

#include "ui_visualizer.h"
#include "visualizer.h"

#include <QFile>
#include <QFileDialog>
#include <QGraphicsSvgItem>
#include <QMessageBox>
#include <QProcess>
#include <QPushButton>
#include <QSettings>
#include <QSvgRenderer>

#include <cmath>

using namespace bertha;
using namespace cmn;

class VisualizerPrivate : public QObject
{
    Q_OBJECT
public:
    VisualizerPrivate(Visualizer *pub);
    QByteArray runDot(const QStringList &arguments, const QByteArray &data,
                      bool mergeError);

public Q_SLOTS:
    void fit();
    void zoomIn();
    void zoomOut();
    void scale(double scaleFactor);
    void save();
    void open();

public:
    Visualizer *const pub;
    bool dpiFix; // TODO: this can be removed for dot >= 2.16
    Ui::Visualizer ui;
    QByteArray svg;
    QString dotPath;
    QGraphicsSvgItem *svgItem;
    QSvgRenderer *renderer;
};

// VisualizerPrivate ===========================================================

VisualizerPrivate::VisualizerPrivate(Visualizer *pub) : pub(pub), dpiFix(false)
{
    dotPath = QString::fromLatin1("dot");
#ifdef Q_OS_WIN
    QSettings settings(QString::fromLatin1("HKEY_LOCAL_MACHINE\\Software\\ATT\\"
                                           "Graphviz"),
                       QSettings::NativeFormat);
    if (settings.contains(QString::fromLatin1("InstallPath")))
        dotPath =
            QDir(settings.value(QString::fromLatin1("InstallPath")).toString())
                .absoluteFilePath(QString::fromLatin1("bin/dot.exe"));

    if (!QFile::exists(dotPath)) {
        int result = QMessageBox::critical(
            0, tr("Could not start dot"),
            tr("To show connection graphs, APEX uses the dot tool, which is "
               "part of graphviz. "
               "APEX could not find graphiviz on you system. "
               "If it is not installed, you can download it from "
               "http://www.graphviz.org "
               "restart APEX and try again.\n"
               "Alternatively, use the open button to select the path to dot "
               "manually."),
            QMessageBox::Ok | QMessageBox::Open, QMessageBox::Ok);
        if (result == QMessageBox::Open) {
            dotPath = QFileDialog::getOpenFileName(
                0, tr("Select location of dot.exe"), dotPath,
                QString::fromLatin1("Dot (dot.exe)"));
        }
    }
#endif
}

QByteArray VisualizerPrivate::runDot(const QStringList &arguments,
                                     const QByteArray &data, bool mergeError)
{
    QProcess dot;
    if (mergeError)
        dot.setProcessChannelMode(QProcess::MergedChannels);
    dot.start(dotPath, arguments);
    if (!dot.waitForStarted())
        throw Exception(tr("Unable to start graph visualization tool"));

    if (dot.write(data) != data.size())
        throw Exception(tr("Unable to use graph visualization tool"));
    dot.closeWriteChannel();
    if (!dot.waitForFinished())
        throw Exception(tr("Unable to use graph visualization tool"));
    return dot.readAll();
}

void VisualizerPrivate::fit()
{
    ui.graphicsView->fitInView(ui.graphicsView->sceneRect(),
                               Qt::KeepAspectRatio);
}

void VisualizerPrivate::zoomIn()
{
    scale(sqrt(2.0));
}

void VisualizerPrivate::zoomOut()
{
    scale(1 / sqrt(2.0));
}

void VisualizerPrivate::scale(double scaleFactor)
{
    ui.graphicsView->scale(scaleFactor, scaleFactor);
}

void VisualizerPrivate::save()
{
    const QString filePath = QFileDialog::getSaveFileName(
        NULL, tr("Open File"), tr("../../data"), tr("*.svg"));
    if (!filePath.isEmpty()) {
        QFile file(filePath);
        if (!file.open(QIODevice::WriteOnly) || file.write(svg) != svg.length())
            QMessageBox::critical(
                pub, tr("Unable to write SVG file"),
                tr("Unable to write SVG file: %1").arg(file.errorString()));
        file.close();
    }
}

void VisualizerPrivate::open()
{
    const QString filePath = QFileDialog::getOpenFileName(
        NULL, tr("Open File"), tr("../../data/experiments"), tr("*.xml"));
    pub->setExperimentFile(filePath);
}

// Visualizer ==================================================================

Visualizer::Visualizer(QWidget *parent)
    : QDialog(parent), d(new VisualizerPrivate(this))
{
    d->ui.setupUi(this);

    QPushButton *open = new QPushButton(tr("Open..."), this);
    connect(open, SIGNAL(clicked()), d, SLOT(open()));
    d->ui.buttonBox->addButton(open, QDialogButtonBox::ActionRole);
    QPushButton *save = new QPushButton(tr("Save..."), this);
    connect(save, SIGNAL(clicked()), d, SLOT(save()));
    d->ui.buttonBox->addButton(save, QDialogButtonBox::ActionRole);
    QPushButton *zoomIn = new QPushButton(tr("Zoom in"), this);
    connect(zoomIn, SIGNAL(clicked()), d, SLOT(zoomIn()));
    d->ui.buttonBox->addButton(zoomIn, QDialogButtonBox::ActionRole);
    QPushButton *zoomOut = new QPushButton(tr("Zoom out"), this);
    connect(zoomOut, SIGNAL(clicked()), d, SLOT(zoomOut()));
    d->ui.buttonBox->addButton(zoomOut, QDialogButtonBox::ActionRole);

    QByteArray version = d->runDot(QStringList() << QString::fromLatin1("-V"),
                                   QByteArray(), true);
    if (version.contains("version 2.12"))
        d->dpiFix = true;

    d->renderer = new QSvgRenderer(this);
    QGraphicsScene *scene = new QGraphicsScene(this);
    d->ui.graphicsView->setScene(scene);
    d->svgItem = new QGraphicsSvgItem;
    scene->addItem(d->svgItem);
}

void Visualizer::setExperimentFile(QString file)
{
    QStringList dotInput;
    XmlLoader loader;
    ExperimentData data = loader.loadFile(file);

    dotInput << QString::fromLatin1("digraph connections {");
    dotInput << QString::fromLatin1("rankdir=LR;");
    dotInput << QString::fromLatin1("ranksep=1.5;");
    dotInput << QString::fromLatin1("node [shape=plaintext];");

    QMap<QString, unsigned> idMap;
    QMap<QString, unsigned> portMap;

    QString device = data.device().id();
    QList<ConnectionData> connections = data.connections();
    Q_FOREACH (const ConnectionData &connection, connections) {
        QString sourceName = connection.sourceBlock();
        QString targetName = connection.targetBlock();
        if (sourceName == device)
            sourceName = QString::fromLatin1("device_input");
        if (targetName == device)
            targetName = QString::fromLatin1("device_output");

        int sourcePort;
        int targetPort;
        QString number =
            connection.sourcePort().split(QChar::fromLatin1('-'))[1];
        sourcePort = number.toInt() - 1;
        number = connection.targetPort().split(QChar::fromLatin1('-'))[1];
        targetPort = number.toInt() - 1;

        if (!idMap.contains(sourceName)) {
            idMap.insert(sourceName, idMap.size());
            portMap.insert(sourceName, sourcePort);
        } else {
            portMap[sourceName] =
                qMax(portMap[sourceName], unsigned(sourcePort));
        }
        if (!idMap.contains(targetName)) {
            idMap.insert(targetName, idMap.size());
            portMap.insert(targetName, targetPort);
        } else {
            portMap[targetName] =
                qMax(portMap[targetName], unsigned(targetPort));
        }
        dotInput << QString::fromLatin1("n%1:o%2 -> n%3:i%4 [arrowsize=0.5];")
                        .arg(idMap.value(sourceName))
                        .arg(sourcePort)
                        .arg(idMap.value(targetName))
                        .arg(targetPort);
    }

    QMapIterator<QString, unsigned> j(idMap);
    while (j.hasNext()) {
        j.next();
        QString blockName = j.key();
        if (blockName == QString::fromLatin1("device_input") ||
            blockName == QString::fromLatin1("device_output"))
            blockName = device;
        QMap<QString, QVariant> map = data.block(blockName).parameters();
        QStringList parameters;

        QMapIterator<QString, QVariant> i(map);
        while (i.hasNext()) {
            i.next();
            parameters.append(QString::fromLatin1("%1: %2").arg(
                i.key(), i.value().toString()));
        }

        QString parameterString =
            parameters.isEmpty()
                ? QString()
                : QString::fromLatin1(
                      "<br/><font color=\"gray\" point-size=\"12\">%1"
                      "</font>")
                      .arg(parameters.join(QString::fromLatin1("<br/>")));

        const unsigned maxPorts = portMap.value(j.key());
        QStringList inputs;
        for (unsigned k = 0; k < maxPorts + 1; ++k)
            inputs.append(QString::fromLatin1("<td port=\"i%1\"><font point-"
                                              "size=\"8\">%2</font></td>")
                              .arg(k)
                              .arg(k));
        QStringList outputs;
        for (unsigned k = 0; k < maxPorts + 1; ++k)
            outputs.append(QString::fromLatin1("<td port=\"o%1\"><font point-"
                                               "size=\"8\">%2</font></td>")
                               .arg(k)
                               .arg(k));
        QString label;
        label += QString::fromLatin1("<<table cellspacing=\"0\">");
        label += QString::fromLatin1("<tr>%2<td rowspan=\"%1\">%3%4</td>"
                                     "%5</tr>")
                     .arg(maxPorts + 1)
                     .arg(inputs.takeFirst(), j.key(), parameterString,
                          outputs.takeFirst());
        for (unsigned k = 1; k < maxPorts + 1; ++k)
            label += QString::fromLatin1("<tr>%1%2</tr>")
                         .arg(inputs.takeFirst(), outputs.takeFirst());
        label += QString::fromLatin1("</table>>");
        dotInput
            << QString::fromLatin1("n%1 [label=%2];").arg(j.value()).arg(label);
    }

    dotInput << QString::fromLatin1("}");

    QStringList parameters(QString::fromLatin1("-Tsvg"));
    // QtSvg convertToPixels() thinks 1pt=1.25px,1in=72pt->1in=90px, which
    // doesn't work too well with graphviz 2.12
    if (d->dpiFix) {
        qCDebug(EXPORL_BERTHA, "Fixing dpi");
        parameters.append(QString::fromLatin1("-Gdpi=90"));
    }

    d->svg = d->runDot(
        parameters, dotInput.join(QString::fromLatin1("\n")).toUtf8(), false);

    d->renderer->load(d->svg);
    // Invalidate cache
    d->svgItem->setSharedRenderer(d->renderer);
    // We have do delay the fit somewhat, otherwise the QScrollArea's viewport
    // is not yet updated
    QMetaObject::invokeMethod(d, "fit", Qt::QueuedConnection);
}

Visualizer::~Visualizer()
{
    delete d;
}

#include "visualizer.moc"
