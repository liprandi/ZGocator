#ifndef DLGJOBNAME_H
#define DLGJOBNAME_H

#include <QDialog>

namespace Ui {
class DlgJobName;
}

class DlgJobName : public QDialog
{
    Q_OBJECT

public:
    explicit DlgJobName(QWidget *parent = nullptr);
    ~DlgJobName();
public:
    const QString jobName();
private:
    Ui::DlgJobName *ui;

};

#endif // DLGJOBNAME_H
