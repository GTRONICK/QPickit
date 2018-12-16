#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QFile>
#include <QFileDialog>
#include <QMessageBox>

/**
  Constructor de la clase MainWindow
  @param *parent - Apuntador al objeto padre.
*/
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    gobWorker = new Worker;
    workerThread = new QThread;
    gobWorker->moveToThread(workerThread);
    connect(gobWorker,SIGNAL(worker_signal_prepareCommandExecution()),this,SLOT(main_slot_prepareCommandExecution()));
    connect(gobWorker,SIGNAL(worker_signal_processOutput(QString)),this,SLOT(main_slot_processOutput(QString)));
    connect(this,SIGNAL(main_signal_pickitInfo()),gobWorker,SLOT(worker_slot_pickitInfo()));
    connect(gobWorker,SIGNAL(worker_signal_pickitInfo(QString)),this,SLOT(main_slot_pickitInfo(QString)));
    connect(this,SIGNAL(main_signal_executeCommand(QStringList)),gobWorker,SLOT(worker_slot_executeCommand(QStringList)));
    connect(gobWorker,SIGNAL(worker_signal_taskCompleted(bool,QString)),this,SLOT(main_slot_taskCompleted(bool,QString)));
    connect(workerThread,SIGNAL(finished()),gobWorker,SLOT(deleteLater()));
    connect(workerThread,SIGNAL(finished()),workerThread,SLOT(deleteLater()));
    workerThread->start();

    QFile styleFile("style.qss");
    if(styleFile.exists() && styleFile.open(QFile::ReadOnly)){
        QString StyleSheet = QLatin1String(styleFile.readAll());
        this->setStyleSheet(StyleSheet);
    }else{
        this->ui->defaultThemeRadioButton->setChecked(true);
        this->ui->customThemeRadioButton->setChecked(false);
    }
}

/**
  Habilita o deshabilita todos los botones del tab principal
  para evitar envíos de comandos indeseados durante la
  ejecución de un proceso.
  @param abValue - true: Habilita todos los botones,
                   false: Deshabilita todos los botones.
*/
void MainWindow::main_slot_enableAllButtons(bool abValue)
{
    ui->contactButton->setEnabled(abValue);
    ui->programButton->setEnabled(abValue);
    ui->verifyButton->setEnabled(abValue);
    ui->eraseButton->setEnabled(abValue);
    ui->readButton->setEnabled(abValue);
    ui->hexFileButton->setEnabled(abValue);
    ui->resetButton->setEnabled(abValue);
}

/**
  Destructor de la clase
*/
MainWindow::~MainWindow()
{
    delete ui;
    workerThread->quit();
    workerThread->wait();
}

/**
  Evento de click para el botón HEX File...
*/
void MainWindow::on_hexFileButton_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open Hex"), "", tr("Hex files (*.hex)"));
    ui->hexFileLineEdit->setText(fileName);
}

/**
  Evento de click para el botón Burn
*/
void MainWindow::on_programButton_clicked()
{
    QDir lobDir(QCoreApplication::applicationDirPath());
    if(ui->hexFileLineEdit->text().isNull() || ui->hexFileLineEdit->text().isEmpty()){
        gsHexFileName = QFileDialog::getOpenFileName(this, tr("Open Hex"), "", tr("Hex files (*.hex)"));
        ui->hexFileLineEdit->setText(gsHexFileName);
    } else {
        gsHexFileName = ui->hexFileLineEdit->text();
    }

    if(!gsHexFileName.isNull() && !gsHexFileName.isEmpty()){
        gsHexFileName = lobDir.relativeFilePath(ui->hexFileLineEdit->text());

        gobArguments << "-p" << "-f" << gsHexFileName << "-m" << "-r";
        emit(main_signal_executeCommand(gobArguments));
    }else{
        QMessageBox::critical(this,"ERROR","Please, select a valid Hex file");
    }
}

/**
  Evento de click para el botón Contact
*/
void MainWindow::on_contactButton_clicked()
{
    gobArguments << "-p" << "-i";
    emit(main_signal_executeCommand(gobArguments));
}

/**
  Evento de click para el botón Erase
*/
void MainWindow::on_eraseButton_clicked()
{
    gobArguments << "-p" << "-e";
    emit(main_signal_executeCommand(gobArguments));
}

/**
  Evento de click para el botón Reset
*/
void MainWindow::on_resetButton_clicked()
{
    gobArguments << "-p" << "-r";
    emit(main_signal_executeCommand(gobArguments));
}

/**
  Evento de click para el botón Verify
*/
void MainWindow::on_verifyButton_clicked()
{
    QDir lobDir(QCoreApplication::applicationDirPath());

    if(ui->hexFileLineEdit->text().isNull() || ui->hexFileLineEdit->text().isEmpty()){
        gsHexFileName = QFileDialog::getOpenFileName(this, tr("Open Hex"), "", tr("Hex files (*.hex)"));
        ui->hexFileLineEdit->setText(gsHexFileName);
    } else {
        gsHexFileName = ui->hexFileLineEdit->text();
    }

    if(!gsHexFileName.isNull() && !gsHexFileName.isEmpty()){
        gsHexFileName = lobDir.relativeFilePath(ui->hexFileLineEdit->text());

        gobArguments << "-p" << "-f" << gsHexFileName << "-y";
        emit(main_signal_executeCommand(gobArguments));
    }else{
        QMessageBox::critical(this,"ERROR","Please, select a valid Hex file");
    }
}

/**
  Evento de click para el botón Read
*/
void MainWindow::on_readButton_clicked()
{
    QDir lobDir(QCoreApplication::applicationDirPath());
    gsHexFileName = QFileDialog::getSaveFileName(this, tr("Save Hex"), "", tr("Hex files (*.hex)"));
    if(!gsHexFileName.isNull() && !gsHexFileName.isEmpty()){
        if(!gsHexFileName.contains(".hex")){
            gsHexFileName += ".hex";
        }
        ui->hexFileLineEdit->setText(gsHexFileName);
        gsHexFileName = lobDir.relativeFilePath(ui->hexFileLineEdit->text());

        gobArguments << "-p" << "-gf" << gsHexFileName;
        emit(main_signal_executeCommand(gobArguments));
    }else{
        QMessageBox::critical(this,"ERROR","Please, select a valid Hex file");
    }
}

/**
  Evento de click para el botón Detect programmer
*/
void MainWindow::on_detectPickitButton_clicked()
{
    ui->pickitInfoTextArea->clear();
    emit(main_signal_pickitInfo());
}

/**
  Prepara a la aplicación para ejecutar el siguiente
  comando, borrando el log de eventos, deshabilitando
  todos los botones del tab principal y limpiando
  el arreglo de argumentos.
*/
void MainWindow::main_slot_prepareCommandExecution()
{
    this->main_slot_enableAllButtons(false);
    ui->logTextArea->clear();
    gobArguments.clear();
}

/**
  Slot que se ejecuta cuando el comando actual ha sido
  notificado como terminado por la clase Worker.
  @param abExitStatus - true: El comando se ejecutó correctamente.
                        false: El comando falló.
         asExitString - Cadena que contiene el texto del error producido.
*/
void MainWindow::main_slot_taskCompleted(bool abExitStatus, QString asExitString)
{
    this->main_slot_enableAllButtons(true);
    if(!abExitStatus) {
        ui->logTextArea->appendPlainText(asExitString);
    }
}

/**
  Evento de click para el botón About
*/
void MainWindow::on_aboutButton_clicked()
{
    const char *lsHelpText = ("<h2>QPickit 2.1</h2>"
                "<p>pk2cmd GUI for Linux"
                "<br>Pickit 2 clone compatible"
                "<p> Author: Jaime A. Quiroga P."
                "<br><a href=\"http://www.gtronick.com\">www.gtronick.com</a>");

    QMessageBox::about(this, tr("About QPickit 2.1"),
    tr(lsHelpText));
}

/**
  Evento de click para establecer el tema por defecto
*/
void MainWindow::on_defaultThemeRadioButton_clicked()
{
    this->setStyleSheet("");
}

/**
  Evento de click para establecer un tema personalizado
*/
void MainWindow::on_customThemeRadioButton_clicked()
{
    QFile styleFile("style.qss");
    if(styleFile.exists() && styleFile.open(QFile::ReadOnly)){
        QString StyleSheet = QLatin1String(styleFile.readAll());
        this->setStyleSheet(StyleSheet);
    }
}

/**
  Slot que agrega la salida del comando ejecutado en
  el visor del log, y en la barra de estado.
  @param asCommandOutput - String que contiene la salida
  del comando ejecutado.
*/
void MainWindow::main_slot_processOutput(QString asCommandOutput)
{
    if(!asCommandOutput.isNull() && asCommandOutput != "\n") {
        ui->logTextArea->appendPlainText(asCommandOutput);
        ui->statusBar->showMessage(asCommandOutput.simplified(),2000);
    }
}

/**
  Slot que inserta la información del pickit2 conectado
  actualmente, al visor de log.
  @param asPicKitInfo - String que contiene la información
  recuperada del PicKit2 conectado.
*/
void MainWindow::main_slot_pickitInfo(QString asPicKitInfo)
{
    ui->pickitInfoTextArea->appendPlainText(asPicKitInfo);
}


