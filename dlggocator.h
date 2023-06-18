
#ifndef DLGGOCATOR_H
#define DLGGOCATOR_H

#include <QDialog>
#include <QTcpSocket>


QT_BEGIN_NAMESPACE
namespace Ui { class DlgGocator; }
QT_END_NAMESPACE

class DlgGocator : public QDialog

{
    Q_OBJECT
private:
    enum
    {
        idle,
        start,
        trigger,
        endCycle,
        results,
        stop
    } mCommand;
public:
    DlgGocator(QWidget *parent = nullptr);
    ~DlgGocator();
private:
    void readAswResults(const QString& answer);
private slots:
    void readAnswer();

private:
    int mSelectedCamera;    // camera selected: 0 - no camera
    double mGap;
    double mFlush;

    QTcpSocket *tcpSocket = nullptr;
    QString mAnswer;
    bool mResultsAvailable;
private:
    Ui::DlgGocator *ui;
};

#endif // DLGGOCATOR_H
