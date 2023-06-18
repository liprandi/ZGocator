
#include <QMessageBox>
#include <QStringView>
#include "dlggocator.h"
#include "dlgjobname.h"
#include "./ui_dlggocator.h"


DlgGocator::DlgGocator(QWidget *parent)
    : QDialog(parent)
    , mCommand(idle)
    , mSelectedCamera(0)
    , tcpSocket(new QTcpSocket(this))
    , ui(new Ui::DlgGocator)
{
    mGap = mFlush = 0.;
    mResultsAvailable = false;
    ui->setupUi(this);
    ui->pbStart->setEnabled(false);
    ui->pbStop->setEnabled(false);
    ui->pbTrigger->setEnabled(false);
    ui->pbResults->setEnabled(false);
    ui->pbLoadJob->setEnabled(false);
    ui->pbStamp->setEnabled(false);
    connect(ui->cbSelCamera, &QComboBox::currentIndexChanged, this, [&](int idx)
    {
        if(mSelectedCamera > 0)
        {
             mGap = mFlush = 0.;
        }
        mSelectedCamera = idx;
        ui->pbStart->setEnabled(mSelectedCamera > 0);
        ui->pbStop->setEnabled(mSelectedCamera > 0);
        ui->pbTrigger->setEnabled(mSelectedCamera > 0);
        ui->pbResults->setEnabled(mSelectedCamera > 0);
        ui->pbLoadJob->setEnabled(mSelectedCamera > 0);
        ui->pbStamp->setEnabled(mSelectedCamera > 0);
        tcpSocket->abort();
        if(mSelectedCamera > 0)
        {
            QString host[4] = {"121.31.78.82", "121.31.78.86", "121.31.78.90", "121.31.78.94"};
            int port[4] = {8190, 8190, 8190, 8190};
            if(mSelectedCamera > 0)
            {
                 tcpSocket->connectToHost(host[mSelectedCamera - 1],
                                          port[mSelectedCamera - 1]);
            }
        }
    });
    connect(tcpSocket, &QIODevice::readyRead, this, &DlgGocator::readAnswer);
    //! [2] //! [4]
    connect(tcpSocket, &QAbstractSocket::errorOccurred, this, [&](QAbstractSocket::SocketError socketError)
    {
        switch (socketError) {
        case QAbstractSocket::RemoteHostClosedError:
            break;
        case QAbstractSocket::HostNotFoundError:
            QMessageBox::information(this, tr("Gocator G&F"),
                                     tr("The host was not found. Please check the "
                                        "host name and port settings."));
            break;
        case QAbstractSocket::ConnectionRefusedError:
            QMessageBox::information(this, tr("Gocator G&F"),
                                     tr("The connection was refused by the peer. "
                                        "Make sure the fortune server is running, "
                                        "and check that the host name and port "
                                        "settings are correct."));
            break;
        default:
            QMessageBox::information(this, tr("Gocator G&F"),
                                     tr("The following error occurred: %1.")
                                         .arg(tcpSocket->errorString()));
        }
    });
    connect(ui->pbStart, &QPushButton::pressed, this, [&]()
    {
        if(tcpSocket->isValid() && tcpSocket->isOpen())
        {
            tcpSocket->write("Start\n");
            mCommand = start;
        }
    });
    connect(ui->pbTrigger, &QPushButton::pressed, this, [&]()
    {
        if(tcpSocket->isValid() && tcpSocket->isOpen())
        {
            tcpSocket->write("Trigger\n");
            mCommand = trigger;
        }
    });
    connect(ui->pbStop, &QPushButton::pressed, this, [&]()
    {
        if(tcpSocket->isValid() && tcpSocket->isOpen())
        {
            tcpSocket->write("Stop\n");
            mCommand = stop;
        }
    });
    connect(ui->pbResults, &QPushButton::pressed, this, [&]()
    {
        if(tcpSocket->isValid() && tcpSocket->isOpen())
        {
            tcpSocket->write("Result,130,131\n");
            mCommand = results;
        }
    });
    connect(ui->pbLoadJob, &QPushButton::pressed, this, [&]()
    {
        if(tcpSocket->isValid() && tcpSocket->isOpen())
        {
            DlgJobName dlg;
            if(dlg.exec() == DlgJobName::Accepted)
            {
                QString snd = "LoadJob," + dlg.jobName() + "\n";
                tcpSocket->write(snd.toLocal8Bit());
            }
        }
        mCommand = idle;
    });
    connect(ui->pbStamp, &QPushButton::pressed, this, [&]()
    {
        if(tcpSocket->isValid() && tcpSocket->isOpen())
        {
            tcpSocket->write("Stamp\n");
            mCommand = idle;
        }
    });

}

DlgGocator::~DlgGocator()
{
    delete ui;
}

void DlgGocator::readAswResults(const QString &answer)
{
    QStringList l = answer.split(',');
    if(l.count() > 3)
    {
        if(!l[1].compare("M130") && l[3][1] == '1')
        {
            double gap;
            bool ok;
            QStringView v = l[2].mid(1);
            gap = v.toDouble(&ok);
            if(ok)
                ui->lbGap->setText(QString::number(gap / 1000., 'f', 3));
        }
    }
    if(l.count() > 6)
    {
        if(!l[4].compare("M131") && l[6][1] == '1')
        {
            double flush;
            bool ok;
            QStringView v = l[5].mid(1);
            flush = v.toDouble(&ok);
            if(ok)
                ui->lbFlush->setText(QString::number(flush / 1000., 'f', 3));
        }
    }
}
void DlgGocator::readAnswer()
{
    mAnswer = tcpSocket->readAll();

    ui->lbMessages->setText(mAnswer);
    bool ok = false;
    QStringView v = mAnswer.left(2);
    if(!v.compare("OK"))
        ok = true;
    switch(mCommand)
    {
    case idle:
        break;
    case start:
    case stop:
        mCommand = idle;
        mResultsAvailable = false;
        break;
    case trigger:
        if(ok)
        {
            if(mAnswer.length() == 3)
                mCommand = endCycle;
            else
            {
                mCommand = idle;
                ui->lbMessages->setText(tr("Results available"));
                mResultsAvailable = true;
            }
        }
        else
        {
            mCommand = idle;
            mResultsAvailable = false;
        }
        break;
    case endCycle:
        mCommand = idle;
        ui->lbMessages->setText(tr("Results available"));
        break;
    case results:
        if(ok)
            readAswResults(mAnswer);
        mCommand = idle;
    }
};


