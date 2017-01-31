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

#include "connection/connection.h"

#include "parameters/parametermanager.h"

#include "runner/experimentrundelegate.h"

#include "services/filedialog.h"

#include "connectiondialog.h"
#include "exceptions.h"
#include "ui_connectiondialog.h"

#include <cmath>

#include <QGraphicsSvgItem>
#include <QMessageBox>
#include <QProcess>
#include <QPushButton>
#include <QSettings>
#include <QSvgRenderer>

namespace apex
{

class ConnectionDialogPrivate : public QObject
{
    Q_OBJECT
public:
    ConnectionDialogPrivate (QDialog *pub);
    QByteArray runDot (const QStringList &arguments, const QByteArray &data,
            bool mergeError);

public Q_SLOTS:
    void fit();
    void zoomIn();
    void zoomOut();
    void scale (double scaleFactor);
    void save();

public:
    QDialog * const pub;
    bool dpiFix; // TODO: this can be removed for dot >= 2.16
    Ui::ConnectionDialog ui;
    QByteArray svg;
    QString dotPath;
    QGraphicsSvgItem *svgItem;
    QSvgRenderer *renderer;
};

// ConnectionDialogPrivate =====================================================

ConnectionDialogPrivate::ConnectionDialogPrivate (QDialog *pub) :
    pub (pub),
    dpiFix (false)
{
    dotPath = "dot";
#ifdef Q_OS_WIN
    QSettings settings ("HKEY_LOCAL_MACHINE\\Software\\ATT\\Graphviz",
            QSettings::NativeFormat);
    if (settings.contains ("InstallPath"))
        dotPath = QDir (settings.value ("InstallPath").toString())
            .absoluteFilePath ("bin/dot.exe");

    if (!QFile::exists(dotPath)) {
        int result = QMessageBox::critical(0, tr("Could not start dot"),
                             tr("To show connection graphs, APEX uses the dot tool, which is part of graphviz. "
                                     "APEX could not find graphiviz on you system. "
                                     "If it is not installed, you can download it from http://www.graphviz.org "
                                     "restart APEX and try again.\n"
                                      "Alternatively, use the open button to select the path to dot manually."),
                                      QMessageBox::Ok | QMessageBox::Open,
                                      QMessageBox::Ok);
        if (result== QMessageBox::Open) {
            dotPath = QFileDialog::getOpenFileName(0, tr("Select location of dot.exe"),
                    dotPath, "Dot (dot.exe)");
        }

    }
#endif
}

QByteArray ConnectionDialogPrivate::runDot (const QStringList &arguments,
        const QByteArray &data, bool mergeError)
{
    QProcess dot;
    if (mergeError)
        dot.setProcessChannelMode(QProcess::MergedChannels);
    dot.start (dotPath, arguments);
    if (!dot.waitForStarted())
        throw ApexStringException ("Unable to start graph visualization tool");

    if (dot.write (data) != data.size())
        throw ApexStringException ("Unable to use graph visualization tool");
    dot.closeWriteChannel();
    if (!dot.waitForFinished())
        throw ApexStringException ("Unable to use graph visualization tool");
    return dot.readAll();
}

void ConnectionDialogPrivate::fit()
{
    ui.graphicsView->fitInView (ui.graphicsView->sceneRect(),
            Qt::KeepAspectRatio);
}

void ConnectionDialogPrivate::zoomIn()
{
    scale (sqrt (2.0));
}

void ConnectionDialogPrivate::zoomOut()
{
    scale (1 / sqrt (2.0));
}

void ConnectionDialogPrivate::scale (double scaleFactor)
{
    ui.graphicsView->scale (scaleFactor, scaleFactor);
}

void ConnectionDialogPrivate::save()
{
    const QString filePath = FileDialog::Get().mf_sGetAnyFile("*.svg",
            tr("SVG graphics files (*.svg)"));
    if (!filePath.isEmpty()) {
        QFile file (filePath);
        if (!file.open (QIODevice::WriteOnly) ||
            file.write (svg) != svg.length())
            QMessageBox::critical (pub, tr ("Unable to write SVG file"),
             tr ("Unable to write SVG file: %1").arg (file.errorString()));
        file.close();
    }
}

// ConnectionDialog ============================================================

ConnectionDialog::ConnectionDialog (QWidget *parent) :
    QDialog (parent),
    d (new ConnectionDialogPrivate (this))
{
    d->ui.setupUi (this);

    QPushButton *save = new QPushButton (tr ("Save..."), this);
    connect (save, SIGNAL (clicked()), d.get(), SLOT (save()));
    d->ui.buttonBox->addButton (save, QDialogButtonBox::ActionRole);
    QPushButton *zoomIn = new QPushButton (tr ("Zoom in"), this);
    connect (zoomIn, SIGNAL (clicked()), d.get(), SLOT (zoomIn()));
    d->ui.buttonBox->addButton (zoomIn, QDialogButtonBox::ActionRole);
    QPushButton *zoomOut = new QPushButton (tr ("Zoom out"), this);
    connect (zoomOut, SIGNAL (clicked()), d.get(), SLOT (zoomOut()));
    d->ui.buttonBox->addButton (zoomOut, QDialogButtonBox::ActionRole);

    QByteArray version = d->runDot (QStringList() << "-V", QByteArray(), true);
    if (version.contains ("version 2.12"))
        d->dpiFix = true;

    d->renderer = new QSvgRenderer (this);
    QGraphicsScene *scene = new QGraphicsScene (this);
    d->ui.graphicsView->setScene (scene);
    d->svgItem = new QGraphicsSvgItem;
    scene->addItem (d->svgItem);
}

void ConnectionDialog::setDelegate (const ExperimentRunDelegate &delegate)
{
    QStringList dotInput;

    dotInput << "digraph connections {";
    dotInput << "rankdir=LR;";
    dotInput << "ranksep=1.5;";
    dotInput << "node [shape=plaintext];";
    QMap<QString, unsigned> idMap;
    QMap<QString, unsigned> portMap;
    const ParameterManager * const manager = delegate.GetParameterManager();
    stimulus::tConnectionsMap connections = delegate.GetConnections();
    QMapIterator<QString, stimulus::tConnections> i (connections);
    while (i.hasNext()) {
        i.next();
        Q_FOREACH (const stimulus::tConnection &connection, i.value()) {
            if (!idMap.contains (connection.m_sFromID)) {
                idMap.insert (connection.m_sFromID, idMap.size());
                portMap.insert (connection.m_sFromID,
                        connection.m_nFromChannel);
            } else {
                portMap[connection.m_sFromID] = qMax
                    (portMap[connection.m_sFromID], connection.m_nFromChannel);
            }
            if (!idMap.contains (connection.m_sToID)) {
                idMap.insert (connection.m_sToID, idMap.size());
                portMap.insert (connection.m_sToID,
                        connection.m_nToChannel);
            } else {
                portMap[connection.m_sToID] = qMax
                    (portMap[connection.m_sToID], connection.m_nToChannel);
            }
            dotInput << QString ("n%1:o%2 -> n%3:i%4 [arrowsize=0.5];")
                .arg (idMap.value (connection.m_sFromID))
                .arg (connection.m_nFromChannel)
                .arg (idMap.value (connection.m_sToID))
                .arg (connection.m_nToChannel);
        }
    }

    QMapIterator<QString, unsigned> j (idMap);
    while (j.hasNext()) {
        j.next();

        data::ParameterValueMap map = manager->parametersForOwner (j.key());
        QStringList parameters;
		
        QMapIterator<data::Parameter, QVariant> i (map);
        while (i.hasNext()) {
            i.next();
            parameters.append (QString ("%1: %2")
                    .arg (i.key().toString(), i.value().toString()));
        }

        QString parameterString = parameters.isEmpty() ? QString() :
            QString ("<br/><font color=\"gray\" point-size=\"12\">%1</font>")
            .arg (parameters.join ("<br/>"));

        const unsigned maxPorts = portMap.value (j.key());
        QStringList inputs;
        for (unsigned k = 0; k < maxPorts + 1; ++k)
            inputs.append (QString ("<td port=\"i%1\"><font point-size=\"8\">"
                        "%2</font></td>").arg (k).arg (k ));
        QStringList outputs;
        for (unsigned k = 0; k < maxPorts + 1; ++k)
            outputs.append (QString ("<td port=\"o%1\"><font point-size=\"8\">"
                        "%2</font></td>").arg (k).arg (k ));
        QString label;
        label += "<<table cellspacing=\"0\">";
        label += QString ("<tr>%2<td rowspan=\"%1\">%3%4</td>%5</tr>")
            .arg (maxPorts + 1).arg (inputs.takeFirst(), j.key(),
                    parameterString, outputs.takeFirst());
        for (unsigned k = 1; k < maxPorts + 1; ++k)
            label += QString ("<tr>%1%2</tr>")
                .arg (inputs.takeFirst(), outputs.takeFirst());
        label += "</table>>";
        dotInput << QString ("n%1 [label=%2];") .arg (j.value()).arg (label);
    }

    dotInput << "}";

    QStringList parameters ("-Tsvg");
    // QtSvg convertToPixels() thinks 1pt=1.25px,1in=72pt->1in=90px, which
    // doesn't work too well with graphviz 2.12
    if (d->dpiFix) {
        qDebug ("Fixing dpi");
        parameters.append ("-Gdpi=90");
    }

    d->svg = d->runDot (parameters, dotInput.join ("\n").toUtf8(), false);

    d->renderer->load (d->svg);
    // Invalidate cache
    d->svgItem->setSharedRenderer (d->renderer);
    // We have do delay the fit somewhat, otherwise the QScrollArea's viewport
    // is not yet updated
    QMetaObject::invokeMethod (d.get(), "fit", Qt::QueuedConnection);
}

ConnectionDialog::~ConnectionDialog()
{
}

} // namespace apex

#include "connectiondialog.moc"
