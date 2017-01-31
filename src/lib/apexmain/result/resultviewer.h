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

#ifndef APEXRESULTVIEWER_H
#define APEXRESULTVIEWER_H

#include <qstring.h>
#include <qobject.h>
#include <QByteArray>
#include <QUrl>
#include <QDialog>

class Ui_ResultViewer;

namespace apex {
    namespace data
    {
        class ResultParameters;
    }

    class RTResultSink;

    /**
      @author Tom Francart,,,
      */
    class ResultViewer: QObject {
        Q_OBJECT                                        // we use the tr() function
    public:
        ResultViewer(data::ResultParameters* p_param,
                const QString& p_resultfile,
                const QString& p_xsltpath=QString());
        void show(bool ask);
        bool addtofile( const QString& p_filename);
        bool ProcessResult();
        const QString GetResultHtml() const;

        ~ResultViewer();

    private:
        bool ProcessResultXslt();
        bool ProcessResultJavascript();

        void showXslt();
        void showJavascript();

        data::ResultParameters* m_pParam;
        QString m_sResultfile;
        QString m_sXsltPath;
        QUrl m_resultPagePath;

        QByteArray m_result_html;
        QByteArray m_result_text;

        bool m_xsltOK;
        bool m_javascriptOK;

        RTResultSink* m_rtr;
        QDialog* m_dialog;
        Ui_ResultViewer* m_ui;

    public slots:
        void loadFinished(bool ok);
        void exportToPdf();
        void print();

    };

}

#endif
