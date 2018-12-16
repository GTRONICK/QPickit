#include <QDebug>
#include <QProcess>
#include "worker.h"

Worker::Worker(QObject *parent) : QObject(parent)
{
    giInfoFlag = 0;
}

/**
  Ejecuta el comando pk2cmd con los argumentos que llegan como
  argumento en un arreglo.
  @param aobArguments - QStringList, Arreglo de argumentos
*/
void Worker::worker_slot_executeCommand(QStringList aobArguments){

    gobProcess = new QProcess(this);
    QString lsProgram = "pk2cmd";
    QString lsCompleteCommand = lsProgram;
    QString lsExitStatus;
    bool lbReturnValue;

    for(int liIndex = 0; liIndex < aobArguments.size(); liIndex++){
        lsCompleteCommand += " "+aobArguments.at(liIndex);
    }
    emit worker_signal_prepareCommandExecution();
    gobProcess->setProcessChannelMode(QProcess::MergedChannels);
    connect(gobProcess,SIGNAL(readyReadStandardOutput()),this,SLOT(worker_slot_internalProcessOutputCapture()));
    gobProcess->start(lsProgram, aobArguments);
    if (!gobProcess->waitForFinished()){
        lsExitStatus = "ERROR: " + this->gobProcess->errorString();
        lbReturnValue = false;
    } else {
        lsExitStatus = "OK";
        lbReturnValue = true;
    }

    emit worker_signal_taskCompleted(lbReturnValue, lsExitStatus);
}

/**
  Ejecuta el comando para recuperar la información del
  Pickit2 conectado.
*/
void Worker::worker_slot_pickitInfo()
{
    giInfoFlag = 1;
    QStringList lobArguments;
    lobArguments << "-S";
    this->worker_slot_executeCommand(lobArguments);
}

/**
  Envía la salida del comando ejecutado al hilo principal.
*/
void Worker::worker_slot_internalProcessOutputCapture()
{
    if(giInfoFlag == 0) emit(worker_signal_processOutput(gobProcess->readAllStandardOutput()));
    else{
        emit(worker_signal_pickitInfo(gobProcess->readAllStandardOutput()));
        giInfoFlag = 0;
    }
}
