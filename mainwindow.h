#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QThread>
#include "worker.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void main_slot_enableAllButtons(bool abValue);
    void on_hexFileButton_clicked();
    void on_programButton_clicked();
    void on_aboutButton_clicked();
    void on_customThemeRadioButton_clicked();
    void on_defaultThemeRadioButton_clicked();
    void main_slot_processOutput(QString asCommandOutput);
    void main_slot_pickitInfo(QString asPicKitInfo);
    void on_contactButton_clicked();
    void on_eraseButton_clicked();
    void on_resetButton_clicked();
    void on_verifyButton_clicked();
    void on_readButton_clicked();
    void on_detectPickitButton_clicked();
    void main_slot_prepareCommandExecution();
    void main_slot_taskCompleted(bool abExitStatus, QString asExitString);

signals:
    void main_signal_executeCommand(QStringList aobArguments);
    void main_signal_pickitInfo();


private:
    Ui::MainWindow *ui;
    QString gsHexFileName;
    QThread *workerThread;
    QStringList gobArguments;
    Worker *gobWorker;
};

#endif // MAINWINDOW_H
