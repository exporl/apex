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

#ifndef _EXPORL_SRC_LIB_APEXMAIN_RESULT_RESULTVIEWER_H_
#define _EXPORL_SRC_LIB_APEXMAIN_RESULT_RESULTVIEWER_H_

#include "apextools/global.h"

#include <QByteArray>
#include <QDialog>
#include <QObject>
#include <QString>
#include <QUrl>

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
    class APEX_EXPORT ResultViewer: public QObject {
        Q_OBJECT                                        // we use the tr() function
    public:
        ResultViewer(const data::ResultParameters* p_param,
                const QString& p_resultfile);
        void show(bool ask);
        bool addtofile( const QString& p_filename);

        ~ResultViewer();

    private:
        bool ProcessResultJavascript();
        bool findResultPage();          // Look for result page in results file, using tag <jscript>
        QByteArray createCSVtext();

        void showJavascript();

        void setupDialog();
        void showDialog();

        const data::ResultParameters* m_pParam;
        QString m_sResultfile;          // Results file that is currently being used
        QUrl m_resultPagePath;          // HTML page to show results

        RTResultSink* m_rtr;
        QDialog* m_dialog;
        Ui_ResultViewer* m_ui;

    public slots:
        void loadFinished(bool ok);
        void exportToPdf();
        void print();

    signals:
        void errorMessage(const QString& source, const QString& message);

    };

}

#endif
