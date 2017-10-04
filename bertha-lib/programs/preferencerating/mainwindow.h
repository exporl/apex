#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "bertha/experimentprocessor.h"

#include <QMainWindow>
#include <QScopedPointer>

using namespace bertha;

class QButtonGroup;
class QString;
class QFile;

namespace Ui
{
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

    QScopedPointer<ExperimentProcessor> processor;
    ExperimentData createExperiment(const QString &file1Name,
                                    const QString &file2Name);
    void initFileNames(); // Assign filenames to the placeholders file1Name and
                          // file2Name

protected:
    void changeEvent(QEvent *e);

private slots:
    void onAlgSelected(void); // Action to take when an algorithm is selected.
    void on_butChoose_clicked(void); // An algorithm choice is confirmed.
    void on_butNext_clicked(void);   // Go to next experiment.
    void onPrefSelected(void); // Action after clicking a preference rating.
    void start();              // Initalise and start sound delivery.
    void
    on_butPlySndA_clicked(); // Play sound A or, if already playing, stop it.
    void
    on_butPlySndB_clicked(); // Play sound B or, if already playing, stop it.

private:
    Ui::MainWindow *ui;

    int isSelected, isRated, // Condition examination for appropriate action.
        expIdx, nExp;        // Number of experiments to be conducted.

    bool isPlayingA, isPlayingB; // Boolean tests to see if the respective
                                 // sounds are playing.

    int *arrExpIdx,    // Experiment index for randomisation.
        *arrSigIdx,    // Signal order index. Enough to store one value for
                       // pairwise comparison.
        *arrResult[2]; // Result storage.

    QFile *outFile;   // The file where the results are stored.
    QString subjName, // The name of the test subject. Output file contains this
                      // name.
        file1Name, file2Name; // The sound files for testing.
};

#endif // MAINWINDOW_H
